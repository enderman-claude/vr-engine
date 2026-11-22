# Engine plans

Owner decisions are marked (owner). Status is as of the last session in NOTES.md.

## Platforms
- Official: Windows, Linux, Meta Quest, Android XR headsets (owner).
- Flatscreen is a supported mode (owner).
- macOS: stubs only, no commitment (owner).

## Backend order
1. SteamVR backend (OpenVR) first (owner). Initializes and falls back cleanly. Frame submission is a stub.
2. OpenXR generic backend last (owner). Monado is the Linux candidate.
3. Quest and Android XR: vendor layers on the shared Android/OpenXR path.

## Rendering model
- Most of the frame is rendered once on a shared, curved layer. A cheap per-eye stage finishes each eye.
- The shared pass renders with per-eye coordinates (owner), so parallax is handled in the shared pass.
- The per-eye stage is a dirty correction. Anything it doesn't correct must be listed explicitly.
- Full-eye effects (vignettes, fades, grading) run per eye.
- HUD: head-locked elements on the shared layer. Stereo-dependent elements (crosshair convergence, world markers) per eye.
- HUD elements are layers, and their depth can be set (owner).

## Curvature (owner)
- Two engine presets: flat and VR. Developers may change them.
- Default is chosen automatically. Flatscreen uses flat, which is the infinite-radius limit with no warp.
- Manual curvature control is allowed but triggers a compiler warning on any use.
- Build-time default, with a runtime override available (owner answer: "both, build-time as default").

## Flatscreen and eye modes (owner)
- Compile-time modes: stereo, right eye to desktop, left eye to desktop, variable (requires code), no VR render, no render.
- Flatscreen runs the same pipeline with one view. No per-eye stage in flatscreen.

## Packaging (owner)
- Studio output: a single-file binary, or an archive folder (20+ files in subfolders).
- Both work on Windows and Linux.
- The single-file binary carries its own asset table. Archives need no manifest (owner).
- Games are a runtime executable linked against the engine. Packaged games use the same host.

## Multithreading (owner)
- Make multithreading suspiciously easy: a job system where parallelism is the default.
- Per-eye stage and frame submission stay on a dedicated high-priority thread.

## Companion app (owner)
- Headset only: Meta Quest and Android XR headsets. Not a phone app, like Steam Link on a headset.
- Streaming transport: ALVR (MIT), approved. Pinned to v20.14.1 (commit a9f6542), not yet integrated.
- Shows device GPU, CPU, memory, and thermal data (native Vulkan via the NDK).
- Builds signed release APKs with the shared Android signing identity.

## Test game
- VR Snake is the official test game (owner). Logic and tests exist. Headset rendering does not exist yet.

## Dependencies (policy)
- Permissive, mature open source is used without per-item approval (owner waiver).
- GPL, LGPL, AGPL, and unlicensed code need explicit approval.
- Each dependency is pinned to a commit and recorded in THIRD_PARTY_NOTICES.md with its license text.
