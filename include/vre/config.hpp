#pragma once
#include <string>

namespace vre {

// Eye modes: stereo to headset, one eye to desktop, variable (developer-defined),
// no VR render (flatscreen game), or no render (headless).
enum class EyeMode { Stereo, RightEyeToDesktop, LeftEyeToDesktop, Variable, NoVrRender, NoRender };
enum class Curvature { Flat, Vr };

const char* to_string(EyeMode mode);
bool parse_eye_mode(const std::string& text, EyeMode& out);
const char* to_string(Curvature curvature);

// Engine-managed: the VR curve is used only when stereo output goes to a headset.
Curvature default_curvature(EyeMode mode);

// Eye mode chosen at compile time by the VRE_EYE_MODE CMake option.
EyeMode build_eye_mode();

}  // namespace vre
