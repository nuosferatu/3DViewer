#ifndef SHAPE_H
#define SHAPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "primitive.h"
#include "cone.h"
#include "cylinder.h"
#include "sphere.h"
#include "plane.h"
#include <Windows.h>
#include "transform.h"

struct SPFN_GTInfo {
	int id;
	string type;
	float para1;
	float para2;
	float para3;
};

class Shape {
public:
	vector<Primitive> instances; // 基元列表（通常是 24 个）
	vector<Vertex> points;       // 8096 个点的坐标
	vector<Vertex> points_highlight;

	vector<unsigned int> indices;
	Mesh *raw_points;
	vector<SPFN_GTInfo> gt_info;

	// verify correction of per-point-normal
	vector<Vertex> normal_line_points;  // save normal as a triangle with 3 points (2 same 1 new)
	vector<Vertex> normal_line_gt_points;
	

	Shape(string paras_path,
		  string points_path,
		  string normals_path,
		  string normalsgt_path,
		  string mask_path,
		  string matching_path,
		  string points_gt_path,
		  string gtinfo_path)
	{
		importPoints(points_path);
		importPointsNormal(normals_path);
		importNormalsGT(normalsgt_path);
		importParas(paras_path, glm::vec3(1.0f, 0.0f, 0.0f));
		importPointsGT(matching_path, points_gt_path, glm::vec3(1.0f, 0.65f, 0.0f));
		importGTInfo(gtinfo_path);
	}

private:
	void importPoints(string points_path, glm::vec3 _color = glm::vec3(0.8f, 0.8f, 0.8f)) {
		// read file
		fstream file(points_path);
		vector<string> lines;
		string line;
		stringstream s;
		float x, y, z;
		while (getline(file, line)) {
			lines.push_back(line);
		}

		// points
		for (unsigned int i = 0; i < lines.size(); i++) {
			s.str("");
			s.clear();
			s << lines[i];
			s >> x >> y >> z;
			points.push_back(Vertex(glm::vec3(x, y, z), _color));
			points_highlight.push_back(Vertex(glm::vec3(x, y, z), glm::vec3(0.0f, 1.0f, 0.0f)));
			indices.push_back(i);
		}

		raw_points = new Mesh(points, indices);
	}

	// Read point normal
	void importPointsNormal(string normals_path) {
		// read file
		fstream file(normals_path);
		vector<string> lines;
		string line;
		stringstream s;
		float x, y, z;
		while (getline(file, line)) {
			lines.push_back(line);
		}

		// point normal
		for (unsigned int i = 0; i < lines.size(); i++) {
			s.str("");
			s.clear();
			s << lines[i];
			s >> x >> y >> z;
			normal_line_points.push_back(Vertex(glm::vec3(0.1*x+points[i].position.x, 
				0.1*y+points[i].position.y,
				0.1*z+points[i].position.z), glm::vec3(1.0f, 1.0f, 1.0f)));  // new point
		}
	}

	// Read ground-truth of point normal
	void importNormalsGT(string normalsgt_path) {
		// read file
		fstream file(normalsgt_path);
		vector<string> lines;
		string line;
		stringstream s;
		float x, y, z;
		while (getline(file, line)) {
			lines.push_back(line);
		}

		// ground-truth of point normal
		for (unsigned int i = 0; i < lines.size(); i++) {
			s.str("");
			s.clear();
			s << lines[i];
			s >> x >> y >> z;
			normal_line_gt_points.push_back(Vertex(glm::vec3(0.1 * x + points[i].position.x,
				0.1 * y + points[i].position.y,
				0.1 * z + points[i].position.z), glm::vec3(1.0f, 1.0f, 1.0f)));  // new point

		}
	}

