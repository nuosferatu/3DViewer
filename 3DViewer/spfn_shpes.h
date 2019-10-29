#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "mesh.h"

using namespace std;

struct SPFN_Shape {
	int id = 0;
	int type = -1; // "cone", "cylinder", "plane", "sphere"
	vector<float> paras;
	int num = 0; // 点的数量
	vector<Vertex> points; // 点的坐标
	vector<unsigned int> indices; // 点在 8096 个点中的索引
	Mesh *sub_point_clouds; // 这些点构成的小点云（用于绘制）
};

void ReadFilelist(string _filepath, vector<string>& _filelist) {
	fstream file(_filepath);
	string line;

	// read file
	while (getline(file, line)) {
		_filelist.push_back(line);
	}
}

void importPointsFromFile(string filepath, vector<Vertex> &point_list, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f)) {
	fstream file(filepath);
	vector<string> lines;
	string line;
	stringstream s;
	float x, y, z;

	// read file
	while (getline(file, line)) {
		lines.push_back(line);
	}

	for (unsigned int i = 0; i < lines.size(); i++) {
		s.str("");
		s.clear();
		s << lines[i];
		s >> x >> y >> z;
		point_list.push_back(Vertex(glm::vec3(x, y, z), color));
	}

	cout << "Points: " << point_list.size() << endl;
}

void importShapeParasFromFile(string filepath, vector<SPFN_Shape> &shapes, vector<Vertex> &_point_list) {
	// some variables
	fstream file(filepath);
	vector<string> lines;
	string line;
	stringstream s;
	float x, y, z;
	bool dirty = false;
	int pre = -1;

	// read file
	while (getline(file, line)) {
		lines.push_back(line);
	}

	// analysis shapes
	for (unsigned int i = 0; i < lines.size(); i++) {
		// every single instance
		if (lines[i].c_str()[0] == '-') {
			SPFN_Shape temp;

			// id
			temp.id = atoi(lines[i].substr(4).c_str());

			// type, "cone", "cylinder", "plane", "sphere"
			i++;
			if (lines[i] == string("cone")) {
				temp.type = 3;
			}
			else if (lines[i] == string("cylinder")) {
				temp.type = 2;
			}
			else if (lines[i] == string("plane")) {
				temp.type = 0;
			}
			else if (lines[i] == string("sphere")) {
				temp.type = 1;
			}
			i++;

			/* parameters */
			// cone
			if (temp.type == 3) {
				s.str("");
				s.clear();
				s << lines[i];
				s >> x >> y >> z;
				temp.paras.push_back(x); // apex (x, y, z)
				temp.paras.push_back(y);
				temp.paras.push_back(z);
				i++;
				s.str("");
				s.clear();
				s << lines[i];
				s >> x >> y >> z;
				temp.paras.push_back(x); // axis (x, y, z)
				temp.paras.push_back(y);
				temp.paras.push_back(z);
				i++;
				s.str("");
				s.clear();
				s << lines[i];
				s >> x;
				temp.paras.push_back(x); // half angle
			}
			// cylinder
			else if (temp.type == 2) {
				s.str("");
				s.clear();
				s << lines[i];
				s >> x >> y >> z;
				temp.paras.push_back(x); // center (x, y, z)
				temp.paras.push_back(y);
				temp.paras.push_back(z);
				i++;
				s.str("");
				s.clear();
				s << lines[i];
				s >> x >> y >> z;
				temp.paras.push_back(x); // axis (x, y, z)
				temp.paras.push_back(y);
				temp.paras.push_back(z);
				i++;
				s.str("");
				s.clear();
				s << lines[i];
				s >> x;
				temp.paras.push_back(x); // radius squared
			}
			// plane
			else if (temp.type == 0) {
				s.str("");
				s.clear();
				s << lines[i];
				s >> x >> y >> z;
				temp.paras.push_back(x); // n (x, y, z)
				temp.paras.push_back(y);
				temp.paras.push_back(z);
				i++;
				s.str("");
				s.clear();
				s << lines[i];
				s >> x;
				temp.paras.push_back(x); // c
			}
			// sphere
			else if (temp.type == 1) {
				s.str("");
				s.clear();
				s << lines[i];
				s >> x >> y >> z;
				temp.paras.push_back(x); // center (x, y, z)
				temp.paras.push_back(y);
				temp.paras.push_back(z);
				i++;
				s.str("");
				s.clear();
				s << lines[i];
				s >> x;
				temp.paras.push_back(x); // radius squared
			}

			// number of points
			i++;
			temp.num = atoi(lines[i].c_str());

			// list of point indices
			i++;
			s.str("");
			s.clear();
			s << lines[i];
			s >> x;
			dirty = true;
			while (dirty) {
				temp.indices.push_back(x);
				temp.points.push_back(_point_list[x]);
				pre = x;
				s >> x;
				if (x == pre) {
					dirty = false;
				}
				else {
					dirty = true;
				}
			}
			vector<Texture> texture_coords;
			temp.sub_point_clouds = new Mesh(temp.points, temp.indices, texture_coords);
			shapes.push_back(temp);
		}
	}
}

