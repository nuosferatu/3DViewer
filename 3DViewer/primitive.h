#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "transform3d.h"
#include "model.h"

class Primitives {
public:
	vector<unsigned int> point_indices;
	vector<glm::vec3> point_list;

	bool selected;

	Model *model_shape;

	virtual void rotate(float _angle, glm::vec3 _axis) {
		model_shape->rotate(_angle, _axis);
	}

	virtual void translate(glm::vec3 _dxyz) {
		model_shape->translate(_dxyz);
	}

	virtual void scale(glm::vec3 _dxyz) {
		model_shape->scale(_dxyz);
	}

	bool isSelected() {
		return selected;
	}
};

#endif  // PRIMITIVE_H