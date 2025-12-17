#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec3 vertNormal;
//layout(location = 1) in vec3 lightDir;
layout(location = 4) in vec3 lightDir[5];
layout(location = 2) in vec3 eyeDir; 
layout(location = 3) in vec2 textureCoords; 
uniform vec4 Specular[5]; 
uniform vec4 Diffuse[5]; 
uniform sampler2D myTexture; 



void main() {
vec4 ka[5];
for(int i = 0; i < 5; i++) {
	 ka[i] = 0.1 * Diffuse[i];
	}
	vec4 kt = texture(myTexture,textureCoords); 
	vec4 colour = vec4(0.0f);
	float diff;
	
	for(int i = 0; i < 5; i++){
	diff = max(dot(vertNormal, lightDir[i]), 0.0);

	/// Reflection is based incedent which means a vector from the light source
	/// not the direction to the light source so flip the sign
	vec3 reflection = normalize(reflect(-lightDir[i], vertNormal));
	
	float spec = max(dot(eyeDir, reflection), 0.0);
	spec = pow(spec,.0);

//	for(int h = 0; h < 5; h++) {
//	colour += (diff * Diffuse[h]) + (spec * Specular[h]);
//	}

	colour += (diff * Diffuse[i]) + (spec * Specular[i]);
	}
	fragColor =  (ka[0] + colour) * kt;	
}
