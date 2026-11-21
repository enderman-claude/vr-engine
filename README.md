# vr-engine

Experimental VR game engine. Renders most of each frame once on a shared (optionally curved) layer, then finishes per-eye with a cheap correction pass.

Status: early development. See `NOTES.md` for milestones and decisions.

## Build

```
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Options:
- `VRE_BUILD_TESTS` (default ON)
- `VRE_ENABLE_STEAMVR` (default ON; stub until milestone 2)

## License

MIT. See `LICENSE`.
