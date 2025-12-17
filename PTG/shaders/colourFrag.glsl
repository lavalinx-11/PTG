#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;
layout(location = 3) in vec2 texture_coordinates;



uniform sampler2D myTexture;
uniform vec4 color_for_frag;


void main() {
    // vec4 texture_constant = texture(myTexture, texture_coordinates);
    fragColor = vec4(color_for_frag);
}