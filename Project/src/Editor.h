#ifndef EDITOR_H
#define EDITOR_H

#include "Helpers.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // GLFW is necessary to handle the OpenGL context
#else
#include <GLFW/glfw3.h> // GLFW is necessary to handle the OpenGL context
#endif

#include <Eigen/Core>
#include <Eigen/Dense>
#include <chrono>
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <initializer_list>
#include <math.h>
#include <cmath>
#include "read_off.h"
#include "normal.h"
#include <SOIL/SOIL.h>

#define TRANSLATION_MODE 1
#define DELETE_MODE 2
#define QUIT_MODE 7

using namespace std;
using namespace Eigen;

class Obj {
	public:
		int type;
		Eigen::Matrix4f model;
		Eigen::Matrix4f translation, rotation, scaling;
		int text_num;
		int norm_map_num;

	void init(int init_type, Eigen::Vector3f init_pos, float init_scale, int init_texture, int init_norm_map);
};

inline void Obj::init(int init_type, Eigen::Vector3f init_pos, float init_scale, int init_texture, int init_norm_map) {
	type = init_type;
	text_num = init_texture;
	norm_map_num = init_norm_map;

	model = MatrixXf::Identity(4, 4);
	translation = MatrixXf::Identity(4, 4);
	rotation = MatrixXf::Identity(4, 4);
	scaling = MatrixXf::Identity(4, 4);

	translation(0,3) = init_pos(0);
	translation(1,3) = init_pos(1);
	translation(2,3) = init_pos(2);

	Matrix4f s = MatrixXf::Identity(4, 4);
	s.topLeftCorner(3,3) << init_scale, 0, 0, 
							0, init_scale, 0,
							0, 0, init_scale;
	scaling = s;

	model = translation * rotation * scaling;
}

class Editor {
	public:
		int mode;										// Current mode
		int vertex_count;								// Numbers of vertices in V
		int normal_type;								// type of normal for shading
		bool obj_clicked;								// Whether there is aobject being clicked
		int ith_obj;									// which object is selected (for translate and animation)
		float width, height;							// width and height of screen
		int camera_type;								// perspective or orthogonal
		// for view control
		int pause;
		float alpha;
		float alpha_shift, beta_shift;
		Eigen::Matrix4f view;							// view transformation matrix
		Eigen::Matrix4f camera;							// camera transformation matrix
		Eigen::Matrix4f persp, orth;					// projection
		float radius;									// distance from camera to origin
		Vector3f cam_pos;								// current camera position
		Vector2f p0, p1; 								// previous and current cursor position
		std::vector<int> obj_index; 					// num of triangles for each type of obj
		std::vector<Obj> obj_vector;


		Eigen::MatrixXf V, N, VN;						// vertices, face normals, vertex normals

		Eigen::MatrixXf UV;
		Eigen::MatrixXi UF;
		Eigen::MatrixXf U;


		void init(void);
		void switch_mode(int m);
		void insert_mesh(std::string filename, float scale);
		void add_obj(int obj_id, Eigen::Vector3f init_pos, float init_scale, int init_texture, int init_norm_map);
		bool click_on_obj(Eigen::Vector2f world_coord_2d);
		void camera_move(int dir, int clock);

		void rotate_by(double degree, int clockwise, int direction);
		void scale_by(double percentage, int up);
		void delete_at(int del_ith_obj);
		void toggle_render_mode(int right);
		Eigen::Vector3f pixel_to_world_coord(Eigen::Vector2f pixel, int width, int height);
};

inline void Editor::switch_mode(int m){
	obj_clicked = false;
	ith_obj = -1;
	mode = m;
	V.conservativeResize(3, vertex_count);

	if (m == TRANSLATION_MODE) { std::cout << "Translation mode is on." << std::endl; }
	else if (m == DELETE_MODE) { std::cout << "Deletion mode is on." << std::endl; }
	else if (m == QUIT_MODE) { std::cout << "Quit Application." << std::endl; }
}

