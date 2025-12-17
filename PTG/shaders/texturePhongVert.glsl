    #version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 uvCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
//uniform vec3 lightPos;

//layout(std140) uniform litPosBlock {
  uniform vec3 Litpos[5];   // Array of 5 light positions
//};


layout(location = 0) out vec3 vertNormal;

layout(location = 2) out vec3 eyeDir;
layout(location = 3) out vec2 textureCoords; 
layout(location = 4) out vec3 lightDir[5];


void main() {
    textureCoords = uvCoord;
    mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
    vertNormal = normalize(normalMatrix * vNormal); /// Rotate the normal to the correct orientation 
    vec3 vertPos = vec3(viewMatrix * modelMatrix * vVertex);
    vec3 vertDir = normalize(vertPos);
    eyeDir = -vertDir;
    
    for (int i = 0; i < 5; i++) {
    lightDir[i] = normalize(vec3(Litpos[i]) - vertPos); 
    }


    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vVertex;
    
}
