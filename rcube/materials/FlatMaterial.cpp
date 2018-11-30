#include "FlatMaterial.h"
#include <string>

const std::string vert_src = R"(
#version 420

layout (location = 0) in vec3 vertex;
layout (location = 3) in vec3 color;

layout (std140, binding=0) uniform Matrices {
    mat4 view_matrix;
    mat4 projection_matrix;
    mat4 viewport_matrix;
};

uniform mat4 modelview_matrix;

out vec3 frag_color;

void main() {
    vec4 cam_vertex = modelview_matrix * vec4(vertex, 1.0);
    gl_Position = projection_matrix * cam_vertex;
    frag_color = color;
}
)";

const std::string frag_src = R"(
#version 420

in vec3 frag_color;
out vec4 out_color;

void main() {
    out_color = vec4(frag_color, 1.0);
}
)";

FlatMaterial::FlatMaterial() {
}
std::string FlatMaterial::vertexShader() {
    return vert_src;
}
std::string FlatMaterial::fragmentShader() {
    return frag_src;
}
std::string FlatMaterial::geometryShader() {
    return "";
}
void FlatMaterial::setUniforms() {}

int FlatMaterial::renderPriority() const {
    return RenderPriority::Opaque;
}
