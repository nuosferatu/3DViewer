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

class Shape {
public:
	vector<Primitive> instances; // 基元列表（通常是 24 个）
	vector<Vertex> points;       // 8096 个点的坐标

	vector<unsigned int> indices;
	Mesh *raw_points;


	Shape(string paras_path,
		  string points_path,
		  string mask_path,
		  string matching_path,
		  string points_gt_path)
	{
		importPoints(points_path);
		importParas(paras_path, glm::vec3(1.0f, 0.0f, 0.0f));
		importPointsGT(matching_path, points_gt_path, glm::vec3(1.0f, 0.65f, 0.0f));
	}

private:
	void importPoints(string points_path, glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f)) {
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
			indices.push_back(i);
		}

		raw_points = new Mesh(points, indices);
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
				else if (ins.type == 0) {
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
				else if (ins.type == 1) {
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
					s >> x;
					ins.paras.push_back(sqrtf(x)); // radius
				}

				// number of points
				i++;
				ins.point_num = atoi(lines[i].c_str());

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
					}
				}

				ins.createModel();
				if (ins.point_num > 0) {
					ins.model_pointclouds = new Mesh(points, ins.indices);
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
};

#endif  // SHAPE_H