void ConstructPrimitives(vector<SPFN_Shape> _spfn_shapes, vector<Primitives>& _shapes, int _bias = 0) {
	for (unsigned int i = 0; i < _spfn_shapes.size(); i++) {
		//if (spfn_shapes[i].num < 600) {
		//	continue;
		//}
		// cylinder
		if (_spfn_shapes[i].type == 2 && _spfn_shapes[i].num > _bias) {
			Cylinder cylinder_temp;
			// scale: radius_squared
			float r, x, y, z;
			x = _spfn_shapes[i].paras[3];
			y = _spfn_shapes[i].paras[4];
			z = _spfn_shapes[i].paras[5];
			cout << x << " " << y << " " << z << " " << endl;
			cylinder_temp.rotate(cylinder_temp.cylinder_axis, glm::vec3(x, y, z));
			cylinder_temp.cylinder_radius_squared = _spfn_shapes[i].paras[6];
			r = std::sqrtf(_spfn_shapes[i].paras[6]);
			cylinder_temp.scale(glm::vec3(r, 1.0f, r));
			cout << r << endl;
			// rotate: axis

			cylinder_temp.cylinder_axis = glm::vec3(x, y, z);
			// translate: center
			x = _spfn_shapes[i].paras[0];
			y = _spfn_shapes[i].paras[1];
			z = _spfn_shapes[i].paras[2];
			cout << x << " " << y << " " << z << endl;
			cylinder_temp.translate(glm::vec3(x, y, z));
			cylinder_temp.cylinder_center = glm::vec3(x, y, z);
			_shapes.push_back(cylinder_temp);
			//cout << endl;
		}
		// cone
		else if (_spfn_shapes[i].type == 3 && _spfn_shapes[i].num > _bias) {
			Cone cone_temp;
			// scale: half_angle
			float r, x, y, z;
			cone_temp.cone_half_angle = _spfn_shapes[i].paras[6];
			r = std::sqrtf(_spfn_shapes[i].paras[6]);
			x = _spfn_shapes[i].paras[3];
			y = _spfn_shapes[i].paras[4];
			z = _spfn_shapes[i].paras[5];
			cone_temp.rotate(cone_temp.cone_axis, glm::vec3(x, y, z));
			cone_temp.scale(glm::vec3(1.0f, r / 2, 1.0f));
			cout << r << endl;
			// rotate: axis

			cout << x << " " << y << " " << z << " " << endl;

			cone_temp.cone_axis = glm::vec3(x, y, z);
			// translate: apex
			x = _spfn_shapes[i].paras[0];
			y = _spfn_shapes[i].paras[1];
			z = _spfn_shapes[i].paras[2];
			cout << x << " " << y << " " << z << endl;
			//cone_temp.translate(glm::vec3(x, y, z));
			cone_temp.cone_apex = glm::vec3(x, y, z);
			_shapes.push_back(cone_temp);
			cout << endl;
		}
		// sphere
		else if (_spfn_shapes[i].type == 1 && _spfn_shapes[i].num > _bias) {
			Sphere sphere_temp;
			// scale: radius_squared
			float r, x, y, z;
			sphere_temp.sphere_radius_squared = _spfn_shapes[i].paras[3];
			r = std::sqrtf(_spfn_shapes[i].paras[3]);
			sphere_temp.scale(glm::vec3(r, r, r));
			cout << r << endl;
			// translate: center
			x = _spfn_shapes[i].paras[0];
			y = _spfn_shapes[i].paras[1];
			z = _spfn_shapes[i].paras[2];
			cout << x << " " << y << " " << z << endl;
			sphere_temp.translate(glm::vec3(x, y, z));
			sphere_temp.sphere_center = glm::vec3(x, y, z);
			_shapes.push_back(sphere_temp);
			cout << endl;
		}
		// plane
		else if (_spfn_shapes[i].type == 0 && _spfn_shapes[i].num > _bias) {
			Plane plane_temp;
			// rotate: n
			float c, x, y, z;
			x = _spfn_shapes[i].paras[0];
			y = _spfn_shapes[i].paras[1];
			z = _spfn_shapes[i].paras[2];
			cout << x << " " << y << " " << z << endl;
			plane_temp.rotate(plane_temp.plane_n, glm::vec3(x, y, z));
			plane_temp.plane_n = glm::vec3(x, y, z);
			// translate: c
			c = _spfn_shapes[i].paras[3];
			plane_temp.translate(glm::vec3(0.0f, c, 0.0f));
			plane_temp.plane_c = _spfn_shapes[i].paras[3];
			cout << c << endl;
			_shapes.push_back(plane_temp);
			cout << endl;
		}
	}
}