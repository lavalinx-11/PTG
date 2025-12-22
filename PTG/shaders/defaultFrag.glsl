#version 450
#extension GL_ARB_separate_shader_objects : enable
uniform vec3 debugColor;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = vec4(debugColor, 1.0);
}