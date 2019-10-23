#ifndef CAMERA_H
#define CAMERA_H

// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>


// opengl & glfw
#include <GL/gl3w.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
using namespace glm;

#include <vector>

constexpr auto PI = 3.141592653;

// 相机参数
const float SENSITIVITY = 0.005f;
const float ZOOM = 25.0f;

class Camera {
public:
	// 相机属性
	glm::vec3 Position; // 相机位置
	glm::vec3 Front;    // 相机方向
	glm::vec3 Up;       // 相机上轴
	glm::vec3 Right;    // 相机右轴
	glm::vec3 WorldUp;  // 世界上向量
	glm::vec3 Target;   // 观察点

	float r;

	// 相机选项
	float MouseSensitivity; // 鼠标灵敏度
	float Zoom;             // 相机焦距

	// 构造相机（通过向量）
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 30.0f), // 相机位置
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))           // 世界上向量
		: MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Target = glm::vec3(0.0f, 0.0f, 0.0f);
		r = sqrtf(Position.x * Position.x + Position.y * Position.y + Position.z * Position.z);
		Front = glm::normalize(Target - Position);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}


	// 获取 view 矩阵
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}


	// 鼠标移动事件处理函数（改变相机角度）
	void MouseMovementMeshLabStyle(float xoffset, float yoffset)
	{
		xoffset *= MouseSensitivity; // 鼠标 x 轴偏移量（角度）
		yoffset *= MouseSensitivity; // 鼠标 y 轴偏移量（角度）

		glm::quat rotateQuat;
		glm::mat4 transMatrix;

		rotateQuat = glm::angleAxis(-xoffset, Up);
		transMatrix = glm::toMat4(rotateQuat);
		Position = vec3(transMatrix * vec4(Position, 1.0f));
		Front = glm::normalize(Target - Position);
		Right = glm::normalize(glm::cross(Front, Up));

		rotateQuat = glm::angleAxis(yoffset, Right);
		transMatrix = glm::toMat4(rotateQuat);
		Position = vec3(transMatrix * vec4(Position, 1.0f));
		Front = glm::normalize(Target - Position);
		Up = glm::normalize(glm::cross(Right, Front));
	}

	// 鼠标移动事件处理函数（改变相机角度）
	void MouseMovementPlatformStyle(float xoffset, float yoffset)
	{
		xoffset *= MouseSensitivity; // 鼠标 x 轴偏移量（角度）
		yoffset *= MouseSensitivity; // 鼠标 y 轴偏移量（角度）

		glm::quat rotateQuat;
		glm::mat4 transMatrix;

		rotateQuat = glm::angleAxis(-xoffset, WorldUp);
		transMatrix = glm::toMat4(rotateQuat);
		Position = vec3(transMatrix * vec4(Position, 1.0f));
		Front = glm::normalize(Target - Position);
		Up = vec3(transMatrix * vec4(Up, 1.0f));
		Right = glm::normalize(glm::cross(Front, Up));

		rotateQuat = glm::angleAxis(yoffset, Right);
		transMatrix = glm::toMat4(rotateQuat);
		Position = vec3(transMatrix * vec4(Position, 1.0f));
		Front = glm::normalize(Target - Position);
		Up = glm::normalize(glm::cross(Right, Front));
	}


	void translate(float xoffset, float yoffset)
	{
		xoffset *= MouseSensitivity; // 鼠标 x 轴偏移量（角度）
		yoffset *= MouseSensitivity; // 鼠标 y 轴偏移量（角度）

		/*Position += Right * (-xoffset);
		Position += Up * (-yoffset);
		Target += Right * (-xoffset);
		Target += Up * (-yoffset);*/

		Position += Right * (-xoffset);
		Position += Up * (-yoffset);
		Target += Right * (-xoffset);
		Target += Up * (-yoffset);
	}


	// 鼠标滚轮事件响应函数（改变相机距观察点距离）
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 0.1f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 0.1f)
			Zoom = 0.1f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}


	// 重置相机
	void resetCamera()
	{
		Position = glm::vec3(0.0f, 0.0f, 30.0f);
		WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		Target = glm::vec3(0.0f, 0.0f, 0.0f);
		r = sqrtf(Position.x * Position.x + Position.y * Position.y + Position.z * Position.z);
		Zoom = ZOOM;
		MouseSensitivity = SENSITIVITY;

		Front = glm::normalize(Target - Position);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif // CAMERA_H