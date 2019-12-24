#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <math.h>

#include "mesh.h"


inline glm::vec3 rotateVec3(glm::vec3 src, glm::vec3 dst) {
	// compute rotate angle
	glm::vec3 v_src = glm::normalize(src);
	glm::vec3 v_dst = glm::normalize(dst);
	float dot = v_src.x * v_dst.x + v_src.y * v_dst.y + v_src.z * v_dst.z;
	float angle = std::acosf(dot);

	// compute rotate axis
	glm::vec3 axis = glm::normalize(glm::cross(v_src, v_dst));

	//// rotate
	//return glm::rotate(src, angle, axis);


	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, angle, axis);
	return glm::vec3(trans * glm::vec4(src, 1.0f));
}

inline glm::vec3 projectPlane(glm::vec3 src, glm::vec3 dst) {
	
	glm::mat4 trans = glm::mat4(1.0f);

	// compute rotate angle
	glm::vec3 v_src = glm::normalize(src);
	glm::vec3 v_dst = glm::normalize(dst);
	float dot = v_src.x * v_dst.x + v_src.y * v_dst.y + v_src.z * v_dst.z;
	float angle = std::acosf(dot);

	// compute rotate axis
	glm::vec3 axis = glm::normalize(glm::cross(v_src, v_dst));


	trans = glm::rotate(trans, angle, axis);
	return glm::vec3(trans * glm::vec4(src, 1.0f));
}

#endif  // !__TRANSFORM_H__