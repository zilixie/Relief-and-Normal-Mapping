#ifndef READ_JSON_H
#define READ_JSON_H
#include <string>
#include <vector>
#include <json.hpp>
#include <fstream>

bool read_json(
	const std::string & json_filename,
	std::string & vs_filename,
	std::string & tcs_filename,
	std::string & tes_filename,
	std::string & fs_filename,
	std::vector<std::string> & obj_type_vector,
	std::vector<Eigen::Vector3f> & pos_vector,
	std::vector<int> & text_vector,
	std::vector<std::string> & text_paths,
	std::vector<int> & norm_vector,
	std::vector<float> & scale_vector);


bool read_json(
	const std::string & json_filename,
	std::string & vs_filename,
	std::string & tcs_filename,
	std::string & tes_filename,
	std::string & fs_filename,
	std::vector<std::string> & obj_type_vector,
	std::vector<Eigen::Vector3f> & pos_vector,
	std::vector<int> & text_vector,
	std::vector<std::string> & text_paths,
	std::vector<int> & norm_vector,
	std::vector<int> & depth_vector,
	std::vector<float> & scale_vector);


bool read_json(
	const std::string & json_filename,
	std::string & vs_filename,
	std::string & tcs_filename,
	std::string & tes_filename,
	std::string & fs_filename,
	std::vector<std::string> & obj_type_vector,
	std::vector<Eigen::Vector3f> & pos_vector,
	std::vector<int> & text_vector,
	std::vector<std::string> & text_paths,
	std::vector<int> & norm_vector,
	std::vector<float> & scale_vector)
{
	using json = nlohmann::json;

	std::ifstream input(json_filename);
	if (!input) { return false; }
	json js;
	input >> js;

	// vs_filename.clear();
	// tcs_filename.clear();
	// tes_filename.clear();
	obj_type_vector.clear();
	pos_vector.clear();
	text_vector.clear();
	text_paths.clear();
	norm_vector.clear();
	scale_vector.clear();
	vs_filename = js["vertex"];
	tcs_filename = js["tess control"];
	tes_filename = js["tess evaluation"];
	fs_filename = js["fragment"];

	for (const json & obj: js["objects"]) {
		obj_type_vector.push_back(obj["type"]);
		pos_vector.push_back(Eigen::Vector3f(obj["x"], obj["y"], obj["z"]));
		text_vector.push_back(obj["texture_num"]);
		norm_vector.push_back(obj["normal_num"]);
		scale_vector.push_back(obj["scale"]);
	}

	for (const std::string & text: js["textures"]) {
		text_paths.push_back(text);
	}
	return true;
}

bool read_json(
	const std::string & json_filename,
	std::string & vs_filename,
	std::string & tcs_filename,
	std::string & tes_filename,
	std::string & fs_filename,
	std::vector<std::string> & obj_type_vector,
	std::vector<Eigen::Vector3f> & pos_vector,
	std::vector<int> & text_vector,
	std::vector<std::string> & text_paths,
	std::vector<int> & norm_vector,
	std::vector<int> & depth_vector,
	std::vector<float> & scale_vector)
{
	using json = nlohmann::json;

	std::ifstream input(json_filename);
	if (!input) { return false; }
	json js;
	input >> js;

	// vs_filename.clear();
	// tcs_filename.clear();
	// tes_filename.clear();
	obj_type_vector.clear();
	pos_vector.clear();
	text_vector.clear();
	text_paths.clear();
	norm_vector.clear();
	scale_vector.clear();
	vs_filename = js["vertex"];
	tcs_filename = js["tess control"];
	tes_filename = js["tess evaluation"];
	fs_filename = js["fragment"];

	for (const json & obj: js["objects"]) {
		obj_type_vector.push_back(obj["type"]);
		pos_vector.push_back(Eigen::Vector3f(obj["x"], obj["y"], obj["z"]));
		text_vector.push_back(obj["texture_num"]);
		norm_vector.push_back(obj["normal_num"]);
		scale_vector.push_back(obj["scale"]);
		depth_vector.push_back(obj["depth_num"]);
	}

	for (const std::string & text: js["textures"]) {
		text_paths.push_back(text);
	}
	return true;
}

#endif
