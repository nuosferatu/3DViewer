// dear imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include "my_ui.h"

// gl3w
#include <GL/gl3w.h>

// glfw
#include <GLFW/glfw3.h>
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW/glfw3native.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// other
#include "stb_image.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"
#include "transform3d.h"
#include "primitive.h"
#include "cylinder.h"
#include "cone.h"
#include "sphere.h"
#include "plane.h"
#include "shape.h"
#include "resource.h"

// std
#include <Windows.h>
#include <iostream>
#include <vector>
#include <math.h>


/* 一些全局变量 */
const static char* glsl_version = "#version 130";

// 窗口参数
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
unsigned int current_scr_width = SCR_WIDTH;
unsigned int current_scr_height = SCR_HEIGHT;
const static char* WINDOW_NAME = "3D VIEWER";

// 摄像机参数
Camera camera;
float lastX, lastY;
bool firstMouse = true;

// 其它全局变量
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

Shader *modelShader;
Shader *floorShader;
Shader *basicShapeShader;
Shader *spfnPointCloudShader;
Shader *wireframeShader;

// 一些状态量
int displayMode = GL_LINE; // 渲染模式
bool enableMouseMovement = false;
bool enableWorldAxis = true;
bool enableModelAxis = false;
int current = 0;
unsigned int highlight = -1;
bool enableMouseMiddleMovement = false;
int spfn_displayPoints = 1;
bool spfn_switchBetween = false;
bool spfn_hideUnselected = false;
bool spfn_displayGT = false;
bool spfn_displayPred = false;
bool spfn_displayRaw = false;
bool spfn_displayGTInfo = false;
bool displayFloor = true;
bool isMeshLabStyle = false;
bool displayNormalLines = true;
bool displayNormalLinesGT = false;

string primitives_name[] = {
	"plane",
	"sphere",
	"cylinder",
	"cone"
};

glm::mat4 raw_point_model = glm::mat4(1.0f);

vector<Shape> shapes;
int currentFileIndex = -1;
vector<string> filelist;




Mesh *axis_mesh;
Mesh *floor_mesh;
Mesh *axis_degree;
Shape *temp;

void switchHighlight() {
	// switch among all
	if (spfn_switchBetween) {
		highlight++;
		highlight = highlight % shapes[currentFileIndex].instances.size();
		for (unsigned int s = 0; s < shapes[currentFileIndex].instances.size(); s++)
			shapes[currentFileIndex].instances[s].selected = false;
		shapes[currentFileIndex].instances[highlight].selected = true;
	}
	// Switch among matched
	else {
		do {
			highlight++;
			highlight = highlight % shapes[currentFileIndex].instances.size();
		} while (shapes[currentFileIndex].instances[highlight].mask == false);
		for (unsigned int s = 0; s < shapes[currentFileIndex].instances.size(); s++)
			shapes[currentFileIndex].instances[s].selected = false;
		shapes[currentFileIndex].instances[highlight].selected = true;
	}
}

// 窗口尺寸变化回调函数 -------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	current_scr_width = width;
	current_scr_height = height;
	glViewport(0, 0, width, height);
}

// 窗口渲染循环中按键检测函数 -------------------------------------------------
void processInput(GLFWwindow *window) {
	// 摄像机移动
	// MeshLab(0) or FPS(1)
	switch (isMeshLabStyle)
	{
	case 0:
		//if (glfwGetKey(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		//{
		//	//if (enableMouseMovement == FALSE)
		//	//{
		//		//enableMouseMovement = TRUE;
		//		cout << "鼠标左键按下" << endl;
		//	//}
		//}
		//if (glfwGetKey(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		//{
		//	//if (enableMouseMovement == TRUE)
		//	//{
		//		//enableMouseMovement = FALSE;
		//		cout << "鼠标左键释放" << endl;
		//	//}
		//}
		//if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		//	cout << "W" << endl;
		break;
	case 1:
		//if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		//	camera.ProcessKeyboard(FORWARD, deltaTime);
		//if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		//	camera.ProcessKeyboard(BACKWARD, deltaTime);
		//if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		//	camera.ProcessKeyboard(LEFT, deltaTime);
		//if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		//	camera.ProcessKeyboard(RIGHT, deltaTime);

	default:
		break;
	}



}

// 鼠标移动检测回调函数 -------------------------------------------------------
void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos) {
	// 初始化鼠标位置
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	switch (isMeshLabStyle)
	{
	case 1:
		if (enableMouseMovement) {
			// 计算鼠标移动量
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;
			// 摄像机发生变化
			camera.MouseMovementMeshLabStyle(xoffset, yoffset);
		}
		if (enableMouseMiddleMovement) {
			// 计算鼠标移动量
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;
			// 摄像机发生变化
			camera.translate(xoffset, yoffset);
		}
		break;
	case 0:
		if (enableMouseMovement) {
			// 计算鼠标移动量
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;
			// 摄像机发生变化
			camera.MouseMovementPlatformStyle(xoffset, yoffset);
		}
		if (enableMouseMiddleMovement) {
			// 计算鼠标移动量
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;
			// 摄像机发生变化
			camera.translate(xoffset, yoffset);
		}
		break;
	//case FPS_STYLE:
	//	if (enableMouseMovement) {
	//		// 计算鼠标移动量
	//		float xoffset = xpos - lastX;
	//		float yoffset = lastY - ypos;
	//		// 摄像机发生变化
	//		//camera.ProcessReleasedMouseMovement(xoffset, yoffset);
	//	}
	//	break;
	default:
		break;
	}

	lastX = xpos;
	lastY = ypos;
}

