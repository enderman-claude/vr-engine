#include "packager.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace studio {

namespace {

// Single-file layout: [runtime bytes][entries...][trailer]
// entry:   u32 path length, path bytes (UTF-8, '/' separators), u64 size, data bytes
// trailer: u64 table offset, u64 entry count, 8-byte magic
constexpr char kMagic[8] = {'V', 'R', 'E', 'P', 'A', 'K', '0', '1'};
constexpr std::uintmax_t kTrailerSize = 24;
constexpr std::size_t kMaxPathLength = 4096;

struct Entry {
    std::string rel;  // relative path with '/' separators, the same on every OS
    fs::path abs;
};

PackageResult fail(const std::string& message) {
    PackageResult r;
    r.error = message;
    return r;
}

std::vector<Entry> collect(const fs::path& root) {
    std::vector<Entry> entries;
    for (const auto& de : fs::recursive_directory_iterator(root)) {
        if (!de.is_regular_file()) continue;
        entries.push_back({fs::relative(de.path(), root).generic_string(), de.path()});
    }
    // Sorted so the output is deterministic on every platform.
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) { return a.rel < b.rel; });
    return entries;
}

bool copy_stream(std::istream& in, std::ostream& out, std::uintmax_t bytes) {
    std::vector<char> buffer(64 * 1024);
    while (bytes > 0) {
        const auto chunk = static_cast<std::streamsize>(std::min<std::uintmax_t>(bytes, buffer.size()));
        if (!in.read(buffer.data(), chunk)) return false;
        if (!out.write(buffer.data(), chunk)) return false;
        bytes -= static_cast<std::uintmax_t>(chunk);
    }
    return true;
}

void put_u32(std::ostream& o, std::uint32_t v) {
    for (int i = 0; i < 4; ++i) o.put(static_cast<char>((v >> (8 * i)) & 0xFFu));
}

void put_u64(std::ostream& o, std::uint64_t v) {
    for (int i = 0; i < 8; ++i) o.put(static_cast<char>((v >> (8 * i)) & 0xFFu));
}

bool get_bytes(std::istream& in, void* dst, std::size_t n) {
    return static_cast<bool>(in.read(static_cast<char*>(dst), static_cast<std::streamsize>(n)));
}

bool get_u32(std::istream& in, std::uint32_t& v) {
    unsigned char b[4];
    if (!get_bytes(in, b, 4)) return false;
    v = 0;
    for (int i = 0; i < 4; ++i) v |= static_cast<std::uint32_t>(b[i]) << (8 * i);
    return true;
}

bool get_u64(std::istream& in, std::uint64_t& v) {
    unsigned char b[8];
    if (!get_bytes(in, b, 8)) return false;
    v = 0;
    for (int i = 0; i < 8; ++i) v |= static_cast<std::uint64_t>(b[i]) << (8 * i);
    return true;
}

void make_executable(const fs::path& p) {
#ifndef _WIN32
    std::error_code ec;
    fs::permissions(p,
                    fs::perms::owner_all | fs::perms::group_read | fs::perms::group_exec | fs::perms::others_read |
                        fs::perms::others_exec,
                    fs::perm_options::replace, ec);
#else
    (void)p;  // Windows decides executability from the file extension.
#endif
}

PackageResult package_archive(const std::vector<Entry>& entries, const fs::path& runtime, const fs::path& out) {
    std::error_code ec;
    fs::create_directories(out, ec);
    if (ec) return fail("cannot create " + out.string() + ": " + ec.message());

    const fs::path runtime_dst = out / runtime.filename();
    fs::copy_file(runtime, runtime_dst, fs::copy_options::overwrite_existing, ec);
    if (ec) return fail("cannot copy runtime: " + ec.message());
    make_executable(runtime_dst);

    PackageResult r;
    for (const auto& e : entries) {
        const fs::path dst = out / fs::path(e.rel);
        fs::create_directories(dst.parent_path(), ec);
        if (ec) return fail("cannot create folder for " + e.rel + ": " + ec.message());
        fs::copy_file(e.abs, dst, fs::copy_options::overwrite_existing, ec);
        if (ec) return fail("cannot copy " + e.rel + ": " + ec.message());
        ++r.files;
    }
    r.ok = true;
    return r;
}

