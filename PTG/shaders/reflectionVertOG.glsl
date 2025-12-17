#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec4 inVertex;
layout(location = 1) in vec3 inNormal;
 
layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 modelMatrix;
layout(location = 1) out vec3 vertDir;
layout(location = 2) out vec3 Normal;


void main() {

  
   
    vec3 vertDir = normalize(vec3(viewMatrix * modelMatrix * inVertex));
    mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
    Normal = normalize(normalMatrix * inNormal);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * inVertex;

  }