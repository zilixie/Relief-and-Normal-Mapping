// This example is heavily based on the tutorial at https://open.gl
// OpenGL Helpers to reduce the clutter
#include "Helpers.h"
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

#include "Editor.h"
#include "read_json.h"
// Global Variables
VertexBufferObject VBO; // VertexBufferObject wrapper
VertexBufferObject VBO_n; // VertexBufferObject wrapper
VertexBufferObject VBO_t; // VertexBufferObject wrapper
Editor e;

// Callback Functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) { e.radius -= 0.2 * yoffset; }

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos) {
	// Get the size of the window.
	int width, height;
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwGetWindowSize(window, &width, &height);
	Eigen::Vector2f pixel(xpos, height-1-ypos); // pixel position
	// Track the mouse positions
	if (e.p0 != Vector2f(0,0) && e.p1 != Vector2f(0,0)) {
		e.p0 = e.p1;
		e.p1 = pixel;
	} else { e.p0 = e.p1 = pixel; }

	if (e.mode == TRANSLATION_MODE && e.ith_obj != -1 && e.obj_clicked) {
		Eigen::Vector3f world_p0 = e.pixel_to_world_coord(e.p0, width, height);
		Eigen::Vector3f world_p1 = e.pixel_to_world_coord(e.p1, width, height);
		if (e.camera_type == 0) {
			e.obj_vector[e.ith_obj].translation(0,3) += (world_p1(0) - world_p0(0)) * e.radius/9;
			e.obj_vector[e.ith_obj].translation(1,3) += (world_p1(1) - world_p0(1)) * e.radius/9;
			e.obj_vector[e.ith_obj].translation(2,3) += (world_p1(2) - world_p0(2)) * e.radius/9;
		} else {
			e.obj_vector[e.ith_obj].translation(0,3) += (world_p1(0) - world_p0(0));
			e.obj_vector[e.ith_obj].translation(1,3) += (world_p1(1) - world_p0(1));
			e.obj_vector[e.ith_obj].translation(2,3) += (world_p1(2) - world_p0(2));
		}
	}
}

void mouse_click_callback(GLFWwindow* window, int button, int action, int mods) {
	if (e.mode == TRANSLATION_MODE) {	
		if (action == GLFW_RELEASE) { e.obj_clicked = false; } 
		else if (!e.click_on_obj(e.p1)) { e.ith_obj = -1; }
	} else if (e.mode == DELETE_MODE) {
		if (e.click_on_obj(e.p1) && action == GLFW_PRESS) { // down edge: delete triangle
			e.delete_at(e.ith_obj);
			e.ith_obj = -1;
			e.obj_clicked = false;
		}
		if (action == GLFW_RELEASE) { e.ith_obj = -1; }
	} 
	VBO.update(e.V); // Upload the change to the GPU
	VBO_n.update(e.VN.transpose());
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_O && action == GLFW_RELEASE) { e.switch_mode(TRANSLATION_MODE); }
	if (key == GLFW_KEY_P && action == GLFW_RELEASE) { e.switch_mode(DELETE_MODE); }
	if (key == GLFW_KEY_Q && action == GLFW_RELEASE) { e.switch_mode(QUIT_MODE); }
	if (key == 49 && action == GLFW_RELEASE) { e.add_obj(1, Eigen::Vector3f(0.0, 0.0, 0.0),1, 0, 0); }
	if (key == 50 && action == GLFW_RELEASE) { e.add_obj(2, Eigen::Vector3f(0.0, 0.0, 0.0),1, 0, 0); }
	if (key == 51 && action == GLFW_RELEASE) { e.add_obj(3, Eigen::Vector3f(0.0, 0.0, 0.0),1, 0, 0); }
	if (key == 52 && action == GLFW_RELEASE) { e.add_obj(4, Eigen::Vector3f(0.0, 0.0, 0.0),1, 0, 0); }

	if (key == GLFW_KEY_LEFT_BRACKET  && action == GLFW_RELEASE) { e.toggle_render_mode(1); }
	if (key == GLFW_KEY_RIGHT_BRACKET  && action == GLFW_RELEASE) { e.toggle_render_mode(0); }
	if (key == GLFW_KEY_SLASH && action == GLFW_RELEASE) { e.camera_type = abs(e.camera_type-1); }
	if (key == GLFW_KEY_G) { e.rotate_by(2,1,0); }
	if (key == GLFW_KEY_H) { e.rotate_by(2,1,1); }
	if (key == GLFW_KEY_J) { e.rotate_by(2,1,2); }
	if (key == GLFW_KEY_B) { e.rotate_by(2,0,0); }
	if (key == GLFW_KEY_N) { e.rotate_by(2,0,1); }
	if (key == GLFW_KEY_M) { e.rotate_by(2,0,2); }
	if (key == GLFW_KEY_K) { e.scale_by(0.05,1); }
	if (key == GLFW_KEY_L) { e.scale_by(0.05,0); }
	if (key == GLFW_KEY_LEFT) { e.alpha_shift -= M_PI/90.0; }
	if (key == GLFW_KEY_RIGHT) { e.alpha_shift += M_PI/90.0; }
	if (key == GLFW_KEY_UP) { e.beta_shift -= M_PI/90.0; }
	if (key == GLFW_KEY_DOWN) { e.beta_shift += M_PI/90.0; }
	if (key == GLFW_KEY_MINUS) {e.view.topLeftCorner(2,2) = e.view.topLeftCorner(2,2) * 0.97;}
	if (key == GLFW_KEY_EQUAL) {e.view.topLeftCorner(2,2) = e.view.topLeftCorner(2,2) * 1.03;}
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) { e.pause = abs(e.pause-1); if (e.pause == 1) {e.alpha_shift = e.alpha;} }
	if (key == GLFW_KEY_W) { e.camera_move(2, 0); }
	if (key == GLFW_KEY_S) { e.camera_move(2, 1); }
	if (key == GLFW_KEY_E) { e.camera_move(1, 0); }
	if (key == GLFW_KEY_D) { e.camera_move(1, 1); }
	if (key == GLFW_KEY_R) { e.camera_move(3, 0); }
	if (key == GLFW_KEY_F) { e.camera_move(3, 1); }
}