	void importParas(string paras_path, glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f)) {
		// read file
		fstream paras_file(paras_path);
		vector<string> lines;
		string line;
		stringstream s;
		float x, y, z;
		bool dirty = false;
		int pre = -1;
		int m;

		// read file
		while (getline(paras_file, line)) {
			lines.push_back(line);
		}

		// analysis shapes
		for (unsigned int i = 0; i < lines.size(); i++) {
			// every single instance
			if (lines[i].c_str()[3] == '-') {
				Primitive ins;
				ins.mask = false;

				// type
				i++;
				ins.type = atoi(lines[i].substr(2).c_str());

				/* parameters */
				i++;
				// cone
				if (ins.type == 3) {
					s.str("");
					s.clear();
					s << lines[i];
					s >> x >> y >> z;
					ins.paras.push_back(x); // apex
					ins.paras.push_back(y);
					ins.paras.push_back(z);
					ins.center = glm::vec3(x, y, z);
					i++;
					s.str("");
					s.clear();
					s << lines[i];
					s >> x >> y >> z;
					ins.paras.push_back(x); // axis
					ins.paras.push_back(y);
					ins.paras.push_back(z);
					i++;
					s.str("");
					s.clear();
					s << lines[i];
					s >> x;
					ins.paras.push_back(x); // half angle
				}
				// cylinder
				else if (ins.type == 2) {
					s.str("");
					s.clear();
					s << lines[i];
					s >> x >> y >> z;
					ins.paras.push_back(x); // center
					ins.paras.push_back(y);
					ins.paras.push_back(z);
					ins.center = glm::vec3(x, y, z);
					i++;
					s.str("");
					s.clear();
					s << lines[i];
					s >> x >> y >> z;
					ins.paras.push_back(x); // axis
					ins.paras.push_back(y);
					ins.paras.push_back(z);
					i++;
					s.str("");
					s.clear();
					s << lines[i];
					s >> x;
					ins.paras.push_back(sqrtf(x)); // radius
				}
				// plane
				else if (ins.type == 1) {
					s.str("");
					s.clear();
					s << lines[i];
					s >> x >> y >> z;
					ins.paras.push_back(x); // n
					ins.paras.push_back(y);
					ins.paras.push_back(z);
					ins.center = glm::vec3(x, y, z);
					i++;
					s.str("");
					s.clear();
					s << lines[i];
					s >> x;
					ins.paras.push_back(x); // c
					ins.center *= x;
				}
				// sphere
				else if (ins.type == 0) {
					i++;
					//s.str("");
					//s.clear();
					//s << lines[i];
					//s >> x >> y >> z;
					//ins.paras.push_back(x); // center
					//ins.paras.push_back(y);
					//ins.paras.push_back(z);
					//ins.center = glm::vec3(x, y, z);
					//i++;
					//s.str("");
					//s.clear();
					//s << lines[i];
					//s >> x;
					//ins.paras.push_back(sqrtf(x)); // radius
				}

				// number of points
				i++;
				ins.point_num = atoi(lines[i].c_str());

				glm::mat4 trans = glm::mat4(1.0f);
				glm::mat4 trans1 = glm::mat4(1.0f);
				glm::mat4 trans2 = glm::mat4(1.0f);
				// list of point indices
				if (ins.point_num > 0) {
					i++;
					s.str("");
					s.clear();
					s << lines[i];
					for (unsigned int pp = 0; pp < ins.point_num; pp++) {
						//s.str("");
						//s.clear();
						//s << lines[i];
						s >> m;
						//cout << m << " ";
						ins.indices.push_back(m);
						//ins.points.push_back(Vertex(this->points[m].position, _color));
						ins.normal_line_points.push_back(normal_line_points[m]);
						ins.normal_line_points.push_back(points[m]);
						ins.normal_line_points.push_back(points[m]);
						ins.normal_line_gt_points.push_back(normal_line_gt_points[m]);
						ins.normal_line_gt_points.push_back(points[m]);
						ins.normal_line_gt_points.push_back(points[m]);
						ins.normal_line_indices.push_back(3*pp);
						ins.normal_line_indices.push_back(3*pp+1);
						ins.normal_line_indices.push_back(3*pp+2);
						//pp = ins.point_num;


						// projective points of plane:
						if (ins.type == 1) {
							Vertex p(points[m].position);
							if (pp == 0) {
								glm::vec3 v_src = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
								glm::vec3 v_dst = glm::normalize(glm::vec3(ins.paras[0], ins.paras[1], ins.paras[2]));
								float dot = v_src.x * v_dst.x + v_src.y * v_dst.y + v_src.z * v_dst.z;
								float angle = std::acosf(dot);
								glm::vec3 axis = glm::normalize(glm::cross(v_src, v_dst));
								trans = glm::rotate(trans, angle, axis);
							}
							p.position = glm::vec3(trans * glm::vec4(p.position, 1.0f));
							p.position.y = -ins.paras[3];

							p.position = glm::vec3((-trans) * glm::vec4(p.position, 1.0f));

							ins.proj_points.push_back(p);
							ins.proj_points.push_back(p);
							ins.proj_points.push_back(p);

						}

						// Projective points of cylinder
						if (ins.type == 2) {

							Vertex p(points[m].position);

							if (pp == 0) {
								// compute translate vector
								glm::vec3 v_translate = glm::vec3(ins.paras[0], ins.paras[1], ins.paras[2]);
								
								// compute rotate axis & angle
								glm::vec3 v_src = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
								glm::vec3 v_dst = glm::normalize(glm::vec3(ins.paras[3], ins.paras[4], ins.paras[5]));
								float dot = v_src.x * v_dst.x + v_src.y * v_dst.y + v_src.z * v_dst.z;
								float angle = std::acosf(dot);
								glm::vec3 axis = glm::normalize(glm::cross(v_src, v_dst)); 

								// construct transform matrix
								trans = glm::rotate(trans, angle, axis);    // rotate
								trans1 = glm::translate(trans1, v_translate); // translate
							}

							//p.position = glm::vec3((-trans1) * glm::vec4(p.position, 1.0f));
							p.position.x = p.position.x - ins.paras[0];
							p.position.y = p.position.y - ins.paras[1];
							p.position.z = p.position.z - ins.paras[2];
							p.position = glm::vec3(trans * glm::vec4(p.position, 1.0f));
							glm::vec3 p_on_y = glm::vec3(0.0f, p.position.y, 0.0f);
							glm::vec3 entend_line = glm::vec3(p.position.x-p_on_y.x, p.position.y - p_on_y.y, p.position.z - p_on_y.z);
							entend_line = glm::normalize(entend_line);
							p.position = p_on_y + (entend_line * ins.paras[6]);
							p.position = glm::vec3((-trans) * glm::vec4(p.position, 1.0f));
							//p.position = glm::vec3((trans1) * glm::vec4(p.position, 1.0f));
							p.position.x = p.position.x + ins.paras[0];
							p.position.y = p.position.y + ins.paras[1];
							p.position.z = p.position.z + ins.paras[2];

							ins.proj_points.push_back(p);
							ins.proj_points.push_back(p);
							ins.proj_points.push_back(p);
						}
						
					}

					ins.model_normal_line = new Mesh(ins.normal_line_points, ins.normal_line_indices);
					ins.model_normal_line_gt = new Mesh(ins.normal_line_gt_points, ins.normal_line_indices);
					if (ins.type == 1 || ins.type == 2) {
						ins.model_proj_points = new Mesh(ins.proj_points, ins.normal_line_indices);
					}

				}

				

				ins.createModel();
				if (ins.point_num > 0) {
					ins.model_pointclouds = new Mesh(points_highlight, ins.indices);
				}
				else {
					ins.model_pointclouds = nullptr;
				}
				instances.push_back(ins);
			}
		}
	}

	void importPointsGT(string points_gt_path, string matching_path, glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f)) {
		// read file
		fstream gt_file(points_gt_path);
		fstream matching_file(matching_path);
		vector<string> lines;
		vector<string> lines1;
		string line;
		stringstream s;
		float x, y, z;
		Primitive *p = nullptr;
		unsigned int ins_index;
		unsigned int ins_index1;

		// read file
		while (getline(gt_file, line)) {
			lines.push_back(line);
		}
		while (getline(matching_file, line)) {
			lines1.push_back(line);
		}

		// analysis shapes
		for (unsigned int i = 0; i < lines.size(); i++) {
			// every single instance
			if (lines[i].c_str()[3] == '-') {
				ins_index = atoi(lines[i].substr(4).c_str()) - 1;
				ins_index1 = atoi(lines1[ins_index].c_str());
				if (ins_index1 > 0) {
					instances[ins_index1].mask = true;
					for (unsigned int j = 0; j < 512; j++) {
						i++;
						s.str("");
						s.clear();
						s << lines[i];
						s >> x >> y >> z;
						instances[ins_index1].points_gt.push_back(Vertex(glm::vec3(x, y, z), _color));
						instances[ins_index1].indices_gt.push_back(j);
					}
					instances[ins_index1].model_gt = new Mesh(instances[ins_index1].points_gt, instances[ins_index1].indices_gt);
				}
			}
		}
	}

	void importGTInfo(string gtinfo_path) {
		// read file
		fstream paras_file(gtinfo_path);
		string line;
		vector<string> lines;
		stringstream s;
		float x, y, z;
		int u;
		int v;
		string types[] = { "none", "plane", "cylinder", "cone" };
		// read file
		while (getline(paras_file, line)) {
			lines.push_back(line);
		}

		// analysis shapes
		for (unsigned int i = 0; i < lines.size(); i++) {
			// every single instance
			s.str("");
			s.clear();
			s << lines[i];
			s >> u >> v >> x >> y >> z;
			types[v];
			gt_info.push_back(SPFN_GTInfo{u, types[v], x, y, z});
		}
	}
};

#endif  // SHAPE_H