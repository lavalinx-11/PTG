#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 1) in vec3 vertDir;
layout(location = 2) in vec3 Normal;
layout(location = 0) out vec4 fragColor;
layout(binding = 0) uniform samplerCube skybox;

void main() {

   float ratio = 1.00 / 1.42;
   vec3 reflecter = reflect(vertDir, Normal);


   vec3 refrac = refract(vertDir, normalize(Normal), ratio);
   float thetha = dot(-vertDir, normalize(Normal));
   vec3 reflectedColor = texture(skybox, reflecter).rgb;
   vec3 refractedColor = texture(skybox, refrac).rgb;
   vec3 finalColor = mix(reflectedColor,refractedColor, thetha);
   fragColor = vec4(finalColor, 1.0);
   
}