PackageResult package_single(const std::vector<Entry>& entries, const fs::path& runtime, const fs::path& out) {
    std::ifstream rt(runtime, std::ios::binary);
    if (!rt) return fail("cannot read runtime " + runtime.string());
    std::ofstream o(out, std::ios::binary | std::ios::trunc);
    if (!o) return fail("cannot write " + out.string());

    std::error_code ec;
    const auto runtime_size = fs::file_size(runtime, ec);
    if (ec || !copy_stream(rt, o, runtime_size)) return fail("cannot copy runtime");

    const std::uint64_t table_offset = runtime_size;
    for (const auto& e : entries) {
        std::ifstream in(e.abs, std::ios::binary);
        if (!in) return fail("cannot read " + e.abs.string());
        const auto size = fs::file_size(e.abs, ec);
        if (ec) return fail("cannot stat " + e.abs.string());
        if (e.rel.size() > kMaxPathLength) return fail("path too long: " + e.rel);

        put_u32(o, static_cast<std::uint32_t>(e.rel.size()));
        o.write(e.rel.data(), static_cast<std::streamsize>(e.rel.size()));
        put_u64(o, size);
        if (!copy_stream(in, o, size)) return fail("cannot pack " + e.rel);
    }
    put_u64(o, table_offset);
    put_u64(o, entries.size());
    o.write(kMagic, sizeof(kMagic));
    o.close();
    if (!o) return fail("cannot finish " + out.string());

    make_executable(out);
    PackageResult r;
    r.ok = true;
    r.files = entries.size();
    return r;
}

}  // namespace

PackageResult package_project(const fs::path& project_dir,
                              const fs::path& runtime,
                              const fs::path& out,
                              PackageKind kind) {
    try {
        std::error_code ec;
        if (!fs::is_directory(project_dir, ec)) return fail("project directory not found: " + project_dir.string());
        if (!fs::is_regular_file(runtime, ec)) return fail("runtime not found: " + runtime.string());
        const auto entries = collect(project_dir);
        return kind == PackageKind::Archive ? package_archive(entries, runtime, out)
                                            : package_single(entries, runtime, out);
    } catch (const fs::filesystem_error& err) {
        return fail(err.what());
    }
}

bool verify_single_file(const fs::path& file, std::size_t& entries) {
    std::error_code ec;
    const auto total = fs::file_size(file, ec);
    if (ec || total < kTrailerSize) return false;

    std::ifstream in(file, std::ios::binary);
    if (!in) return false;
    in.seekg(static_cast<std::streamoff>(total - kTrailerSize));
    std::uint64_t table_offset = 0;
    std::uint64_t count = 0;
    char magic[8];
    if (!get_u64(in, table_offset) || !get_u64(in, count) || !get_bytes(in, magic, 8)) return false;
    if (!std::equal(magic, magic + 8, kMagic)) return false;
    if (table_offset > total - kTrailerSize) return false;

    in.seekg(static_cast<std::streamoff>(table_offset));
    for (std::uint64_t i = 0; i < count; ++i) {
        std::uint32_t path_len = 0;
        if (!get_u32(in, path_len) || path_len > kMaxPathLength) return false;
        std::string rel(path_len, '\0');
        if (!get_bytes(in, rel.data(), path_len)) return false;
        std::uint64_t size = 0;
        if (!get_u64(in, size)) return false;
        in.seekg(static_cast<std::streamoff>(size), std::ios::cur);
        if (!in) return false;
    }
    entries = static_cast<std::size_t>(count);
    return static_cast<std::uint64_t>(in.tellg()) == total - kTrailerSize;
}

}  // namespace studio
