#version 150 core
layout (vertices = 3) out;


in vec4 vs_pos[];
in vec4 vs_obj_space_pos[];
in vec3 vs_vn[]; 

out vec4 tcs_pos[];
out vec4 tcs_obj_space_pos[];
out vec3 tcs_vn[];

void main()
{
	tcs_pos[gl_InvocationID] = vs_pos[gl_InvocationID];
	tcs_vn[gl_InvocationID] = vs_vn[gl_InvocationID];
	tcs_obj_space_pos[gl_InvocationID] = vs_obj_space_pos[gl_InvocationID];

	if (gl_InvocationID == 0) {
		gl_TessLevelInner[0] = 10;
		gl_TessLevelInner[1] = 10;
		gl_TessLevelOuter[0] = 10;
		gl_TessLevelOuter[1] = 10;
		gl_TessLevelOuter[2] = 10;
		gl_TessLevelOuter[3] = 10;

	}
}