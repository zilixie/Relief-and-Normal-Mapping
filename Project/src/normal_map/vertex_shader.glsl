#version 150 core
#define PI 3.1415926538

in vec4 position;
in vec3 vertex_normal;
in vec2 textcoord;

out vec4 vs_pos;
out vec4 vs_obj_space_pos;
out vec3 vs_vn;
out vec2 vs_textcoord;

uniform mat4 model;					//the model matrix defined for the current vertex.
uniform mat4 view;					//the viewing matrix that change vertex from world coordinate system to camera coordinate system.
uniform mat4 camera;				//the matrix defined for camera transformation.
uniform mat4 proj;					//the matrix defined for projection (perspective or orthogonal).



void main()
{

	vs_pos = view * proj * camera * model * vec4(vec3(position[0], position[1], position[2]), 1.0);
	gl_Position = view * proj * camera * model * vec4(vec3(position[0], position[1], position[2]), 1.0);
	vs_obj_space_pos = position;
	vs_vn = vertex_normal;
	vs_textcoord = textcoord;

}