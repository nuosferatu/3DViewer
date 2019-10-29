// dear imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

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
#include "spfn_shpes.h"
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
const static char* WINDOW_NAME = "3D VIEWER";

// 摄像机参数
Camera camera;
float lastX, lastY;
bool firstMouse = true;

// 其它全局变量
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// 一些状态量
int displayMode = GL_FILL; // 渲染模式
bool enableMouseMovement = FALSE;
bool enableWorldAxis = TRUE;
bool enableModelAxis = FALSE;
int current = 0;
unsigned int highlight = 0;
bool enableMouseMiddleMovement = FALSE;
int spfn_displayPoints = 0;

vector<Primitives> shapes;
int currentFileIndex = -1;
vector<string> filelist;
vector<SPFN_Shape> spfn_shapes;
vector<Vertex> point_list;


enum interactionStyle {
	MESHLAB_STYLE,
	PLATFORM_STYLE,
	FPS_STYLE
};
interactionStyle OperationMode = PLATFORM_STYLE;


Shader *wireframeShader;
Mesh *axis_mesh;
Mesh *floor_mesh;


// 窗口尺寸变化回调函数 -------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// 窗口渲染循环中按键检测函数 -------------------------------------------------
void processInput(GLFWwindow *window) {
	// 摄像机移动
	// MeshLab(0) or FPS(1)
	switch (OperationMode)
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

	switch (OperationMode)
	{
	case MESHLAB_STYLE:
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
	case PLATFORM_STYLE:
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
	case FPS_STYLE:
		if (enableMouseMovement) {
			// 计算鼠标移动量
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;
			// 摄像机发生变化
			//camera.ProcessReleasedMouseMovement(xoffset, yoffset);
		}
		break;
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
		switch ((OperationMode % 3))
		{
		case MESHLAB_STYLE:
			OperationMode = PLATFORM_STYLE;
			cout << "Operation Mode: " << "PLATFORM_STYLE" << endl;
			break;
		case PLATFORM_STYLE:
			OperationMode = FPS_STYLE;
			cout << "Operation Mode: " << "FPS_STYLE" << endl;
			break;
		case FPS_STYLE:
			OperationMode = MESHLAB_STYLE;
			cout << "Operation Mode: " << "MESHLAB_STYLE" << endl;
			break;
		}
		break;

	case GLFW_KEY_R: // 'R': 重置相机
		camera.resetCamera();
		cout << "相机已重置" << endl;
		break;

	case GLFW_KEY_TAB: // 'TAB': 切换高亮
		highlight++;
		highlight = highlight % shapes.size();
		for (unsigned int s = 0; s < shapes.size(); s++) {
			shapes[s].selected = FALSE;
		}
		shapes[highlight].selected = TRUE;
		break;

	case GLFW_KEY_X: // 'X': 切换坐标轴显示与否
		enableWorldAxis = !enableWorldAxis;
		break;
	case GLFW_KEY_RIGHT: // 右方向键，下一个文件
		currentFileIndex++;
		currentFileIndex %= filelist.size();
		
		vector<SPFN_Shape> new_spfn_shapes;
		vector<Vertex> new_point_list;
		vector<Primitives> new_shapes;
		importPointsFromFile("E:\\instances_paras\\8096_pred\\" + filelist[currentFileIndex] + ".txt", new_point_list, glm::vec3(1.0f, 0.0f, 0.0f)); // position (x, y, z) of all 8096 points
		importShapeParasFromFile("E:\\instances_paras\\ins_type_paras_points\\" + filelist[currentFileIndex] + "__ins_type_paras_points.txt", new_spfn_shapes, new_point_list); // 参数形式的形状列表
		ConstructPrimitives(new_spfn_shapes, new_shapes);

		vector<SPFN_Shape>().swap(spfn_shapes);
		spfn_shapes.swap(new_spfn_shapes);
		vector<Vertex>().swap(point_list);
		point_list.swap(new_point_list);
		vector<Primitives>().swap(shapes);
		shapes.swap(new_shapes);
		
		shapes[0].selected = true;
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
		vertex.texCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	vector<unsigned int> indices;
	for (unsigned int i = 0; i < sizeof(axis_indices) / sizeof(axis_indices[0]); i++) {
		indices.push_back(axis_indices[i]);
	}
	vector<Texture> textures;
	axis_mesh = new Mesh(vertices, indices, textures);
}

void drawWorldAxis()
{
	axis_mesh->model = glm::mat4(1.0f);
	axis_mesh->scale(glm::vec3(1.0f, 1.0f, 1.0f));
	wireframeShader->setMat4("model", axis_mesh->model);
	wireframeShader->setBool("selected", TRUE);
	axis_mesh->Draw(wireframeShader, 0);
}

void drawModelAxis(Primitives *shape)
{
	axis_mesh->model = shape->model_shape->model;
	axis_mesh->scale(glm::vec3(0.15f, 0.15f, 0.15f));
	wireframeShader->setMat4("model", axis_mesh->model);
	wireframeShader->setBool("selected", shape->selected);
	axis_mesh->Draw(wireframeShader, 0);
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
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);


	// 创建着色器
	//Shader pointCloudsShader("point_clouds.vert", "point_clouds.frag");
	Shader *modelShader = new Shader("model_lighting.vert", "model_lighting.frag");
	Shader *floorShader = new Shader("floor.vert", "floor.frag");
	Shader *basicShapeShader = new Shader("basic_shape.vert", "basic_shape.frag");
	Shader *spfnPointCloudShader = new Shader("spfn_point_clouds.vert", "spfn_point_clouds.frag");
	wireframeShader = new Shader("wireframe.vert", "wireframe.frag");


	// 加载模型
	Model floor("floor/c.obj");

	ReadFilelist(".filename_list.txt", filelist);
	currentFileIndex = 0;

	
	importPointsFromFile("E:\\instances_paras\\8096_pred\\" + filelist[currentFileIndex] + ".txt", point_list, glm::vec3(1.0f, 0.0f, 0.0f)); // position (x, y, z) of all 8096 points
	importShapeParasFromFile("E:\\instances_paras\\ins_type_paras_points\\" + filelist[currentFileIndex] + "__ins_type_paras_points.txt", spfn_shapes, point_list); // 参数形式的形状列表
	int bias = 0;
	ConstructPrimitives(spfn_shapes, shapes, bias);
	shapes[0].selected = true;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	

	initAxis();


	// 渲染循环 --------------------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		// 计算每帧时长
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 输入
		processInput(window);  // 窗口按键检测

		// 视口
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		// 清除颜色缓存
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 显示模式
		glPolygonMode(GL_FRONT_AND_BACK, displayMode);

		glfwPollEvents();

		/* 渲染场景 ------------------------------------*/

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));

		// 地板
		floorShader->setMat4("projection", projection);
		floorShader->setMat4("view", view);
		model = glm::translate(model, glm::vec3(0.0f, -0.002f, 0.0f));
		floorShader->setMat4("model", model);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(1.0f);
		floor.Draw(floorShader, 1);

		// 世界坐标系坐标轴
		model = glm::translate(model, glm::vec3(0.0f, 0.002f, 0.0f));
		wireframeShader->setMat4("projection", projection);
		wireframeShader->setMat4("view", view);
		wireframeShader->setMat4("model", model);
		wireframeShader->setBool("selected", TRUE);
		if (enableWorldAxis) {
			glLineWidth(2.0f);
			drawWorldAxis();
		}

		// Model
		basicShapeShader->setMat4("projection", projection);
		basicShapeShader->setMat4("view", view);
		//basicShapeShader->setMat4("model", model);
		basicShapeShader->setVec3("light.position", 0.0f, 30.0f, 4.0f);
		basicShapeShader->setVec3("light.ambient", 0.24f, 0.24f, 0.24f);
		basicShapeShader->setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
		basicShapeShader->setVec3("light.specular", 0.8f, 0.8f, 0.8f);
		//basicShapeShader->setFloat("light.constant", 1.0f);
		//basicShapeShader.setFloat("light.linear", 0.09f);
		//basicShapeShader.setFloat("light.quadratic", 0.032f);
		basicShapeShader->setVec3("viewPos", camera.Position);
		basicShapeShader->setVec3("material.ambient", 0.4f, 0.4f, 0.4f);
		basicShapeShader->setVec3("material.diffuse", 0.3f, 0.3f, 0.3f);
		basicShapeShader->setVec3("material.specular", 0.0f, 0.0f, 0.0f);
		basicShapeShader->setFloat("material.shininess", 64.0f);
		basicShapeShader->setVec3("material2.ambient", 1.0f, 1.0f, 1.0f);
		basicShapeShader->setVec3("material2.diffuse", 1.0f, 1.0f, 1.0f);
		basicShapeShader->setVec3("material2.specular", 0.0f, 0.0f, 0.0f);
		basicShapeShader->setFloat("material2.shininess", 64.0f);


		for (unsigned int s = 0; s < shapes.size(); s++) {
			glLineWidth(1.0f);
			
			// Draw basic shape
			glPolygonMode(GL_FRONT_AND_BACK, displayMode);
			if (displayMode == GL_FILL) {
				basicShapeShader->setMat4("model", shapes[s].model_shape->model);
				basicShapeShader->setBool("selected", shapes[s].selected);
				shapes[s].model_shape->Draw(basicShapeShader, 0);
			}
			else if (displayMode == GL_LINE) {
				wireframeShader->setMat4("model", shapes[s].model_shape->model);
				wireframeShader->setBool("selected", shapes[s].selected);
				shapes[s].model_shape->Draw(wireframeShader, 1);
			}
			
			// Highlight & Local axis
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (shapes[s].selected) {
				glLineWidth(1.0f);
				wireframeShader->setMat4("model", shapes[s].model_shape->model);
				wireframeShader->setBool("selected", shapes[s].selected);
				shapes[s].model_shape->Draw(wireframeShader, 1);
			}
			if (shapes[s].selected && enableModelAxis) {
				glLineWidth(2.0f);
				drawModelAxis(&shapes[s]);
			}

			// Draw points
			switch (spfn_displayPoints) {
			case 0: // NONE
				break;
			case 1: // SELECTED
				if (shapes[s].selected) {
					glPointSize(2);
					spfnPointCloudShader->setMat4("projection", projection);
					spfnPointCloudShader->setMat4("view", view);
					spfnPointCloudShader->setMat4("model", glm::mat4(1.0f));
					spfn_shapes[s].sub_point_clouds->Draw(spfnPointCloudShader, 2);
				}
				break;
			case 2: // ALL
				glPointSize(2);
				spfnPointCloudShader->setMat4("projection", projection);
				spfnPointCloudShader->setMat4("view", view);
				spfnPointCloudShader->setMat4("model", glm::mat4(1.0f));
				spfn_shapes[s].sub_point_clouds->Draw(spfnPointCloudShader, 2);
				break;
			default:
				break;
			}
		}

		

		
		/* 渲染 GUI ------------------------------------*/
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		// Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			//static int counter = 0;

			ImGuiWindowFlags window_flags = 0;
			static bool no_move = true;
			static bool no_resize = true;
			static bool no_background = false;
			static bool no_menu = true;
			static bool no_titlebar = true;

			if (no_move)         window_flags |= ImGuiWindowFlags_NoMove;
			if (no_resize)       window_flags |= ImGuiWindowFlags_NoResize;
			if (no_background)   window_flags |= ImGuiWindowFlags_NoBackground;
			if (!no_menu)        window_flags |= ImGuiWindowFlags_MenuBar;
			if (no_titlebar)     window_flags |= ImGuiWindowFlags_NoTitleBar;

			ImGui::Begin("Menu", NULL, window_flags);                          // Create a window called "Hello, world!" and append into it.

			ImGui::Checkbox("Display World Axis", &enableWorldAxis);
			ImGui::Checkbox("Display Model Axis", &enableModelAxis);
			//if (ImGui::Button("Enable/Disable World Axis"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			//	enableWorldAxis = !enableWorldAxis;
			//if (ImGui::Button("Enable/Disable Model Axis"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			//	enableModelAxis = !enableModelAxis;

			
			
			//ImGui::Checkbox("No move", &no_move);
			
			ImGui::Text("Rendering Mode:");
			ImGui::RadioButton("Wireframe", &displayMode, GL_LINE); ImGui::SameLine();
			ImGui::RadioButton("Face", &displayMode, GL_FILL);
			//if (ImGui::Button("Exchange Display Mode"))
			//	displayMode == GL_LINE ? displayMode = GL_FILL : displayMode = GL_LINE;
			//ImGui::Text("counter = %d", counter);
			
			ImGui::Text("Display points:");
			ImGui::RadioButton("None", &spfn_displayPoints, 0); ImGui::SameLine();
			ImGui::RadioButton("Selected", &spfn_displayPoints, 1); ImGui::SameLine();
			ImGui::RadioButton("All", &spfn_displayPoints, 2);

			ImGui::End();

			ImGuiWindowFlags frame_bar_flags =
				  ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoBackground
				| ImGuiWindowFlags_NoTitleBar;

			ImGui::Begin("Info Bar", NULL, frame_bar_flags);
			ImGui::Text("Frame: %.1f FPS", ImGui::GetIO().Framerate);
			ImGui::Text("Points selected: %d", spfn_shapes[highlight].num);
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



		// 检查并调用事件，交换缓冲
		glfwSwapBuffers(window);  // 交换颜色缓冲（双缓冲），迭代一次渲染
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();  // 释放资源

	return 0;
}
