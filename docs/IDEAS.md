# Ideas (not committed)

## Rendering
- Cross-eye reuse: shade the shared pass once, reuse results where the two eyes' samples match, and recompute only at disocclusions and specular highlights. Needs a clear reuse rule.
- Skip the per-eye stage only when the correction is provably a no-op within a measured error bound. "The head barely moved" is not enough.
- Static HUD caching: render rarely-changing HUD elements once, keyed by their state, size, and layer placement.
- Measured PC/headset split with hysteresis, decided at runtime from timings over recent frames.
- Per-eye dynamic resolution: a slower eye drops resolution without the other one matching it.
- Schedule non-urgent engine work (streaming, shader compilation) into the gap after frame submission.

## Lighting
- Light probes or irradiance volumes for moving objects.
- Lightmaps for static geometry.
- Light sets: precompute a few lighting configurations and blend at runtime (UberBake-style).
- Static virtual point lights with runtime intensity scaling.

## Streaming
- Split rendering: heavy shared pass on the PC, per-eye stage on the headset. Choose the split point at runtime from measured transfer cost, compute cost, and staleness tolerance.
- Video-style compression for the shared-layer transfer. Check the added latency.

## Tooling
- A studio editor later. Start with the CLI runner and packager.

## Companion
- GLES fallback in the device info when Vulkan reports no GPU.
- Export or share the device info JSON.
