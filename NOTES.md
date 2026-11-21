# Engine working notes

Living document. Update at the end of every work session. Newest entries at the top of each section.

## Status
- Milestone 0 (in progress): repo, license, CMake skeleton, CI on Linux and Windows.

## Milestones
- **M0 Foundation** — repo, MIT license, CMake, CI (Linux + Windows), null backend, tests. *Current.*
- **M1 Core runtime** — job system (dependency graph, default-parallel), frame loop, capability flags, config/compile-time settings.
- **M2 SteamVR backend** — real OpenVR/SteamVR session, pose tracking, submission.
- **M3 Shared-layer renderer** — curved common pass, per-eye dirty stage, HUD layer split, cross-eye reuse rules.
- **M4 Flatscreen mode** — flat projection path, eye/no-eye/no-render compile modes.
- **M5 Generic OpenXR backend** — runtimes other than SteamVR (Monado on Linux first).
- **M6 Companion app integration** — streaming link to vr-companion.
- **M7 Android / Quest** — Meta Horizon OS backend, Android XR vendor layer. Stubs exist from M0.
- **M8 macOS** — stubs only; no commitment.

## Decisions
- Platforms: Windows, Linux, Quest, Android (official). Flatscreen is supported as a mode. macOS: stubs only.
- Backend order: SteamVR first, then OpenXR last (owner decision).
- Backends report capabilities as flags; core code never checks the platform.
- License: MIT. Permissive dependencies only (MIT, BSD, Apache-2.0, zlib, or similar). GPL is acceptable per owner if needed, but avoid it in the default build.
- Curvature: flat and VR presets are engine-managed. Manual curvature control is allowed but emits a compiler warning (owner decision).
- Flatscreen runs the shared pass with a flat projection (curvature radius = infinity); no per-eye stage.
- Multithreading: job system should make parallelism the default. Per-eye stage and frame submission stay on a dedicated high-priority thread.

## Open questions
- Build-time or runtime selection of curvature (owner to decide).
- Whether the repos become public (affects license visibility).
- Dependencies to evaluate for integration (OpenVR, a job system library, a math library, a test framework).

## Session log
- Session 1: created repos, license, CMake skeleton, null and SteamVR-stub backends, tests, CI workflow.
