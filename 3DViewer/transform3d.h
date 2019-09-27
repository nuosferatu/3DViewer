#ifndef TRANSFORM3D_H
#define TRANSFORM3D_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <math.h>

namespace Transform3D {
	glm::vec3 rotate(glm::vec3 _target, glm::vec3 _axis, float _angle) {
		glm::quat rotateQuat = glm::angleAxis(_angle, _axis);
		glm::mat4 transMatrix = glm::toMat4(rotateQuat);
		return glm::vec3(transMatrix * glm::vec4(_target, 1.0f));
	}
	glm::vec3 rotate(glm::vec3 _from, glm::vec3 _to) {
		float a_mod = _from.x*_from.x + _from.y*_from.y + _from.z*_from.z;
		float b_mod = _to.x*_to.x + _to.y*_to.y + _to.z*_to.z;
		float a_dot_b = _from.x*_to.x + _from.y*_to.y + _from.z*_to.z;
		float cos_theta = a_dot_b / (a_mod*b_mod);
		float angle = acosf(cos_theta);
		glm::vec3 axis = glm::normalize(glm::cross(_from, _to));
		return rotate(_from, axis, angle);
	}
};

#endif // TRANSFORM3D_H
