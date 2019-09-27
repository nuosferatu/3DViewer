#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "transform3d.h"
#include "model.h"

#include "primitive.h"

/*
--------------- INSTANCE 1 ---------------
Type: sphere
Parameters:
sphere_center: (-0.04276157, -0.038524244, -0.02160791)
sphere_radius_squared: 1.173716
Points: 67 points
15 62 445 456 564 704 749 770 777 1048 1079 1382 ... (67 indices)
*/

class Sphere : public Primitives {
public:
	glm::vec3 sphere_center;
	float sphere_radius_squared;

	Sphere() {
		sphere_center = glm::vec3(0.0f, 0.0f, 0.0f);
		sphere_radius_squared = 1.0f;

		model_shape = new Model("shapes/sphere.obj");

		selected = FALSE;
	}

};

#endif // SPHERE_H
