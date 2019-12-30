#version 150 core
#define PI 3.1415926538

out vec4 outColor;
in vec4 tes_pos;
in vec3 tes_vn;
in vec3 noise_color;
in vec4 tes_obj_space_pos;


uniform int click;
uniform mat4 model;
uniform float time;					//a float value of the time since the start of the program
uniform bool draw_frame;			//the shader will draw frames when this variable is set true.
uniform int normal_type;			//the type of normal the application is using for rendering color (vertex normal or face normal).
uniform vec3 face_normal;			//face normal.
uniform vec3 light_d;				//light direction.


//////////////////////////////////////////////////////////////////

vec2 random(vec3 seed){
	vec2 s = vec2(dot(seed,vec3(127.1,311.7,783.089)), dot(seed,vec3(269.5,183.3,173.542)));
	return fract(sin(s)*43758.5453123);
}

vec3 get_direction(vec3 seed) {
	vec2 uv = random(seed);
	float theta = uv.x * 2 * PI;
	float r = 0;
	if (uv.y > 0.5) {
		r = (uv.y - 1) * 2;
	}
	if (uv.y < 0.5) {
		r = (uv.y * 2) - 1;
	}
	if (uv.y == 0.5) {
		r = 0;
	}
	float x = sqrt(1 - r*r) * cos(theta);
	float y = sqrt(1 - r*r) * sin(theta);
	float z = r;

	return vec3(x,y,z);
}

float improved_smooth_step(float f) {
	float y = 6 * pow(f, 5) - 15 * pow(f, 4) + 10 * pow(f, 3);
	return y;
}


float improved_perlin_noise( vec3 st)  {
	vec3 a = vec3(floor(st.x), floor(st.y), floor(st.z));
	vec3 b = vec3(ceil(st.x), floor(st.y), floor(st.z));
	vec3 c = vec3(ceil(st.x), ceil(st.y), floor(st.z));
	vec3 d = vec3(floor(st.x), ceil(st.y), floor(st.z));
	vec3 e = vec3(floor(st.x), floor(st.y), ceil(st.z));
	vec3 f = vec3(ceil(st.x), floor(st.y), ceil(st.z));
	vec3 g = vec3(ceil(st.x), ceil(st.y), ceil(st.z));
	vec3 h = vec3(floor(st.x), ceil(st.y), ceil(st.z));

	vec3 a_dir = get_direction(a);
	vec3 b_dir = get_direction(b);
	vec3 c_dir = get_direction(c);
	vec3 d_dir = get_direction(d);
	vec3 e_dir = get_direction(e);
	vec3 f_dir = get_direction(f);
	vec3 g_dir = get_direction(g);
	vec3 h_dir = get_direction(h);

	vec3 a_seed = st - a;
	vec3 b_seed = st - b;
	vec3 c_seed = st - c;
	vec3 d_seed = st - d;
	vec3 e_seed = st - e;
	vec3 f_seed = st - f;
	vec3 g_seed = st - g;
	vec3 h_seed = st - h;

	float dot_a = a_dir.x * a_seed.x + a_dir.y * a_seed.y + a_dir.z * a_seed.z;
	float dot_b = b_dir.x * b_seed.x + b_dir.y * b_seed.y + b_dir.z * b_seed.z;
	float dot_c = c_dir.x * c_seed.x + c_dir.y * c_seed.y + c_dir.z * c_seed.z;
	float dot_d = d_dir.x * d_seed.x + d_dir.y * d_seed.y + d_dir.z * d_seed.z;
	float dot_e = e_dir.x * e_seed.x + e_dir.y * e_seed.y + e_dir.z * e_seed.z;
	float dot_f = f_dir.x * f_seed.x + f_dir.y * f_seed.y + f_dir.z * f_seed.z;
	float dot_g = g_dir.x * g_seed.x + g_dir.y * g_seed.y + g_dir.z * g_seed.z;
	float dot_h = h_dir.x * h_seed.x + h_dir.y * h_seed.y + h_dir.z * h_seed.z;

	float alpha_x = improved_smooth_step(a_seed.x);
	float alpha_y = improved_smooth_step(a_seed.y);
	float alpha_z = improved_smooth_step(a_seed.z);

	float ab = mix(dot_a, dot_b, alpha_x);
	float dc = mix(dot_d, dot_c, alpha_x);
	float ef = mix(dot_e, dot_f, alpha_x);
	float hg = mix(dot_h, dot_g, alpha_x);

	float abcd = mix(ab, dc, alpha_y);
	float efgh = mix(ef, hg, alpha_y);

	float abcdefgh = mix(abcd, efgh, alpha_z);

	return abcdefgh - 0.5;
}

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