inline void Editor::init(void) {
	mode = TRANSLATION_MODE;
	normal_type = 3;
	pause = 1;
	radius = 9.0;
	cam_pos = Vector3f(9.0,0,0);

	obj_clicked = false;
	ith_obj = -1;
	vertex_count = 0;
	camera_type = 0;
	alpha_shift = 0;
	beta_shift = 0;

	p0 = Vector2f(0,0);
	p1 = Vector2f(0,0);
	V.resize(3,36);
	view = MatrixXf::Identity(4, 4);


	insert_mesh("../data/cube.off", 1.5);			//0
	insert_mesh("../data/cube.off", 1.5);			//1
	insert_mesh("../data/bunny.off", 10);			//2
	insert_mesh("../data/bumpy_cube.off", 0.2);		//3
	insert_mesh("../data/sphere.off", 1);			//4

	insert_mesh("../data/sphere.off", 0.00000);		//5
	//insert_mesh("../data/polyhedron.off", 1);
	//insert_mesh("../data/cube.off", 1);

	add_obj(5, Eigen::Vector3f (0.0, 0.0, 0.0),1, 0, 0);
	add_obj(5, Eigen::Vector3f (0.0, 0.0, 0.0),1, 0, 0);
	add_obj(5, Eigen::Vector3f (0.0, 0.0, 0.0),1, 0, 0);
	add_obj(5, Eigen::Vector3f (0.0, 0.0, 0.0),1, 0, 0);
	add_obj(5, Eigen::Vector3f (0.0, 0.0, 0.0),1, 0, 0);


	UV.resize(18,2);
	UF.resize(24,3);
	U.resize(72,2);

	UV.row(0) = Eigen::Vector2f (0.00,0.25);
	UV.row(1) = Eigen::Vector2f (0.00,0.50);
	UV.row(2) = Eigen::Vector2f (0.25,0.00);
	UV.row(3) = Eigen::Vector2f (0.25,0.25);
	UV.row(4) = Eigen::Vector2f (0.25,0.50);
	UV.row(5) = Eigen::Vector2f (0.25,0.75);
	UV.row(6) = Eigen::Vector2f (0.50,0.00);
	UV.row(7) = Eigen::Vector2f (0.50,0.25);
	UV.row(8) = Eigen::Vector2f (0.50,0.50);
	UV.row(9) = Eigen::Vector2f (0.50,0.75);
	UV.row(10) = Eigen::Vector2f (0.75,0.25);
	UV.row(11) = Eigen::Vector2f (0.75,0.50);
	UV.row(12) = Eigen::Vector2f (1.00,0.25);
	UV.row(13) = Eigen::Vector2f (1.00,0.50);

	UV.row(14) = Eigen::Vector2f (0.00,0.00);
	UV.row(15) = Eigen::Vector2f (0.00,1.00);
	UV.row(16) = Eigen::Vector2f (1.00,0.00);
	UV.row(17) = Eigen::Vector2f (1.00,1.00);

	UF.row(0) = Eigen::Vector3i (7,8,4);
	UF.row(1) = Eigen::Vector3i (4,3,7);
	UF.row(2) = Eigen::Vector3i (12,13,11);
	UF.row(3) = Eigen::Vector3i (11,10,12);
	UF.row(4) = Eigen::Vector3i (0,3,4);
	UF.row(5) = Eigen::Vector3i (4,1,0);
	UF.row(6) = Eigen::Vector3i (11,8,7);
	UF.row(7) = Eigen::Vector3i (7,10,11);
	UF.row(8) = Eigen::Vector3i (4,8,9);
	UF.row(9) = Eigen::Vector3i (9,5,4);
	UF.row(10) = Eigen::Vector3i (2,6,7);
	UF.row(11) = Eigen::Vector3i (7,3,2);

	UF.row(12) = Eigen::Vector3i (16,17,15);
	UF.row(13) = Eigen::Vector3i (15,14,16);
	UF.row(14) = Eigen::Vector3i (16,17,15);
	UF.row(15) = Eigen::Vector3i (15,14,16);
	UF.row(16) = Eigen::Vector3i (14,16,17);
	UF.row(17) = Eigen::Vector3i (17,15,14);
	UF.row(18) = Eigen::Vector3i (17,15,14);
	UF.row(19) = Eigen::Vector3i (14,16,17);
	UF.row(20) = Eigen::Vector3i (14,16,17);
	UF.row(21) = Eigen::Vector3i (17,15,14);
	UF.row(22) = Eigen::Vector3i (14,16,17);
	UF.row(23) = Eigen::Vector3i (17,15,14);


	for (int i=0; i<UF.rows(); i++) {
		for (int j=0; j<3; j++) {
			U.row(i*3 + j) = UV.row(UF(i,j));
		}
	}
}

inline void Editor::add_obj(int obj_id, Eigen::Vector3f init_pos, float init_scale, int init_texture, int init_norm_map) {
	if (mode != TRANSLATION_MODE) { switch_mode(TRANSLATION_MODE); }

	Obj new_obj;
	new_obj.init(obj_id, init_pos, init_scale, init_texture, init_norm_map);
	obj_vector.push_back(new_obj);

}