// 鼠标滚轮检测回调函数 -------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	// 摄像机发生变化
	camera.ProcessMouseScroll(yoffset);
}

// 鼠标按键检测回调函数 -------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) switch (button) // 按下
	{
	case GLFW_MOUSE_BUTTON_LEFT: // 鼠标左键
		//cout << "鼠标左键按下" << endl;
		enableMouseMovement = TRUE;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE: // 鼠标中键
		enableMouseMiddleMovement = TRUE;
		break;
	default:
		break;
	}
	if (action == GLFW_RELEASE) switch (button) // 释放
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		//cout << "鼠标左键释放" << endl;
		enableMouseMovement = FALSE;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		enableMouseMiddleMovement = FALSE;
		break;
	default:
		break;
	}
}

// 键盘按键检测回调函数 -------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) switch (key)
	{
	case GLFW_KEY_ESCAPE: // 'ESC': 关闭窗口
		glfwSetWindowShouldClose(window, true);
		break;

	case GLFW_KEY_M:      // 'M': 切换渲染模式（线框、填充）
		switch (displayMode)
		{
		case GL_FILL:
			displayMode = GL_LINE;
			cout << "Display Mode: " << "LINE" << endl;
			break;
		case GL_LINE:
			displayMode = GL_FILL;
			cout << "Display Mode: " << "FILL" << endl;
			break;
		}
		break;

	case GLFW_KEY_O: // 'O': 切换操作模式
		switch (isMeshLabStyle)
		{
		case 1:
			isMeshLabStyle = false;
			cout << "Operation Mode: " << "PLATFORM_STYLE" << endl;
			break;
		case 0:
			isMeshLabStyle = true;
			cout << "Operation Mode: " << "FPS_STYLE" << endl;
			break;
		}
		break;

	case GLFW_KEY_R: // 'R': 重置相机
		camera.resetCamera();
		cout << "相机已重置" << endl;
		break;

	case GLFW_KEY_TAB: // 'TAB': 切换高亮
		// Switch between matched
		switchHighlight();
		break;

	case GLFW_KEY_X: // 'X': 切换坐标轴显示与否
		enableWorldAxis = !enableWorldAxis;
		break;

	case GLFW_KEY_LEFT: // 左方向键，上一个文件
		if (currentFileIndex == 0)
			currentFileIndex = 0;
		else
			currentFileIndex--;
		cout << "File name: " << filelist[currentFileIndex] << endl;
		shapes[currentFileIndex].instances[0].selected = true;
		highlight = -1;
		switchHighlight();
		break;
	case GLFW_KEY_RIGHT: // 右方向键，下一个文件
		if (currentFileIndex == filelist.size() - 1)
			currentFileIndex = filelist.size() - 1;
		else
			currentFileIndex++;
		cout << "File name: " << filelist[currentFileIndex] << endl;
		if (currentFileIndex > shapes.size() - 1) {
			temp = new Shape("E:\\SPFN\\instances_paras\\nnnnnnnnnnn\\" + filelist[currentFileIndex] + ".txt",
				"E:\\SPFN\\instances_paras\\8096_pred\\" + filelist[currentFileIndex] + ".txt",
				"E:\\SPFN\\instances_paras\\normals\\" + filelist[currentFileIndex],
				"E:\\SPFN\\instances_paras\\mask\\" + filelist[currentFileIndex] + ".txt",
				"E:\\SPFN\\instances_paras\\512\\" + filelist[currentFileIndex] + "_512.txt",
				"E:\\SPFN\\instances_paras\\matching_indices\\" + filelist[currentFileIndex] + ".txt",
				"E:\\SPFN\\instances_paras\\gt\\" + filelist[currentFileIndex] + "_gt.txt");
			temp->instances[0].selected = true;
			shapes.push_back(*temp);
		}
		else {
			shapes[currentFileIndex].instances[0].selected = true;
		}
		highlight = -1;
		switchHighlight();
		break;
	}
}

