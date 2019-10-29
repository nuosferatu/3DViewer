#ifndef CONE_H
#define CONE_H

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
Type: cone
Parameters:
cone_apex: (-2.087124, -0.10015773, 0.093214035)
cone_axis: (0.9965138, 0.04293514, -0.07153183)
cone_half_angle: 0.6761476
Points: 344 points
10 12 48 66 81 136 154 276 298 306 307 319 327 373 382 422 4 ... (344 indices)
*/

class Cone : public Primitives {
public:
	glm::vec3 cone_apex;
	glm::vec3 cone_axis;
	float cone_half_angle;

	Cone() : Primitives("shapes/cone_face.obj") {
		cone_apex = glm::vec3(0.0f, 0.0f, 0.0f);
		cone_axis = glm::vec3(0.0f, 1.0f, 0.0f);
		cone_half_angle = 1.0f;
	}
};

#endif // CONE_H