// Main
int main(int argc, char* argv[]) {
	GLFWwindow* window;
	if (!glfwInit()) { return -1; }     // Initialize the library
	glfwWindowHint(GLFW_SAMPLES, 8);    // Activate supersampling
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);    // Ensure that we get at least a 3.2 context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	// On apple we have to load a core profile with forward compatibility
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
	#endif
	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, "Project", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // Make the window's context current

	#ifndef __APPLE__
		glewExperimental = true;
		GLenum err = glewInit();
		if(GLEW_OK != err) {
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
		glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
		fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	#endif

	int major, minor, rev;
	major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
	minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
	printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
	printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
	printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	e.init();
	VertexArrayObject VAO;		// Initialize the VAO. A Vertex Array Object (or VAO) is an object that describes
	VAO.init();					// how the vertex attributes are stored in a Vertex Buffer Object (or VBO).
	VAO.bind();					// This means that the VAO is not the actual object storing the vertex data,
								// but the descriptor of the vertex data.
	VBO.init();           // Initialize the VBO with the vertices data
	VBO.update(e.V);      // A VBO is a data container that lives in the GPU memory
	VBO_n.init();
	VBO_n.update(e.VN.transpose());
	VBO_t.init();
	VBO_t.update(e.U.transpose());
	Program program;   	// Initialize the OpenGL Program. A program controls the OpenGL pipeline and it must contains
						// at least a vertex shader and a fragment shader to be valid

	std::vector<std::string> obj_type_vector;
	std::vector<Eigen::Vector3f> pos_vector;
	std::vector<float> scale_vector;
	std::vector<int> text_vector, norm_vector, depth_vector;
	std::vector<std::string> text_paths;


	std::string vs_filename, tcs_filename, tes_filename, fs_filename;
	if (std::string(argv[1]) == "../src/bump_map/data.json") {
		read_json(argv[1], vs_filename, tcs_filename, tes_filename, fs_filename, 
		obj_type_vector, pos_vector, text_vector, text_paths, norm_vector, depth_vector, scale_vector);
	} else {
		read_json(argv[1], vs_filename, tcs_filename, tes_filename, fs_filename, 
		obj_type_vector, pos_vector, text_vector, text_paths, norm_vector, scale_vector);}
	for (int i=0; i<obj_type_vector.size(); i++) {
		if (obj_type_vector[i] == "cube1") {e.add_obj(0, pos_vector[i], scale_vector[i], text_vector[i], norm_vector[i]);}
		if (obj_type_vector[i] == "cube2") {e.add_obj(1, pos_vector[i], scale_vector[i], text_vector[i], norm_vector[i]);}
		if (obj_type_vector[i] == "bunny") {e.add_obj(2, pos_vector[i],scale_vector[i], text_vector[i], norm_vector[i]);}
		if (obj_type_vector[i] == "bumpy_cube") {e.add_obj(3, pos_vector[i],scale_vector[i], text_vector[i], norm_vector[i]);}
		if (obj_type_vector[i] == "sphere") {e.add_obj(4, pos_vector[i],scale_vector[i], text_vector[i], norm_vector[i]);}
	}

	if (tcs_filename != "" || tes_filename != "" ) {
		program.init(vs_filename, fs_filename, tcs_filename, tes_filename, "outColor");
	} else { program.init(vs_filename, fs_filename, "outColor"); }

	program.bind();          // Note that we have to explicitly specify that the output "slot" called outColor
	                         // is the one that we want in the fragment buffer (and thus on screen)
	glfwSetKeyCallback(window, key_callback);                 // Register the keyboard callback
	glfwSetMouseButtonCallback(window, mouse_click_callback); // Register the mouse callback
	glfwSetCursorPosCallback(window, mouse_move_callback);    // Register the cursor move callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Update viewport
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glEnable(GL_DEPTH_TEST);
	glPointSize(10.0f);

	float n = 4.0, f = 100.0, l = -1.0, r = 1.0, t = 1.0, b = -1.0;
	e.persp << 2*n/(r-l), 0,           (r+l)/(r-l),      0,
			   0,         2*n/(t-b),   (t+b)/(t-b),      0,
			   0,         0,           (-1)*(f+n)/(f-n), (-1)*(2*f*n)/(f-n),
			   0,         0,           -1,               0;
	e.orth << 2/(r-l),   0,       0,        -(r+l)/(r-l),
			  0,         2/(t-b), 0,        -(t+b)/(t-b),
			  0,         0,       -2/(f-n), -(f+n)/(f-n),
			  0,         0,       0,        1; 

	TimePoint t_start = std::chrono::high_resolution_clock::now();
	TimePoint t_now = std::chrono::high_resolution_clock::now();
	TimePoint new_t_start = TimePoint();

	float time = 0.0;
	float glob_time = 0.0;
	float alpha = 0.0, beta = 0.0;

	program.bindVertexAttribArray("position",VBO);
	program.bindVertexAttribArray("vertex_normal",VBO_n);
	program.bindVertexAttribArray("textcoord",VBO_t);

	//VAO.bind();    // Bind your VAO (not necessary if you have only one)

	for (int i=0; i<text_paths.size(); i++) {
		GLuint textureId1;
		glGenTextures(1, &textureId1);
		glBindTexture(GL_TEXTURE_2D, textureId1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		unsigned char*imageData = NULL;
		int picWidth, picHeight, channels;
		imageData = SOIL_load_image(text_paths[i].c_str(), &picWidth, &picHeight, &channels, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, picWidth, picHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(imageData);
		glBindTexture(GL_TEXTURE_2D, i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textureId1);

	}


	while (!glfwWindowShouldClose(window) && e.mode != QUIT_MODE) {

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		e.width = float(width);
		e.height = float(height);
		float aspect_ratio = float(height)/float(width);
		if (e.view(0,0)/e.view(1,1) != aspect_ratio) { e.view(0,0) = aspect_ratio * e.view(1,1); }

		// Clear the framebuffer
		//glClearColor(1.0f, 1.0f, 1.0f, 0.4f);
		if (std::string(argv[1]) == "../src/solar_sys/data.json") {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		} else { glClearColor(1.0f, 1.0f, 1.0f, 0.4f); }
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1i(program.uniform("normal_type"), e.normal_type);
		glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, e.view.data());
		t_now = std::chrono::high_resolution_clock::now();
		glob_time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

		if (e.pause) {
			alpha = e.alpha_shift;
			new_t_start = std::chrono::high_resolution_clock::now();
		} 
		else {
			time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - new_t_start).count();
			alpha = e.alpha_shift + 0.3 * time;
		}
		beta = e.beta_shift;
		e.cam_pos = e.radius * Vector3f(cos(alpha)*cos(beta), sin(beta), sin(alpha)*cos(beta));
		Eigen::Matrix4f camera_d, camera_pos;
		camera_d << cos(M_PI/2.0-alpha),   0, -sin(M_PI/2.0-alpha), 0,
					-cos(alpha)*sin(beta), cos(beta),  -sin(alpha)*sin(beta),   0,
					cos(alpha)*cos(beta),  sin(beta),  sin(alpha)*cos(beta),    0,
					0,                     0, 0,                      1;
		camera_pos << 1, 0, 0, -e.cam_pos(0),
				 	  0, 1, 0, -e.cam_pos(1),
					  0, 0, 1, -e.cam_pos(2),
				 	  0, 0, 0, 1;
		e.camera = camera_d * camera_pos;
		e.alpha = alpha;
		float t = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

		glUniform1f(program.uniform("time"), t);
		glUniformMatrix4fv(program.uniform("camera"), 1, GL_FALSE, e.camera.data());
		if (e.camera_type == 0) { glUniformMatrix4fv(program.uniform("proj"), 1, GL_FALSE, e.persp.data()); } 
		else { glUniformMatrix4fv(program.uniform("proj"), 1, GL_FALSE, e.orth.data()); }
		Vector3f light_d = e.cam_pos.normalized();
		glUniform3f(program.uniform("light_d"), light_d(0), light_d(1),light_d(2));

		if (e.mode == TRANSLATION_MODE && e.ith_obj != -1) {
			Eigen::Matrix4f t_m = e.obj_vector[e.ith_obj].translation;
			Eigen::Matrix4f r_m = e.obj_vector[e.ith_obj].rotation;
			Eigen::Matrix4f s_m = e.obj_vector[e.ith_obj].scaling;
			e.obj_vector[e.ith_obj].model = t_m * r_m * s_m;
		}

		// glUniform1i(program.uniform("tex"), 0);

		// Draw triangles
		for (int j = 0; j < e.obj_vector.size(); j++) {
			glUniform1i(program.uniform("obj_id"), j); //don't forget the first 5.

			glUniform1i(program.uniform("tex"), e.obj_vector[j].text_num);
			glUniform1i(program.uniform("normalMap"), e.obj_vector[j].norm_map_num);
			if (std::string(argv[1]) == "../src/bump_map/data.json") {glUniform1i(program.uniform("depth"), (j >= 5) ? depth_vector[j-5]: 0);}

			glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, &e.obj_vector[j].model(0,0)); //&e.model(0, j * 4));
			int num_tri = e.obj_index[e.obj_vector[j].type];
			int t = 0;
			for (int k = 0; k < e.obj_vector[j].type; k++) { t += e.obj_index[k]; } // count total triangles before head
			for (int k = 0; k < num_tri; k++) {
				if (e.ith_obj == j && e.obj_clicked == 1) { glUniform1i(program.uniform("click"), 1); }
				else { glUniform1i(program.uniform("click"), 0); }

				Vector3f deltapos1 = e.V.col(t*3 + k*3 + 1) - e.V.col(t*3 + k*3);
				Vector3f deltapos2 = e.V.col(t*3 + k*3 + 2) - e.V.col(t*3 + k*3 + 1);
				Vector2f deltaUV1 = e.U.row(t*3 + k*3 + 1) - e.U.row(t*3 + k*3);
				Vector2f deltaUV2 = e.U.row(t*3 + k*3 + 2) - e.U.row(t*3 + k*3 + 1);
				float r = 1/(deltaUV1(0) * deltaUV2(1) - deltaUV1(1) * deltaUV2(0));
				Vector3f tangt = (deltapos1 * deltaUV2(1) - deltapos2 * deltaUV1(1)) * r;

				//tangt = deltapos1.normalized();
				glUniform3f(program.uniform("tangt"), tangt(0), tangt(1), tangt(2));

				if (e.normal_type != 0) {
					glUniform1i(program.uniform("draw_frame"), 0);
					Vector4f face_normal (e.N(t+k,0), e.N(t+k,1), e.N(t+k,2), 0);
					face_normal = e.obj_vector[j].model * face_normal;
					glUniform3f(program.uniform("face_normal"), face_normal(0), face_normal(1), face_normal(2));
					if (program.tess == 1) {
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glDrawArrays(GL_PATCHES, t*3 + k*3, 3);
					} else { glDrawArrays(GL_TRIANGLES, t*3 + k*3, 3); }
				}
				if (e.normal_type != 3) {
					glUniform1i(program.uniform("draw_frame"), 1);
					if (program.tess == 1) {
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						glDrawArrays(GL_PATCHES, t*3 + k*3, 3);
					} else { glDrawArrays(GL_LINE_LOOP, t*3 + k*3, 3); }
				}
			}
		}
		glfwSwapBuffers(window); // Swap front and back buffers
		glfwPollEvents(); // Poll for and process events
	}
	// Deallocate opengl memory
	program.free();
	VAO.free();
	VBO.free();
	VBO_n.free();
	VBO_t.free();
	// Deallocate glfw internals
	glfwTerminate();
	return 0;
}