inline void Editor::insert_mesh(std::string filename, float scale) {
	MatrixXf mesh_V;
	MatrixXi mesh_F;
	read_off(filename,mesh_V,mesh_F);
	
	V.conservativeResize(3, vertex_count + mesh_F.rows() * 3);
	for (int i=0; i<mesh_F.rows(); i++) {
		for (int j=0; j<3; j++) {
			int r = mesh_F(i,j);
			V.col(vertex_count + i * 3 + j) << scale*mesh_V(r,0), scale*mesh_V(r,1), scale*mesh_V(r,2);
		}
	}
	obj_index.push_back(mesh_F.rows());

	int obj_count = (int)obj_index.size();
	int i = 0;
	for (int j = 0; j < obj_count-1; j++) {
		i += obj_index[j]; // count total triangles
	}
	float total_area = 0.0;
	Eigen::Vector3f mesh_bc (0.0,0.0,0.0);
	for (int k = 0; k < obj_index[obj_count-1]; k++) {
		Eigen::Vector3f v1(V(0,i*3+k*3), V(1,i*3+k*3), V(2,i*3+k*3));
		Eigen::Vector3f v2(V(0,i*3+k*3+1), V(1,i*3+k*3+1), V(2,i*3+k*3+1));
		Eigen::Vector3f v3(V(0,i*3+k*3+2), V(1,i*3+k*3+2), V(2,i*3+k*3+2));

		Vector3f bc ((v1(0)+v2(0)+v3(0))/3, (v1(1)+v2(1)+v3(1))/3, (v1(2)+v2(2)+v3(2))/3);
		float area = 0.5 * ((v2-v1).cross(v3-v1)).norm();
		mesh_bc += (area * bc);
		total_area += area;
	}
	mesh_bc = mesh_bc/total_area;
	for (int i = vertex_count; i < vertex_count + mesh_F.rows() * 3; i++) {
		V.col(i) << V(0,i) - mesh_bc(0), V(1,i) - mesh_bc(1), V(2,i) - mesh_bc(2); 
	}

	Eigen::MatrixXf new_fn;
	per_face_normals(mesh_V, mesh_F, new_fn);

	N.conservativeResize(vertex_count/3 + mesh_F.rows(), 3);
	for (int i=vertex_count/3; i < vertex_count/3 + mesh_F.rows(); i++){
		N.row(i) = new_fn.row(i - vertex_count/3);
	}

	Eigen::MatrixXf mesh_vn;
	per_vertex_normals(mesh_V, mesh_F, mesh_vn);

	Eigen::MatrixXf new_vn (mesh_F.rows()*3, 3);
	for (int i=0; i<mesh_F.rows(); i++) {
		for (int j=0; j<3; j++) {
			int r = mesh_F(i,j);
			new_vn.row(i * 3 + j) = mesh_vn.row(r);
		}
	}

	VN.conservativeResize(vertex_count + mesh_F.rows()*3, 3);
	for (int i=vertex_count; i < vertex_count + mesh_F.rows()*3; i++){
		VN.row(i) = new_vn.row(i - vertex_count);
	}
	vertex_count += (mesh_F.rows() * 3);
}

inline bool Editor::click_on_obj(Eigen::Vector2f cursor_pos) {
	for (int i = 0; i < obj_vector.size(); i ++) {
		int num_tri = obj_index[obj_vector[i].type];

		int start = 0;
		for (int j = 0; j < obj_vector[i].type; j++) { start += obj_index[j] * 3; }
		for (int j = start; j < start + num_tri * 3; j += 3) {
			Eigen::Vector4f v1(V(0,j), V(1,j), V(2,j), 1);
			Eigen::Vector4f v2(V(0,j+1), V(1,j+1), V(2,j+1), 1);
			Eigen::Vector4f v3(V(0,j+2), V(1,j+2), V(2,j+2), 1);
			if (camera_type == 0) {
				v1 = view * persp * camera * obj_vector[i].model * v1;
				v2 = view * persp * camera * obj_vector[i].model * v2;
				v3 = view * persp * camera * obj_vector[i].model * v3;
			} else {
				v1 = view * orth * camera * obj_vector[i].model * v1;
				v2 = view * orth * camera * obj_vector[i].model * v2;
				v3 = view * orth * camera * obj_vector[i].model * v3;
			}
			Vector2f screen_pos_v1 ((v1(0)/v1(3)+1)* width/2.0, (v1(1)/v1(3)+1)* height/2.0);
			Vector2f screen_pos_v2 ((v2(0)/v2(3)+1)* width/2.0, (v2(1)/v2(3)+1)* height/2.0);
			Vector2f screen_pos_v3 ((v3(0)/v3(3)+1)* width/2.0, (v3(1)/v3(3)+1)* height/2.0);

			MatrixXf m(3,2);
			m << (RowVector2f)(screen_pos_v1), (RowVector2f)(screen_pos_v2), (RowVector2f)(screen_pos_v3);
			m.transposeInPlace();

			Matrix3f A;
			Vector3f b;
			A << m, 1, 1, 1;
			b << cursor_pos(0), cursor_pos(1), 1;
			Vector3f x = A.colPivHouseholderQr().solve(b);
			if (x(0) > 0 && x(1) > 0 && x(2) > 0) {
				ith_obj = i;
				obj_clicked = true;
				return true;
			} else { obj_clicked = false; }
		}
	}
	return false;
}

