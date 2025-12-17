#version 450
#extension GL_ARB_separate_shader_objects : enable


uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

//layout (binding = 0) uniform sampler2D heightTexture;
layout (binding = 1) uniform sampler2D normalMap;
layout (binding = 2) uniform sampler2D diffuseMap;

in vec2 uvCoordFromEval;
in vec3 normalFromEval;
in vec3 eyeDirFromEval;
in vec3 lightDirFromEval;
in float fogValFromEval;
out vec4 fragColor;


uniform sampler2D textureData;
void main() {

    vec4 deepColor = vec4(0.01,0.2,0.4,0.0);
    vec4 kd = vec4(0.5,0.45,0.4,0.0);
    vec4 ks = 0.7 * kd;
    vec4 ka = 0.01 * kd;
    vec4 kt = texture(diffuseMap,uvCoordFromEval);
    vec3 normalFromMap = texture(normalMap,uvCoordFromEval).xyz * 2.0 - 1.0;
    float diff = max(dot(lightDirFromEval, normalFromMap), 0.0);
    vec3 reflection = normalize(reflect(-lightDirFromEval, normalFromMap));
    float spec = max(dot(eyeDirFromEval, reflection), 0.0);
    spec = pow(spec,14.0);
    vec4 outputColor = (ka + (kd * diff) + (ks * spec)) * kt;
    
    
    fragColor = mix(deepColor, outputColor, fogValFromEval);
  
}
