#include "vre/config.hpp"

#ifndef VRE_BUILD_EYE_MODE
#define VRE_BUILD_EYE_MODE "stereo"
#endif

namespace vre {

const char* to_string(EyeMode mode) {
    switch (mode) {
        case EyeMode::Stereo: return "stereo";
        case EyeMode::RightEyeToDesktop: return "right";
        case EyeMode::LeftEyeToDesktop: return "left";
        case EyeMode::Variable: return "variable";
        case EyeMode::NoVrRender: return "no_vr";
        case EyeMode::NoRender: return "no_render";
    }
    return "stereo";
}

bool parse_eye_mode(const std::string& text, EyeMode& out) {
    static const EyeMode all[] = {EyeMode::Stereo,           EyeMode::RightEyeToDesktop, EyeMode::LeftEyeToDesktop,
                                  EyeMode::Variable,         EyeMode::NoVrRender,        EyeMode::NoRender};
    for (EyeMode m : all) {
        if (text == to_string(m)) {
            out = m;
            return true;
        }
    }
    return false;
}

const char* to_string(Curvature curvature) {
    return curvature == Curvature::Vr ? "vr" : "flat";
}

Curvature default_curvature(EyeMode mode) {
    return mode == EyeMode::Stereo ? Curvature::Vr : Curvature::Flat;
}

EyeMode build_eye_mode() {
    EyeMode mode = EyeMode::Stereo;
    parse_eye_mode(VRE_BUILD_EYE_MODE, mode);
    return mode;
}

}  // namespace vre