inline Eigen::Vector3f Editor::pixel_to_world_coord(Eigen::Vector2f pixel, int width, int height) {
	Eigen::Vector4f canonical_coord((pixel(0)/width)*2-1, (pixel(1)/height)*2-1, 0, 1);
	Eigen::Vector4f shift_h;
	if (camera_type == 0) { shift_h = camera.inverse() * persp.inverse() * view.inverse() * canonical_coord; } 
	else { shift_h = camera.inverse() * orth.inverse() * view.inverse() * canonical_coord; }
	Eigen::Vector3f shift (shift_h(0)/shift_h(3), shift_h(1)/shift_h(3), shift_h(2)/shift_h(3));
	return shift;
}

inline void Editor::camera_move(int dir, int clock) {
	Vector3f shift(0,0,0);
	Vector3f new_cam_pos(0,0,0);
	if (dir == 1) { shift = Vector3f(0.3, 0, 0); }
	if (dir == 2) { shift = Vector3f(0, 0.3, 0); }
	if (dir == 3) { shift = Vector3f(0, 0, 0.3); }
	if (clock == 0) {
		new_cam_pos = cam_pos + shift;
	} else { new_cam_pos = cam_pos - shift; }

	radius = new_cam_pos.norm();
	float new_beta = asin(new_cam_pos(1)/radius);
	if (!isnan(new_beta)) {beta_shift = new_beta;}
	if (dir != 2) {
		float new_alpha = acos(new_cam_pos(0)/(radius * cos(new_beta)));
		if (new_cam_pos(2) < 0) { new_alpha = -new_alpha; }
		if (!isnan(new_alpha)) {alpha_shift = new_alpha;}
	}
}

inline void Editor::delete_at(int del_ith_obj) {
	obj_vector.erase(obj_vector.begin() + del_ith_obj);

}

inline void Editor::rotate_by(double degree, int clockwise, int direction) {
	if (mode == TRANSLATION_MODE && ith_obj != -1) {
		double theta;
		if (clockwise == 0) {theta = (-1) * degree * (M_PI / 180);} 
		else {theta = degree * (M_PI / 180);}

		Matrix4f r = MatrixXf::Identity(4, 4);
		if (direction == 0) { //z
			r.topLeftCorner(2,2) << cos(theta), -sin(theta), 
									sin(theta), cos(theta);
		} else if (direction == 1) { //x
			r.topLeftCorner(3,3) << 1,0,0,
									0, cos(theta), -sin(theta),
									0, sin(theta), cos(theta);
		} else if (direction == 2) { //y
			r.topLeftCorner(3,3) << cos(theta), 0,sin(theta),
									0, 1, 0,
									-sin(theta), 0, cos(theta);
		}
		obj_vector[ith_obj].rotation = r  * obj_vector[ith_obj].rotation;
	}
}

inline void Editor::scale_by(double percentage, int up) {
	if (mode == TRANSLATION_MODE && ith_obj != -1) {
		Matrix4f s = MatrixXf::Identity(4, 4);
		if (up) { s.topLeftCorner(3,3) << (1 - percentage), 0, 0, 
											0, (1 - percentage), 0,
											0, 0, (1 - percentage);
		} else { s.topLeftCorner(3,3) << (1 + percentage), 0, 0, 
											0, (1 + percentage), 0,
											0, 0, (1 + percentage);
		}
		obj_vector[ith_obj].scaling = s  * obj_vector[ith_obj].scaling;
	}
}

inline void Editor::toggle_render_mode(int right) {
	if (right) { normal_type ++; } 
	else { normal_type --; }
	if (normal_type == 4) { normal_type = 0; }
	if (normal_type == -1) { normal_type = 3; }
}

#endif

