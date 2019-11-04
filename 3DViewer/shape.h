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

	Shape(string paras_path, string points_path, string mask_path) {
		importPoints(points_path, glm::vec3(1.0f, 0.0f, 0.0f));
		importParas(paras_path);
		importAndSetMask(mask_path);
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
		}
	}

	void importParas(string paras_path) {
		// read file
		fstream paras_file(paras_path);
		vector<string> lines;
		string line;
		stringstream s;
		float x, y, z;
		bool dirty = false;
		int pre = -1;

		// read file
		while (getline(paras_file, line)) {
			lines.push_back(line);
		}

		// analysis shapes
		for (unsigned int i = 0; i < lines.size(); i++) {
			// every single instance
			if (lines[i].c_str()[3] == '-') {
				Primitive ins;

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
					s >> x;
					dirty = true;
					while (dirty) {
						ins.indices.push_back(x);
						ins.points.push_back(this->points[x]);
						pre = x;
						s >> x;
						if (x == pre) {
							dirty = false;
						}
						else {
							dirty = true;
						}
					}
				}

				ins.createModel();
				instances.push_back(ins);
			}
		}
	}

	void importAndSetMask(string mask_path) {
		// 
		fstream mask_file(mask_path);
		string line;
		vector<bool> masks;
		
		// read masks
		while (getline(mask_file, line)) {
			masks.push_back(bool(atoi(line.c_str())));
		}

		for (unsigned int i = 0; i < masks.size(); i++) {
			instances[i].mask = masks[i];
		}
	}

};

#endif  // SHAPE_H