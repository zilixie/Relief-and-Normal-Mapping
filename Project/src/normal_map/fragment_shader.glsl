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
uniform sampler2D normalMap;
uniform vec3 tangt;


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



void tangent(in vec3 N, out vec3 T, out vec3 B) {
	float t_z = (-1) * (N.x + N.y)/N.z;
	T = vec3(1,1,t_z);
	B = cross(N, T);
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
		//kd = vec3(0, 0, 0);



		vec3 bi_norm = vec3(0,0,0);
		vec3 tang = vec3(0,0,0);
		//tangent(vs_vn, tang, bi_norm);
		//tang = normalize(tang);
		//bi_norm = normalize(bi_norm);


		tang = normalize(tangt);
		bi_norm = cross(vs_vn, tangt);






		mat3 toTangentSpc = mat3(
						tang.x, bi_norm.x, vs_vn.x,
						tang.y, bi_norm.y, vs_vn.y,
						tang.z, bi_norm.z, vs_vn.z);



		vec3 new_n = texture(normalMap, vs_textcoord).xyz;
		new_n = normalize(new_n * 2.0 - 1.0);// * 1.01;
		//kd = new_n;



		//vec4 vertex_normal_4d = model * vec4(vs_vn[0], vs_vn[1], vs_vn[2], 0.0);
		//vec3 vertex_normal_3d = vec3(vertex_normal_4d[0], vertex_normal_4d[1], vertex_normal_4d[2]);
		//fs_color = blinn_phong(ka, kd, ks, p, vertex_normal_3d, light_d, light_d);
		

		fs_color = blinn_phong(ka, kd, ks, p, new_n, toTangentSpc * light_d, toTangentSpc * light_d);
	}

	if (click == 1) {
		outColor = vec4(fs_color + vec3(0.05, 0.05, 0.05), 1.0);
	} else {
		outColor = vec4(fs_color, 1.0);
	}

	//outColor = texture(tex, vs_textcoord);

}
