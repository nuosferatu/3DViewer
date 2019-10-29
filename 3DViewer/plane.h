#ifndef PLANE_H
#define PLANE_H

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
Type: plane
Parameters:
plane_n: (-0.13145587, 0.23956053, 0.96194077)
plane_c: -0.22645676
Points: 25 points
519 926 999 2184 3005 3072 3445 3501 3514 3756 3935 4008 426 ... (25 indices)
*/

class Plane : public Primitives {
public:
	glm::vec3 plane_n;
	float plane_c;

	Plane() : Primitives("shapes/plane_face.obj") {
		plane_n = glm::vec3(0.0f, 1.0f, 0.0f);
		plane_c = 1.0f;
	}
};

#endif // PLANE_H
