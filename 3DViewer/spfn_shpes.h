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
	int num = 0;
	vector<int> indices;
};

void importPointsFromFile(string filepath, vector<Vertex> &point_list, glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f)) {
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

void importShapeParasFromFile(string filepath, vector<SPFN_Shape> &shapes) {
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
				pre = x;
				s >> x;
				if (x == pre) {
					dirty = false;
				}
				else {
					dirty = true;
				}
			}

			shapes.push_back(temp);
		}
	}
}