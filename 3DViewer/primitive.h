#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "transform3d.h"
#include "model.h"

class Primitive {
public:
	/* 基元变量 */
	int type;                     // 基元类型
	vector<float> paras;          // 基元参数
	int point_num;                // 点数
	vector<unsigned int> indices; // 点在 Shape 中的索引
	//vector<Vertex> points;        // 点的坐标
	
	Mesh *model_pointclouds;      // 点云模型
	
	// Verify correction of per-point-normal
	vector<Vertex> normal_line_points;
	vector<unsigned int> normal_line_indices;  // indices for drawing normal lines as trangle
	Mesh *model_normal_line;  // visualize normals as trangles (actually lines)
	vector<Vertex> normal_line_gt_points;
	//vector<unsigned int> normal_line_gt_indices;
	Mesh* model_normal_line_gt;

	glm::vec3 center;

	/* 其他变量 */
	Model *model_shape;           // 形状模型
	bool selected;                // 是否选中
	bool mask;                    // 掩码
	vector<Vertex> points_gt;        // 真实数据：点的坐标
	vector<unsigned int> indices_gt; // 真实数据：点的索引
	Mesh *model_gt;                  // 真实数据：模型

	// 默认构造
	Primitive() : type(-1), point_num(0), model_pointclouds(nullptr), model_shape(nullptr), selected(false), mask(true) { }
	Primitive(string modelpath) : type(-1), point_num(0), model_pointclouds(nullptr), selected(false), mask(true) {
		model_shape = new Model(modelpath);
	}

	// 刚性变换
	void inline rotate(float _angle, glm::vec3 _axis) { model_shape->rotate(_angle, _axis); }                 // 旋转
	void inline rotate(float _angle, float _x, float _y, float _z) { rotate(_angle, glm::vec3(_x, _y, _z)); } // 旋转
	void inline rotate(glm::vec3 _src, glm::vec3 _dst) { model_shape->rotate(_src, _dst); }                   // 旋转
	void inline rotate(glm::vec3 _src, float _x, float _y, float _z) { rotate(_src, glm::vec3(_x, _y, _z)); } // 旋转
	void inline translate(glm::vec3 _dxyz) { model_shape->translate(_dxyz); }                                 // 平移
	void inline translate(float _x, float _y, float _z) { translate(glm::vec3(_x, _y, _z)); }                 // 平移
	void inline scale(glm::vec3 _dxyz) { model_shape->scale(_dxyz); }                                         // 缩放
	void inline scale(float _x, float _y, float _z) { scale(glm::vec3(_x, _y, _z)); }                         // 缩放

	void createModel() {
		switch (type) {
		case 0:
			//model_shape = new Model("shapes/plane_face.obj");
			model_shape = nullptr;
			//rotate(glm::vec3(0.0f, 1.0f, 0.0f), paras[0], paras[1], paras[2]);
			//translate(glm::vec3(0.0f, paras[3], 0.0f));
			//rotate(glm::vec3(0.0f, 1.0f, 0.0f), paras[0], paras[1], paras[2]);
			break;
		case 1:
			model_shape = new Model("shapes/plane_face.obj");
			rotate(glm::vec3(0.0f, 1.0f, 0.0f), paras[0], paras[1], paras[2]);
			translate(glm::vec3(0.0f, paras[3], 0.0f));
			//rotate(glm::vec3(0.0f, 1.0f, 0.0f), paras[0], paras[1], paras[2]);
			break;
		case 2:
			model_shape = new Model("shapes/cylinder_face_x.obj");
			translate(paras[0], paras[1], paras[2]);
			rotate(glm::vec3(0.0f, 1.0f, 0.0f), paras[3], paras[4], paras[5]);
			scale(paras[6], paras[6], paras[6]);
			break;
		case 3:
			model_shape = new Model("shapes/cone_face.obj");
			translate(paras[0], paras[1], paras[2]);
			rotate(glm::vec3(0.0f, -1.0f, 0.0f), paras[3], paras[4], paras[5]);
			scale(glm::vec3(1.0f, 1.0f/std::tanf(paras[6])/2.0f, 1.0f));
			break;
		default:
			break;
		}
	}
};

#endif  // PRIMITIVE_H