#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inVertex;
layout(location = 1) in vec3 inNormal;

out VertexStage {
	vec3 normal;
} vs_out;

void main() {
	// Umer sending the vertices and normals directly from the mesh file over to the geometry shader
    gl_Position = vec4(inVertex, 1.0);
	vs_out.normal =  inNormal;
}