#ifndef CYLINDER_H
#define CYLINDER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "transform3d.h"
#include "model.h"

#include "primitive.h"
#include <Windows.h>

/*
--------------- INSTANCE 1 ---------------
Type: cylinder
Parameters: 
cylinder_center: (4.1555075e-05, -0.0042396677, -0.0018813601)
cylinder_axis: (-0.99995697, -0.007219044, -0.0058186296)    // 单位向量
cylinder_radius_squared: 0.96927994
Points: 4080 points
0 1 2 4 5 9 11 13 14 16 17 18 19 23 25 26 28 29 31 32 33 36 37 38 40 41 42 4 ... (4080 indices)
*/

class Cylinder : public Primitive {
public:
	glm::vec3 cylinder_center;
	glm::vec3 cylinder_axis;
	float cylinder_radius_squared;

	Cylinder() : Primitive("shapes/cylinder_face_x.obj") {
		cylinder_center = glm::vec3(0.0f, 0.0f, 0.0f);
		cylinder_axis = glm::vec3(0.0f, 1.0f, 0.0f);
		cylinder_radius_squared = 1.0f;
	}
};

#endif // CYLINDER_H
