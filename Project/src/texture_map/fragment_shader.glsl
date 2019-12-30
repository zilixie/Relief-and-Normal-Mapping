#version 150 core
#define PI 3.1415926538

out vec4 outColor;
in vec4 vs_pos;
in vec3 vs_vn;
in vec4 vs_obj_space_pos;
in vec2 vs_textcoord;

uniform int click;
uniform mat4 model;
uniform bool draw_frame;			//the shader will draw frames when this variable is set true.
uniform int normal_type;			//the type of normal the application is using for rendering color (vertex normal or face normal).
uniform vec3 face_normal;			//face normal.
uniform vec3 light_d;				//light direction.

uniform sampler2D tex;


vec3 blinn_phong(vec3 ka, vec3 kd, vec3 ks, float p, vec3 n, vec3 v, vec3 l) {
	vec3 I = vec3(1,1,1);
	float factor = max(n.x * l.x + n.y * l.y + n.z * l.z, 0);
	vec3 diffuse = factor * I * kd;

	vec3 h = normalize(v + l);
	factor = pow(max((n.x * h.x + n.y * h.y + n.z * h.z), 0), p);
	vec3 specular = factor * I * ks;

	vec3 ambient = ka;
	return ambient + diffuse + specular;
}


void main() {
	vec3 color = vec3(0.0,0.0,0.0);
	vec4 texture_color = texture(tex, vs_textcoord);
	if (normal_type != 0) {
		color = texture_color.xyz;
	}

	vec3 kd = color;
	vec3 ka = kd/10.0;
	vec3 ks = vec3(0.1,0.1,0.1);
	float p = 100;


	vec3 fs_color = color;
	if (normal_type == 2) {
		fs_color = blinn_phong(ka, kd, ks, p, normalize(face_normal), light_d, light_d);
	} else if (normal_type == 3) {


		vec4 vertex_normal_4d = model * vec4(vs_vn[0], vs_vn[1], vs_vn[2], 0.0);
		vec3 vertex_normal_3d = vec3(vertex_normal_4d[0], vertex_normal_4d[1], vertex_normal_4d[2]);
		fs_color = blinn_phong(ka, kd, ks, p, normalize(vertex_normal_3d), light_d, light_d);
	}

	if (click == 1) {
		outColor = vec4(fs_color + vec3(0.05, 0.05, 0.05), 1.0);
	} else {
		outColor = vec4(fs_color, 1.0);
	}

	//outColor = texture(tex, vs_textcoord);

}
