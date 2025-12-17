#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(vertices = 3) out;
uniform float tessalationLev;
uniform float tessLevelInner;
uniform float tessLevelOuter;

in vec2 uvCoordFromVert[];
in vec3 normalFromVert[];

out vec2 uvCoordFromCtrl[];
out vec3 normalFromCtrl[];


void main() {
    /// Pass through input vertices to tessellation evaluation stage
    gl_out[gl_InvocationID].gl_Position =  gl_in[gl_InvocationID].gl_Position;
    uvCoordFromCtrl[gl_InvocationID] = uvCoordFromVert[gl_InvocationID];
    normalFromCtrl[gl_InvocationID] = normalFromVert[gl_InvocationID];

   float tesslevel = 5;
   tesslevel = tessalationLev;
    /// Set tessellation levels
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = tesslevel;
        gl_TessLevelOuter[0] = tesslevel;
        gl_TessLevelOuter[1] = tesslevel;
        gl_TessLevelOuter[2] = tesslevel;

    }
}