vec3 bump_position(vec3 s) {
	float noise = 0.5 * improved_perlin_noise(s);
	vec3 d = normalize(s);
	vec3 adjust = mat3(noise,0,0,
					0,noise,0,
					0,0,noise) * d;
	return s + adjust;
}

void tangent(in vec3 N, out vec3 T, out vec3 B) {
	float t_z = (-1) * (N.x + N.y)/N.z;
	T = vec3(1,1,t_z);
	B = cross(N, T);
}


///////////////////////////////////////////////////////////////////

void main() {
	vec3 color = vec3(0.0,0.0,0.0);
	if (!draw_frame) {
		color = noise_color;
	}

	vec3 kd = color;
	vec3 ka = kd/10.0;
	vec3 ks = vec3(0.1,0.1,0.1);
	float p = 100;

	vec3 fs_color = vec3(0,0,0);

	if (normal_type == 2) {
		fs_color = blinn_phong(ka, kd, ks, p, normalize(face_normal), light_d, light_d);
	} else if (normal_type == 3) {
		if (noise_color == vec3(0.2,0.7,0.0) || noise_color == vec3(1.0,1.0,1.0)) {
			mat3 bump = mat3(5,0,0,
							0,5,0,
							0,0,5);

			vec3 bi_norm = vec3(0,0,0);
			vec3 tang = vec3(0,0,0);
			tangent(tes_vn, tang, bi_norm);

			float eps = 0.0001;
			vec3 new_n = cross(( bump_position(bump * vec3(tes_obj_space_pos[0], tes_obj_space_pos[1], tes_obj_space_pos[2]) + eps*tang)
								- bump_position(bump * vec3(tes_obj_space_pos[0], tes_obj_space_pos[1], tes_obj_space_pos[2])) )/eps,
								 (bump_position(bump * vec3(tes_obj_space_pos[0], tes_obj_space_pos[1], tes_obj_space_pos[2]) + eps*bi_norm)
								- bump_position(bump * vec3(tes_obj_space_pos[0], tes_obj_space_pos[1], tes_obj_space_pos[2])))/eps);

			new_n = normalize(new_n);
			vec4 vertex_normal_4d = model * vec4(new_n[0], new_n[1], new_n[2], 0.0);
			vec3 vertex_normal_3d = vec3(vertex_normal_4d[0], vertex_normal_4d[1], vertex_normal_4d[2]);
			fs_color = blinn_phong(ka, kd, ks, p, normalize(vertex_normal_3d), light_d, light_d);
		}
		else {
			ks = vec3(0.3,0.3,0.3);
			mat3 bump = mat3(15,0,0,
							0,15,0,
							0,0,15);

			vec3 bi_norm = vec3(0,0,0);
			vec3 tang = vec3(0,0,0);
			tangent(tes_vn, tang, bi_norm);

			float eps = 0.0001;
			vec3 new_n = cross((bump_position(bump * vec3(tes_obj_space_pos[0], tes_obj_space_pos[1], tes_obj_space_pos[2]) + eps*tang)
							- bump_position(bump * vec3(tes_obj_space_pos[0], tes_obj_space_pos[1], tes_obj_space_pos[2])))/eps,
							 (bump_position(bump * vec3(tes_obj_space_pos[0], tes_obj_space_pos[1], tes_obj_space_pos[2]) + eps*bi_norm)
							- bump_position(bump * vec3(tes_obj_space_pos[0], tes_obj_space_pos[1], tes_obj_space_pos[2])))/eps);

			new_n = normalize(new_n);
			vec4 vertex_normal_4d = model * vec4(new_n[0], new_n[1], new_n[2], 0.0);
			vec3 vertex_normal_3d = vec3(vertex_normal_4d[0], vertex_normal_4d[1], vertex_normal_4d[2]);
			fs_color = blinn_phong(ka, kd, ks, p, normalize(vertex_normal_3d), light_d, light_d);
		}

	} else {
		fs_color = color;
	}

	if (click == 1) {
		outColor = vec4(fs_color + vec3(0.05, 0.05, 0.05), 1.0);
	} else {
		outColor = vec4(fs_color, 1.0);
	}

	//outColor = vec4(color, 1.0);
}
