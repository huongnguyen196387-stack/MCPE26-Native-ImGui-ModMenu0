# MCPE26 ImGui Native Mod Menu — Fresh Build

Clean Android NDK + Dear ImGui host/test project.
- Android SDK 35
- arm64-v8a
- C++20
- NDK 26.3
- CMake 3.22.1
- Dear ImGui Android + OpenGL ES 3
- Touch input
- GitHub Actions build

This is a native UI/test host. It does not include Minecraft memory patching,
hard-coded offsets, anti-cheat bypasses, process injection, or packet manipulation.

## Performance panel

The menu now contains a Performance section with:
- Low graphics mode toggle (UI setting)
- Performance statistics toggle
- Frame limiter toggle (UI setting)
- Target FPS selector: 30 / 45 / 60 / 90 / 120
- Current FPS and renderer information

These are safe UI/test controls; they do not modify Minecraft internals.

- Render Culling toggle (UI setting)

## Actual Render Culling implementation

For the supplied Minecraft 26.0 ARM64 binary, the native module targets the
GLES draw imports `glDrawElements` and `glDrawArrays` from `libminecraftpe.so`.

When this native library is loaded into the Minecraft process, the Performance
-> Render Culling toggle changes OpenGL ES back-face culling for triangle draws
originating from `libminecraftpe.so`. The hook is limited to the Minecraft
library and restores the previous GL cull state after each draw.

This is real GPU primitive culling (back-face culling), not a fake UI toggle and
not a frustum/occlusion system. A loader is required to load the `.so` into the
Minecraft process; the companion test APK alone cannot modify a separate
Minecraft process.

### Verification notes

The supplied 26.0 ARM64 `libminecraftpe.so` imports `glDrawElements` and
`glDrawArrays` from `libGLESv2.so`. The module hooks those actual GLES entry
points and filters calls whose return address belongs to `libminecraftpe.so`.
No fixed Minecraft function offset is required for this part.

The effect is real OpenGL ES back-face culling on triangle primitives. It is
not frustum/occlusion culling and therefore should not be described as a full
visibility system.
