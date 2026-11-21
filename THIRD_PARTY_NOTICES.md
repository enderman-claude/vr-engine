# Third-party notices

All dependencies are fetched at build time with CMake `FetchContent`, pinned to the exact commit below. No third-party source is copied into this repository. License texts are kept in `third_party/licenses/`.

| Component | Used for | Version (tag) | Commit | License | Source | License text |
|---|---|---|---|---|---|---|
| GLM | Math (matrices, vectors) | 1.0.3 | `8d1fd52e5ab5590e2c81768ace50c72bae28f2ed` | MIT or Happy Bunny (dual; we use MIT) | https://github.com/g-truc/glm | `third_party/licenses/glm-copying.txt` |
| OpenVR | SteamVR backend | v2.15.6 | `0924064316de3effbcd1acf1e309182a2deb1c05` | BSD-3-Clause | https://github.com/ValveSoftware/openvr | `third_party/licenses/openvr-LICENSE.txt` |
| enkiTS | Job system | v1.12 | `0289cf6ffce86697e1f46cb4eeca1c946bffec71` | Zlib | https://github.com/dougbinks/enkiTS | `third_party/licenses/enkits-License.txt` |
| doctest | Test framework | v2.5.3 | `2d0a9359a60c51affe2a9bebb1be1dca47868151` | MIT | https://github.com/doctest/doctest | `third_party/licenses/doctest-LICENSE.txt` |

Approval: owner waived per-item approval for permissive-licensed, mature dependencies (session 2). Each entry was checked for license, recent activity, and a tagged release before use.
