#include "RCube/Core/Graphics/Effects/GammaCorrectionEffect.h"
namespace rcube
{

const static std::string GammaCorrectionEffectFragmentShader = R"(
#version 420
in vec2 v_texcoord;
out vec4 out_color;
layout (binding=0) uniform sampler2D fbo_texture;
void main() {
    vec4 tex = texture(fbo_texture, v_texcoord);
    vec3 gamma = vec3(1.0/2.2);
    vec3 final_color = pow(tex.rgb, gamma);
    out_color = vec4(final_color, 1);
}
)";

std::shared_ptr<ShaderProgram> makeGammaCorrectionEffect()
{
    return makeEffect(GammaCorrectionEffectFragmentShader);
}

} // namespace rcube