// 加载纹理贴图 ---------------------------------------------------------------
unsigned int loadTexture(char const * path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// 事件监听回调函数 -----------------------------------------------------------
void listenEvents(GLFWwindow* window) {
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // 捕获鼠标
}

// 创建GLFW窗口 ---------------------------------------------------------------
GLFWwindow* initGLFW() {
	glfwInit();  // 初始化GLFW

	//opengl & glsl version

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

	// 创建GLFW窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_NAME, NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	// 事件监听
	listenEvents(window);

	return window;
}



void drawFloor()
{
	const float floor_vertices[] = {
		-12.058594, -0.000000, 12.058594,
		-9.646875, -0.000000, 12.058594,
		-7.235156, -0.000000, 12.058594,
		-4.823437, -0.000000, 12.058594,
		-2.411719, -0.000000, 12.058594,
		0.000000, -0.000000, 12.058594,
		2.411719, -0.000000, 12.058594,
		4.823437, -0.000000, 12.058594,
		7.235156, -0.000000, 12.058594,
		9.646876, -0.000000, 12.058594,
		12.058594, -0.000000, 12.058594,
		-12.058594, -0.000000, 9.646875,
		-9.646875, -0.000000, 9.646875,
		-7.235156, -0.000000, 9.646875,
		-4.823437, -0.000000, 9.646875,
		-2.411719, -0.000000, 9.646875,
		0.000000, -0.000000, 9.646875,
		2.411719, -0.000000, 9.646875,
		4.823437, -0.000000, 9.646875,
		7.235156, -0.000000, 9.646875,
		9.646876, -0.000000, 9.646875,
		12.058594, -0.000000, 9.646875,
		-12.058594, -0.000000, 7.235156,
		-9.646875, -0.000000, 7.235156,
		-7.235156, -0.000000, 7.235156,
		-4.823437, -0.000000, 7.235156,
		-2.411719, -0.000000, 7.235156,
		0.000000, -0.000000, 7.235156,
		2.411719, -0.000000, 7.235156,
		4.823437, -0.000000, 7.235156,
		7.235156, -0.000000, 7.235156,
		9.646876, -0.000000, 7.235156,
		12.058594, -0.000000, 7.235156,
		-12.058594, -0.000000, 4.823437,
		-9.646875, -0.000000, 4.823437,
		-7.235156, -0.000000, 4.823437,
		-4.823437, -0.000000, 4.823437,
		-2.411719, -0.000000, 4.823437,
		0.000000, -0.000000, 4.823437,
		2.411719, -0.000000, 4.823437,
		4.823437, -0.000000, 4.823437,
		7.235156, -0.000000, 4.823437,
		9.646876, -0.000000, 4.823437,
		12.058594, -0.000000, 4.823437,
		-12.058594, -0.000000, 2.411719,
		-9.646875, -0.000000, 2.411719,
		-7.235156, -0.000000, 2.411719,
		-4.823437, -0.000000, 2.411719,
		-2.411719, -0.000000, 2.411719,
		0.000000, -0.000000, 2.411719,
		2.411719, -0.000000, 2.411719,
		4.823437, -0.000000, 2.411719,
		7.235156, -0.000000, 2.411719,
		9.646876, -0.000000, 2.411719,
		12.058594, -0.000000, 2.411719,
		-12.058594, 0.000000, 0.000000,
		-9.646875, 0.000000, 0.000000,
		-7.235156, 0.000000, 0.000000,
		-4.823437, 0.000000, 0.000000,
		-2.411719, 0.000000, 0.000000,
		0.000000, 0.000000, 0.000000,
		2.411719, 0.000000, 0.000000,
		4.823437, 0.000000, 0.000000,
		7.235156, 0.000000, 0.000000,
		9.646876, 0.000000, 0.000000,
		12.058594, 0.000000, 0.000000,
		-12.058594, 0.000000, -2.411719,
		-9.646875, 0.000000, -2.411719,
		-7.235156, 0.000000, -2.411719,
		-4.823437, 0.000000, -2.411719,
		-2.411719, 0.000000, -2.411719,
		0.000000, 0.000000, -2.411719,
		2.411719, 0.000000, -2.411719,
		4.823437, 0.000000, -2.411719,
		7.235156, 0.000000, -2.411719,
		9.646876, 0.000000, -2.411719,
		12.058594, 0.000000, -2.411719,
		-12.058594, 0.000000, -4.823437,
		-9.646875, 0.000000, -4.823437,
		-7.235156, 0.000000, -4.823437,
		-4.823437, 0.000000, -4.823437,
		-2.411719, 0.000000, -4.823437,
		0.000000, 0.000000, -4.823437,
		2.411719, 0.000000, -4.823437,
		4.823437, 0.000000, -4.823437,
		7.235156, 0.000000, -4.823437,
		9.646876, 0.000000, -4.823437,
		12.058594, 0.000000, -4.823437,
		-12.058594, 0.000000, -7.235156,
		-9.646875, 0.000000, -7.235156,
		-7.235156, 0.000000, -7.235156,
		-4.823437, 0.000000, -7.235156,
		-2.411719, 0.000000, -7.235156,
		0.000000, 0.000000, -7.235156,
		2.411719, 0.000000, -7.235156,
		4.823437, 0.000000, -7.235156,
		7.235156, 0.000000, -7.235156,
		9.646876, 0.000000, -7.235156,
		12.058594, 0.000000, -7.235156,
		-12.058594, 0.000000, -9.646876,
		-9.646875, 0.000000, -9.646876,
		-7.235156, 0.000000, -9.646876,
		-4.823437, 0.000000, -9.646876,
		-2.411719, 0.000000, -9.646876,
		0.000000, 0.000000, -9.646876,
		2.411719, 0.000000, -9.646876,
		4.823437, 0.000000, -9.646876,
		7.235156, 0.000000, -9.646876,
		9.646876, 0.000000, -9.646876,
		12.058594, 0.000000, -9.646876,
		-12.058594, 0.000000, -12.058594,
		-9.646875, 0.000000, -12.058594,
		-7.235156, 0.000000, -12.058594,
		-4.823437, 0.000000, -12.058594,
		-2.411719, 0.000000, -12.058594,
		0.000000, 0.000000, -12.058594,
		2.411719, 0.000000, -12.058594,
		4.823437, 0.000000, -12.058594,
		7.235156, 0.000000, -12.058594,
		9.646876, 0.000000, -12.058594,
		12.058594, 0.000000, -12.058594
	};
	unsigned int floor_indices[44];
	for (int i = 0; i < 11; i++) {
		floor_indices[i * 4] = 11 * i + 1;
		floor_indices[i * 4 + 1] = (i + 1) * 11;
		floor_indices[i * 4 + 2] = i;
		floor_indices[i * 4 + 3] = i + 110;
	}
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floor_indices), floor_indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glLineWidth(1.0f);
	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, 44, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ReadFilelist(string _filepath, vector<string>& _filelist) {
	// read file
	fstream file(_filepath);
	string line;
	while (getline(file, line)) {
		_filelist.push_back(line);
	}
}


void drawInstructor(Shader *shader, Model *renderable, int render_mode, glm::mat4 proj_matrix, glm::mat4 view_matrix, glm::mat4 model_matrix = glm::mat4(1.0f), float line_width = 1.0f, float point_size = 1.0f) {
	glLineWidth(line_width);
	glPointSize(point_size);
	shader->setMat4("projection", proj_matrix);
	shader->setMat4("view", view_matrix);
	shader->setMat4("model", model_matrix);
	renderable->draw(shader, render_mode);
}
void drawInstructor(Shader *shader, Mesh *renderable, int render_mode, glm::mat4 proj_matrix, glm::mat4 view_matrix, glm::mat4 model_matrix = glm::mat4(1.0f), float line_width = 1.0f, float point_size = 1.0f) {
	glLineWidth(line_width);
	glPointSize(point_size);
	shader->setMat4("projection", proj_matrix);
	shader->setMat4("view", view_matrix);
	shader->setMat4("model", model_matrix);
	renderable->draw(shader, render_mode);
}

void initAxis()
{
	// 坐标轴
	const float axis_points[] = {
		// 位置            // 颜色
		0.0, 0.0, 0.0,     1.0, 0.0, 0.0,
		0.0, 0.0, 0.0,     0.0, 1.0, 0.0,
		0.0, 0.0, 0.0,     0.0, 0.0, 1.0,
		10.0, 0.0, 0.0,    1.0, 0.0, 0.0,
		0.0, 10.0, 0.0,    0.0, 1.0, 0.0,
		0.0, 0.0, 10.0,    0.0, 0.0, 1.0,
	};
	const unsigned int axis_indices[] = {
		0, 3, 3,
		1, 4, 4,
		2, 5, 5
	};
	Vertex vertex(glm::vec3(0.0f, 0.0f, 0.0f));
	vector<Vertex> vertices;
	glm::vec3 vec;
	for (unsigned int i = 0; i < sizeof(axis_points) / sizeof(axis_points[0]) / 2 / 3; i++) {
		vec.x = axis_points[i * 6 + 0];
		vec.y = axis_points[i * 6 + 1];
		vec.z = axis_points[i * 6 + 2];
		vertex.position = vec;
		vec.x = axis_points[i * 6 + 3];
		vec.y = axis_points[i * 6 + 4];
		vec.z = axis_points[i * 6 + 5];
		vertex.color = vec;
		vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
		//vertex.texCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	vector<unsigned int> indices;
	for (unsigned int i = 0; i < sizeof(axis_indices) / sizeof(axis_indices[0]); i++) {
		indices.push_back(axis_indices[i]);
	}
	//vector<Texture> textures;
	axis_mesh = new Mesh(vertices, indices);

	// axis degree
	vector<Vertex> ppps;
	Vertex ppp1(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Vertex ppp2(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Vertex ppp3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	Vertex ppp4(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Vertex ppp5(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Vertex ppp6(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	Vertex ppp7(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Vertex ppp8(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Vertex ppp9(glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	Vertex ppp10(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Vertex ppp11(glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Vertex ppp12(glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	ppps.push_back(ppp1);
	ppps.push_back(ppp2);
	ppps.push_back(ppp3);
	ppps.push_back(ppp4);
	ppps.push_back(ppp5);
	ppps.push_back(ppp6);
	ppps.push_back(ppp7);
	ppps.push_back(ppp8);
	ppps.push_back(ppp9);
	ppps.push_back(ppp10);
	ppps.push_back(ppp11);
	ppps.push_back(ppp12);
	vector<unsigned int> iiis;
	iiis.push_back(0);
	iiis.push_back(1);
	iiis.push_back(2);
	iiis.push_back(3);
	iiis.push_back(4);
	iiis.push_back(5);
	iiis.push_back(7);
	iiis.push_back(8);
	iiis.push_back(9);
	iiis.push_back(10);
	iiis.push_back(11);
	iiis.push_back(12);
	axis_degree = new Mesh(ppps, iiis);
}

void drawAxis(glm::mat4 _proj_matrix, glm::mat4 _view_matrix, glm::mat4 _model_matrix)
{
	//axis_mesh->draw(wireframeShader, 0);
	//axis_degree->draw(spfnPointCloudShader, 2);
	drawInstructor(wireframeShader, axis_mesh, 0, _proj_matrix, _view_matrix, _model_matrix, 2.0f);
	drawInstructor(spfnPointCloudShader, axis_degree, 2, _proj_matrix, _view_matrix, _model_matrix, 1.0f, 4.0f);
}

//------------------------------  主函数  ------------------------------

int main() {
	// 创建一个GLFW窗口对象
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = initGLFW();


	glfwSwapInterval(1); // Enable vsync
	gl3wInit();

	// 启用深度测试
	glLineWidth(1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);


	// 创建着色器
	modelShader = new Shader("model_lighting.vert", "model_lighting.frag");
	floorShader = new Shader("floor.vert", "floor.frag");
	basicShapeShader = new Shader("basic_shape.vert", "basic_shape.frag");
	spfnPointCloudShader = new Shader("spfn_point_clouds.vert", "spfn_point_clouds.frag");
	wireframeShader = new Shader("wireframe.vert", "wireframe.frag");


	// 加载模型
	Model floor("floor/c.obj");

	ReadFilelist(".filename_list_selected.txt", filelist);
	currentFileIndex = 0;

	temp = new Shape("E:\\SPFN\\instances_paras\\nnnnnnnnnnn\\" + filelist[currentFileIndex] + ".txt",
		"E:\\SPFN\\instances_paras\\8096_pred\\" + filelist[currentFileIndex] + ".txt",
		"E:\\SPFN\\instances_paras\\normals\\" + filelist[currentFileIndex],
		"E:\\SPFN\\instances_paras\\mask\\" + filelist[currentFileIndex] + ".txt",
		"E:\\SPFN\\instances_paras\\512\\" + filelist[currentFileIndex] + "_512.txt",
		"E:\\SPFN\\instances_paras\\matching_indices\\" + filelist[currentFileIndex] + ".txt",
		"E:\\SPFN\\instances_paras\\gt\\" + filelist[currentFileIndex] + "_gt.txt"
	);
	temp->instances[0].selected = true;
	shapes.push_back(*temp);
	cout << "File name: " << filelist[currentFileIndex] << endl;





	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	
	MyUi myui, myui1;

	initAxis();

	switchHighlight();

	// 视口
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);


	// 渲染循环 --------------------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		// 计算每帧时长
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 输入
		processInput(window);  // 窗口按键检测


		// 清除颜色缓存
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 显示模式
		glPolygonMode(GL_FRONT_AND_BACK, displayMode);

		glfwPollEvents();

		/* 渲染场景 ------------------------------------*/

		glm::mat4 proj_matrix = glm::perspective(glm::radians(camera.Zoom), current_scr_width / (float)current_scr_height, 0.1f, 100.0f);
		glm::mat4 view_matrix = camera.GetViewMatrix();
		

		// 地板
		glm::mat4 floor_model_matrix = glm::mat4(1.0f);
		floor_model_matrix = glm::translate(floor_model_matrix, glm::vec3(0.0f, -0.002f, 0.0f));
		floor_model_matrix = glm::scale(floor_model_matrix, glm::vec3(0.2f, 0.2f, 0.2f));
		floorShader->setMat4("projection", proj_matrix);
		floorShader->setMat4("view", view_matrix);
		floorShader->setMat4("model", floor_model_matrix);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(1.0f);
		if (displayFloor) {
			floor.draw(floorShader, 1);
		}
		

		// 世界坐标系坐标轴
		glm::mat4 world_model_matrix = glm::mat4(1.0f);
		world_model_matrix = glm::translate(world_model_matrix, glm::vec3(0.0f, 0.002f, 0.0f));
		world_model_matrix = glm::scale(world_model_matrix, glm::vec3(0.5f, 0.5f, 0.5f));
		wireframeShader->setMat4("projection", proj_matrix);
		wireframeShader->setMat4("view", view_matrix);
		wireframeShader->setMat4("model", world_model_matrix);
		wireframeShader->setBool("selected", TRUE);
		if (enableWorldAxis) {
			drawAxis(proj_matrix, view_matrix, world_model_matrix);
		}

		// Model
		basicShapeShader->setVec3("lights[0].position", 0.0f, 30.0f, 4.0f);
		basicShapeShader->setVec4("lights[0].ambient", 0.24f, 0.24f, 0.24f, 1.0f);
		basicShapeShader->setVec4("lights[0].diffuse", 0.8f, 0.8f, 0.8f, 1.0f);
		basicShapeShader->setVec4("lights[0].specular", 0.8f, 0.8f, 0.8f, 1.0f);
		//basicShapeShader->setVec3("lights[1].position", 0.0f, -30.0f, 4.0f);
		//basicShapeShader->setVec4("lights[1].ambient", 0.1f, 0.1f, 0.1f, 0.18f);
		//basicShapeShader->setVec4("lights[1].diffuse", 0.6f, 0.6f, 0.6f, 0.5f);
		//basicShapeShader->setVec4("lights[1].specular", 0.0f, 0.0f, 0.0f, 0.0f);
		//basicShapeShader->setFloat("light.constant", 1.0f);
		//basicShapeShader.setFloat("light.linear", 0.09f);
		//basicShapeShader.setFloat("light.quadratic", 0.032f);
		basicShapeShader->setVec3("viewPos", camera.Position);
		basicShapeShader->setVec4("material.ambient", 0.4f, 0.4f, 0.4f, 0.3f);
		basicShapeShader->setVec4("material.diffuse", 0.3f, 0.3f, 0.3f, 0.3f);
		basicShapeShader->setVec4("material.specular", 0.0f, 0.0f, 0.0f, 1.0f);
		basicShapeShader->setFloat("material.shininess", 64.0f);
		basicShapeShader->setVec4("material2.ambient", 1.0f, 0.7f, 0.2f, 0.3f);
		basicShapeShader->setVec4("material2.diffuse", 1.0f, 1.0f, 1.0f, 0.3f);
		basicShapeShader->setVec4("material2.specular", 0.0f, 0.0f, 0.0f, 1.0f);
		basicShapeShader->setFloat("material2.shininess", 64.0f);



		for (unsigned int s = 0; s < shapes[currentFileIndex].instances.size(); s++) {
			glLineWidth(1.0f);
			
			// Draw basic shape
			glPolygonMode(GL_FRONT_AND_BACK, displayMode);
			if (!spfn_hideUnselected) {
				if (displayMode == GL_FILL) {
					if (shapes[currentFileIndex].instances[s].mask
						&& shapes[currentFileIndex].instances[s].point_num) {
						basicShapeShader->setMat4("projection", proj_matrix);
						basicShapeShader->setMat4("view", view_matrix);
						basicShapeShader->setMat4("model", shapes[currentFileIndex].instances[s].model_shape->getModelMatrix());
						basicShapeShader->setBool("selected", shapes[currentFileIndex].instances[s].selected);
						shapes[currentFileIndex].instances[s].model_shape->draw(basicShapeShader, 0);
					}
				}
				else if (displayMode == GL_LINE) {
					if (shapes[currentFileIndex].instances[s].mask
						&& shapes[currentFileIndex].instances[s].point_num) {
						wireframeShader->setMat4("projection", proj_matrix);
						wireframeShader->setMat4("view", view_matrix);
						wireframeShader->setMat4("model", shapes[currentFileIndex].instances[s].model_shape->getModelMatrix());
						wireframeShader->setBool("selected", shapes[currentFileIndex].instances[s].selected);
						shapes[currentFileIndex].instances[s].model_shape->draw(wireframeShader, 1);
					}
				}
			}
			else {
				if (displayMode == GL_FILL) {
					if (shapes[currentFileIndex].instances[s].mask
						&& shapes[currentFileIndex].instances[s].point_num
						&& s == highlight) {
						basicShapeShader->setBool("selected", shapes[currentFileIndex].instances[s].selected);
						drawInstructor(basicShapeShader, shapes[currentFileIndex].instances[s].model_shape, 0, proj_matrix, view_matrix, shapes[currentFileIndex].instances[s].model_shape->getModelMatrix());
					}
				}
				else if (displayMode == GL_LINE) {
					if (shapes[currentFileIndex].instances[s].mask
						&& shapes[currentFileIndex].instances[s].point_num
						&& s == highlight) {
						wireframeShader->setBool("selected", shapes[currentFileIndex].instances[s].selected);
						drawInstructor(wireframeShader, shapes[currentFileIndex].instances[s].model_shape, 1, proj_matrix, view_matrix, shapes[currentFileIndex].instances[s].model_shape->getModelMatrix());
					}
				}
			}
			
			
			// Highlight & Local axis
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (displayMode != 233) {
				if (shapes[currentFileIndex].instances[s].mask
					&& shapes[currentFileIndex].instances[s].selected
					&& shapes[currentFileIndex].instances[s].point_num) {
					glLineWidth(2.0f);
					wireframeShader->setMat4("projection", proj_matrix);
					wireframeShader->setMat4("view", view_matrix);
					wireframeShader->setMat4("model", shapes[currentFileIndex].instances[s].model_shape->getModelMatrix());
					wireframeShader->setBool("selected", shapes[currentFileIndex].instances[s].selected);
					shapes[currentFileIndex].instances[s].model_shape->draw(wireframeShader, 1);
				}
				if (shapes[currentFileIndex].instances[s].mask
					&& shapes[currentFileIndex].instances[s].selected
					&& shapes[currentFileIndex].instances[s].point_num
					&& enableModelAxis) {
					glLineWidth(2.0f);
					glm::mat4 local_model_matrix = glm::mat4(1.0f);
					local_model_matrix = glm::translate(local_model_matrix, shapes[currentFileIndex].instances[s].center);
					local_model_matrix = glm::scale(local_model_matrix, glm::vec3(0.2f, 0.2f, 0.2f));
					//glm::rotate(local_model_matrix, );
					wireframeShader->setMat4("projection", proj_matrix);
					wireframeShader->setMat4("view", view_matrix);
					wireframeShader->setMat4("model", local_model_matrix);
					wireframeShader->setBool("selected", TRUE);
					//drawAxis(&shapes[currentFileIndex].instances[s]);
					drawAxis(proj_matrix, view_matrix, local_model_matrix);
				}
			}
			
	


			
			// Draw points
			if (spfn_displayRaw) {
				drawInstructor(spfnPointCloudShader, shapes[currentFileIndex].raw_points, 2, proj_matrix, view_matrix, raw_point_model, 1.0f, 4.0f);
			}
			switch (spfn_displayPoints) {
			case 0: // NONE
				break;
			case 1: // SELECTED
				if (shapes[currentFileIndex].instances[s].selected 
					&& spfn_displayPred
					&& shapes[currentFileIndex].instances[s].point_num > 0) {
					drawInstructor(spfnPointCloudShader, shapes[currentFileIndex].instances[s].model_pointclouds, 2, proj_matrix, view_matrix, raw_point_model, 1.0f, 3.0f);
				}
				if (shapes[currentFileIndex].instances[s].selected 
					&& spfn_displayGT
					&& shapes[currentFileIndex].instances[s].points_gt.size() > 0) {
					drawInstructor(spfnPointCloudShader, shapes[currentFileIndex].instances[s].model_gt, 2, proj_matrix, view_matrix, raw_point_model, 1.0f, 3.0f);
				}
				// Draw predicted normal lines of raw point
				if (shapes[currentFileIndex].instances[s].selected
					&& displayNormalLines
					&& shapes[currentFileIndex].instances[s].point_num > 0) {
					drawInstructor(wireframeShader, shapes[currentFileIndex].instances[s].model_normal_line, 0, proj_matrix, view_matrix, raw_point_model, 1.0f, 3.0f);
				}
				// Draw predicted normal lines of raw point
				if (shapes[currentFileIndex].instances[s].selected
					&& displayNormalLinesGT
					&& shapes[currentFileIndex].instances[s].point_num > 0) {
					drawInstructor(wireframeShader, shapes[currentFileIndex].instances[s].model_normal_line_gt, 0, proj_matrix, view_matrix, raw_point_model, 1.0f, 3.0f);
				}
				break;
			case 2: // ALL
				if (shapes[currentFileIndex].instances[s].point_num > 0
					&& spfn_displayPred) {
					drawInstructor(spfnPointCloudShader, shapes[currentFileIndex].instances[s].model_pointclouds, 2, proj_matrix, view_matrix, raw_point_model, 1.0f, 3.0f);

				}
				if (shapes[currentFileIndex].instances[s].points_gt.size() > 0
					&& spfn_displayGT) {
					drawInstructor(spfnPointCloudShader, shapes[currentFileIndex].instances[s].model_gt, 2, proj_matrix, view_matrix, raw_point_model, 1.0f, 3.0f);
				}
				break;
			default:
				break;
			}

		}


		MyUi::startFrame();
		{
			myui.beginWindow("Menu");
			ImGui::Text("WINDOW SETTINGS:");
			ImGui::Checkbox("No move", &myui.no_move); ImGui::SameLine();
			ImGui::Checkbox("No resize", &myui.no_resize);
			ImGui::Text("OPERATE STYLE:");
			ImGui::Checkbox("All directions", &isMeshLabStyle);
			ImGui::Text("RENDERING MODE:");
			ImGui::RadioButton("Wireframe", &displayMode, GL_LINE); ImGui::SameLine();
			ImGui::RadioButton("Face", &displayMode, GL_FILL); ImGui::SameLine();
			ImGui::RadioButton("None", &displayMode, 233);
			ImGui::Text("DISPLAY AXES:");
			ImGui::Checkbox("World axis", &enableWorldAxis); ImGui::SameLine();
			ImGui::Checkbox("Model axis", &enableModelAxis);
			ImGui::Checkbox("Floor", &displayFloor);
			ImGui::Text("DISPLAY POINTS: ");
			ImGui::RadioButton("None", &spfn_displayPoints, 0); ImGui::SameLine();
			ImGui::RadioButton("Selected", &spfn_displayPoints, 1); ImGui::SameLine();
			ImGui::RadioButton("All", &spfn_displayPoints, 2);
			ImGui::Checkbox("Prediction", &spfn_displayPred); ImGui::SameLine();
			ImGui::Checkbox("Ground-truth", &spfn_displayGT);
			ImGui::Text("OTHER: ");
			ImGui::Checkbox("Display raw point clouds", &spfn_displayRaw);
			ImGui::Checkbox("Display normal liens of points", &displayNormalLines);
			ImGui::Checkbox("Display normal liens of points (GT)", &displayNormalLinesGT);
			ImGui::Checkbox("Hide unselected primitives", &spfn_hideUnselected);
			ImGui::Checkbox("Switch among all primitives", &spfn_switchBetween);
			myui.endWindow();
		}	
		{
			myui1.no_background = true;
			myui1.beginWindow("Infobar");
			ImGui::Text("WINDOW SETTINGS:");
			ImGui::Checkbox("No move", &myui1.no_move); ImGui::SameLine();
			ImGui::Checkbox("No resize", &myui1.no_resize);
			ImGui::Text("WINDOW INFO:");
			ImGui::Text("  Frame: %.1f FPS", ImGui::GetIO().Framerate);
			ImGui::Text("  Cursor: %.0f, %.0f", lastX, lastY);
			ImGui::Text("PRIMITIVES INFO:");
			ImGui::Text("  File: %s", filelist[currentFileIndex].c_str());
			ImGui::Text("  ID: %d", highlight);
			ImGui::Text("  Type: %s", primitives_name[shapes[currentFileIndex].instances[highlight].type].c_str());
			ImGui::Text("  Points: %d", shapes[currentFileIndex].instances[highlight].point_num);
			ImGui::Text("  Paras:");
			switch (shapes[currentFileIndex].instances[highlight].type) {
			case 0:
				//ImGui::Text("   - n: (%.2f, %.2f, %.2f)",
				//	shapes[currentFileIndex].instances[highlight].paras[0],
				//	shapes[currentFileIndex].instances[highlight].paras[1],
				//	shapes[currentFileIndex].instances[highlight].paras[2]);
				//ImGui::Text("   - c: %.2f",
				//	shapes[currentFileIndex].instances[highlight].paras[3]);
				break;
			case 1:
				ImGui::Text("   - n: (%.2f, %.2f, %.2f)",
					shapes[currentFileIndex].instances[highlight].paras[0],
					shapes[currentFileIndex].instances[highlight].paras[1],
					shapes[currentFileIndex].instances[highlight].paras[2]);
				ImGui::Text("   - c: %.2f",
					shapes[currentFileIndex].instances[highlight].paras[3]);
				break;
			case 2:
				ImGui::Text("   - centre: (%.2f, %.2f, %.2f)",
					shapes[currentFileIndex].instances[highlight].paras[0],
					shapes[currentFileIndex].instances[highlight].paras[1],
					shapes[currentFileIndex].instances[highlight].paras[2]);
				ImGui::Text("   - axis: (%.2f, %.2f, %.2f)",
					shapes[currentFileIndex].instances[highlight].paras[3],
					shapes[currentFileIndex].instances[highlight].paras[4],
					shapes[currentFileIndex].instances[highlight].paras[5]);
				ImGui::Text("   - radius: %.2f",
					shapes[currentFileIndex].instances[highlight].paras[6]);
				break;
			case 3:
				ImGui::Text("   - apex: (%.2f, %.2f, %.2f)",
					shapes[currentFileIndex].instances[highlight].paras[0],
					shapes[currentFileIndex].instances[highlight].paras[1],
					shapes[currentFileIndex].instances[highlight].paras[2]);
				ImGui::Text("   - axis: (%.2f, %.2f, %.2f)",
					shapes[currentFileIndex].instances[highlight].paras[3],
					shapes[currentFileIndex].instances[highlight].paras[4],
					shapes[currentFileIndex].instances[highlight].paras[5]);
				ImGui::Text("   - half angle: %.2f",
					shapes[currentFileIndex].instances[highlight].paras[6]);
				break;
			default:
				break;
			}
			ImGui::Checkbox("PARAMETERS GT", &spfn_displayGTInfo);
			if (spfn_displayGTInfo) {
				for (unsigned int g = 0; g < shapes[currentFileIndex].gt_info.size(); g++) {
					SPFN_GTInfo temp = shapes[currentFileIndex].gt_info[g];
					ImGui::Text("  %2d: %8s (%.2f, %.2f, %.2f)", temp.id, temp.type.c_str(), temp.para1, temp.para2, temp.para3);
				}
			}

			
			myui1.endWindow();
		}
		MyUi::rendering();


		glfwSwapBuffers(window);
	}

	// Cleanup
	MyUi::cleanUp();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
