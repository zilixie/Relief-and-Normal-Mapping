#version 150 core
#define PI 3.1415926538
layout(triangles, equal_spacing, ccw) in;

in vec4 tcs_pos[];
in vec3 tcs_vn[];
in vec4 tcs_obj_space_pos[];

out vec4 tes_pos;
out vec3 tes_vn;
out vec4 tes_obj_space_pos;
out vec3 noise_color;


uniform mat4 model;					//the model matrix defined for the current vertex.
uniform mat4 view;					//the viewing matrix that change vertex from world coordinate system to camera coordinate system.
uniform mat4 camera;				//the matrix defined for camera transformation.
uniform mat4 proj;					//the matrix defined for projection (perspective or orthogonal).


vec3 interpolate(vec3 bary, vec3 v0, vec3 v1, vec3 v2) {
	return bary.x * v0 + bary.y * v1 + bary.z * v2;
}
vec4 interpolate(vec3 bary, vec4 v0, vec4 v1, vec4 v2) {
	return bary.x * v0 + bary.y * v1 + bary.z * v2;
}

/////////////////////////////////////////////////

vec2 random(vec3 seed){
	vec2 s = vec2(dot(seed,vec3(127.1,311.7,783.089)), dot(seed,vec3(269.5,183.3,173.542)));
	return fract(sin(s)*43758.5453123);
}

vec3 random_direction( vec3 seed) {
	vec2 uv = random(seed);
	float theta = uv.x * 2 * PI;
	float r = 0;
	if (uv.y > 0.5) {
		r = (uv.y - 1) * 2;
	}
	if (uv.y < 0.5) {
		r = (uv.y * 2);
	}
	if (uv.y == 0.5) {
		r = 0;
	}
	float x = sqrt(1 - r*r) * cos(theta);
	float y = sqrt(1 - r*r) * sin(theta);
	float z = r;

	return vec3(x,y,z);
}

float smooth_step( float f) {
	float y = 6 * pow(f, 5) - 15 * pow(f, 4) + 10 * pow(f, 3);
	return y;
}


float improved_perlin_noise( vec3 st) {
	vec3 a = vec3(floor(st.x), floor(st.y), floor(st.z));
	vec3 b = vec3(ceil(st.x), floor(st.y), floor(st.z));
	vec3 c = vec3(ceil(st.x), ceil(st.y), floor(st.z));
	vec3 d = vec3(floor(st.x), ceil(st.y), floor(st.z));
	vec3 e = vec3(floor(st.x), floor(st.y), ceil(st.z));
	vec3 f = vec3(ceil(st.x), floor(st.y), ceil(st.z));
	vec3 g = vec3(ceil(st.x), ceil(st.y), ceil(st.z));
	vec3 h = vec3(floor(st.x), ceil(st.y), ceil(st.z));

	vec3 a_dir = random_direction(a);
	vec3 b_dir = random_direction(b);
	vec3 c_dir = random_direction(c);
	vec3 d_dir = random_direction(d);
	vec3 e_dir = random_direction(e);
	vec3 f_dir = random_direction(f);
	vec3 g_dir = random_direction(g);
	vec3 h_dir = random_direction(h);

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

	float alpha_x = smooth_step(a_seed.x);
	float alpha_y = smooth_step(a_seed.y);
	float alpha_z = smooth_step(a_seed.z);

	float ab = mix(dot_a, dot_b, alpha_x);
	float dc = mix(dot_d, dot_c, alpha_x);
	float ef = mix(dot_e, dot_f, alpha_x);
	float hg = mix(dot_h, dot_g, alpha_x);

	float abcd = mix(ab, dc, alpha_y);
	float efgh = mix(ef, hg, alpha_y);

	float abcdefgh = mix(abcd, efgh, alpha_z);

	return abcdefgh - 0.5;
}

vec3 bump_position(vec3 s)
{
	float noise = 0.5 * improved_perlin_noise(s);
	vec3 d = normalize(s);
	vec3 adjust = mat3(noise,0,0,
					0,noise,0,
					0,0,noise) * d;
	return s + adjust;
}

//////////////////////////////////////////////


void main()
{
	vec4 tmp = interpolate(gl_TessCoord, tcs_pos[0], tcs_pos[1], tcs_pos[2]);
	tes_obj_space_pos = interpolate(gl_TessCoord, tcs_obj_space_pos[0], tcs_obj_space_pos[1], tcs_obj_space_pos[2]);
	tes_vn = interpolate(gl_TessCoord, tcs_vn[0], tcs_vn[1], tcs_vn[2]);


	vec3 noise_vec = vec3(0,0,0);



	noise_color = vec3(0.58,0.44,0.86);

	mat3 texture = mat3(15,0,0,
					0,15,0,
					0,0,15);
	vec3 texture_pos = texture * vec3(tes_obj_space_pos[0], tes_obj_space_pos[1], tes_obj_space_pos[2]);
	float height_noise = (improved_perlin_noise(texture_pos) + 1)/1.5;
	noise_vec = height_noise * tes_vn;



	vec4 adjusted_noise_vec = view * proj * camera * model * vec4(noise_vec, 1);
	vec3 adjusted_noise_vec_3d = 0.08 * adjusted_noise_vec.xyz;

	gl_Position = vec4(vec3(tmp[0], tmp[1], tmp[2]) + adjusted_noise_vec_3d, tmp[3]);
	tes_pos = tmp;
	
}