# Open questions

## Engine
- Curvature selection: build-time default with a runtime override is the owner's answer. Define the exact runtime trigger (headset connected or session start).
- Curved-layer radius per device: fixed presets only, or tuned per headset?
- Cross-eye reuse: exact reuse rule and error bound.
- Which HUD elements are per-eye, using the depth-attribute rule or hand-assignment?
- Warning mechanism for manual curvature control: how the API is marked so every use warns.
- Headset rendering path: which graphics API first (Vulkan or OpenGL ES), and when does VR Snake render to a headset?

## Companion
- ALVR: is there a newer tag than v20.14.1 (July 2025)? Confirm before first integration.
- Quest store and launch requirements: whether the VR category alone is enough for the Horizon OS library, or LAUNCHER is also needed. Verify in Meta's docs.
- Android XR: the exact store filter and launch category. Verify in Google's docs.
- Which ALVR components are needed for our curved-layer split, and which patches must be kept separate?

## Repo and process
- Whether the repos stay public (both are public now, MIT).
- The signing repo README still describes the identity as camera-app only. The owner has said it is shared for new Android apps; the README should be updated to say so.
