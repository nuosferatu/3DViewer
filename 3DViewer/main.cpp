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
#include "resource.h"

// std
#include <Windows.h>
#include <iostream>
#include <vector>



/* һЩȫ�ֱ��� */
const static char* glsl_version = "#version 130";

// ���ڲ���
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
const static char* WINDOW_NAME = "3D VIEWER";

// ���������
Camera camera;
float lastX, lastY;
bool firstMouse = true;

// ����ȫ�ֱ���
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

// һЩ״̬��
int displayMode = GL_FILL; // ��Ⱦģʽ
bool enableMouseMovement = FALSE;
bool enableWorldAxis = TRUE;
bool enableModelAxis = FALSE;
int current = 0;
bool enableMouseMiddleMovement = FALSE;

vector<Primitives*> shapes;
unsigned int highlight = 0;

enum interactionStyle {
	MESHLAB_STYLE,
	PLATFORM_STYLE,
	FPS_STYLE
};
interactionStyle OperationMode = PLATFORM_STYLE;


Shader *wireframeShader;
Mesh *axis_mesh;
Mesh *floor_mesh;


// ���ڳߴ�仯�ص����� -------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// ������Ⱦѭ���а�����⺯�� -------------------------------------------------
void processInput(GLFWwindow *window) {
	// ������ƶ�
	// MeshLab(0) or FPS(1)
	switch (OperationMode)
	{
	case 0:
		//if (glfwGetKey(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		//{
		//	//if (enableMouseMovement == FALSE)
		//	//{
		//		//enableMouseMovement = TRUE;
		//		cout << "����������" << endl;
		//	//}
		//}
		//if (glfwGetKey(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		//{
		//	//if (enableMouseMovement == TRUE)
		//	//{
		//		//enableMouseMovement = FALSE;
		//		cout << "�������ͷ�" << endl;
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

// ����ƶ����ص����� -------------------------------------------------------
void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos) {
	// ��ʼ�����λ��
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	switch (OperationMode)
	{
	case MESHLAB_STYLE:
		if (enableMouseMovement) {
			// ��������ƶ���
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;
			// ����������仯
			camera.MouseMovementMeshLabStyle(xoffset, yoffset);
		}
		if (enableMouseMiddleMovement) {
			// ��������ƶ���
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;
			// ����������仯
			camera.translate(xoffset, yoffset);
		}
		break;
	case PLATFORM_STYLE:
		if (enableMouseMovement) {
			// ��������ƶ���
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;
			// ����������仯
			camera.MouseMovementPlatformStyle(xoffset, yoffset);
		}
		if (enableMouseMiddleMovement) {
			// ��������ƶ���
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;
			// ����������仯
			camera.translate(xoffset, yoffset);
		}
		break;
	case FPS_STYLE:
		if (enableMouseMovement) {
			// ��������ƶ���
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;
			// ����������仯
			//camera.ProcessReleasedMouseMovement(xoffset, yoffset);
		}
		break;
	default:
		break;
	}

	lastX = xpos;
	lastY = ypos;
}

// �����ּ��ص����� -------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	// ����������仯
	camera.ProcessMouseScroll(yoffset);
}

// ��갴�����ص����� -------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) switch (button) // ����
	{
	case GLFW_MOUSE_BUTTON_LEFT: // ������
		//cout << "����������" << endl;
		enableMouseMovement = TRUE;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE: // ����м�
		enableMouseMiddleMovement = TRUE;
		break;
	default:
		break;
	}
	if (action == GLFW_RELEASE) switch (button) // �ͷ�
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		//cout << "�������ͷ�" << endl;
		enableMouseMovement = FALSE;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		enableMouseMiddleMovement = FALSE;
		break;
	default:
		break;
	}
}

// ���̰������ص����� -------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) switch (key)
	{
	case GLFW_KEY_ESCAPE: // 'ESC': �رմ���
		glfwSetWindowShouldClose(window, true);
		break;

	case GLFW_KEY_M:      // 'M': �л���Ⱦģʽ���߿���䣩
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

	case GLFW_KEY_O: // 'O': �л�����ģʽ
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

	case GLFW_KEY_R: // 'R': �������
		camera.resetCamera();
		cout << "���������" << endl;
		break;

	case GLFW_KEY_TAB: // 'TAB': �л�����
		highlight++;
		current = highlight % shapes.size();
		for (unsigned int s = 0; s < shapes.size(); s++) {
			shapes[s]->selected = FALSE;
		}
		shapes[current]->selected = TRUE;
		break;

	case GLFW_KEY_X: // 'X': �л���������ʾ���
		enableWorldAxis = !enableWorldAxis;
		break;
	}
}

// ����������ͼ ---------------------------------------------------------------
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

// �¼������ص����� -----------------------------------------------------------
void listenEvents(GLFWwindow* window) {
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // �������
}

// ����GLFW���� ---------------------------------------------------------------
GLFWwindow* initGLFW() {
	glfwInit();  // ��ʼ��GLFW

	//opengl & glsl version

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

	// ����GLFW����
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_NAME, NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	// �¼�����
	listenEvents(window);

	return window;
}

void initAxis()
{
	// ������
	const float axis_points[] = {
		// λ��            // ��ɫ
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
	Vertex vertex;
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

//------------------------------  ������  ------------------------------

int main() {
	// ����һ��GLFW���ڶ���
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = initGLFW();


	glfwSwapInterval(1); // Enable vsync
	gl3wInit();

	// ������Ȳ���
	glLineWidth(1.0f);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);


	// ������ɫ��
	//Shader pointCloudsShader("point_clouds.vert", "point_clouds.frag");
	Shader *modelShader = new Shader("model_lighting.vert", "model_lighting.frag");
	Shader *floorShader = new Shader("floor.vert", "floor.frag");
	Shader *basicShapeShader = new Shader("basic_shape.vert", "basic_shape.frag");
	wireframeShader = new Shader("wireframe.vert", "wireframe.frag");


	// ����ģ��
	Model floor("floor/c.obj");
	Cylinder cylinder;
	cylinder.rotate(1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	cylinder.scale(glm::vec3(2.0f, 2.0f, 2.0f));
	cylinder.translate(glm::vec3(0.0f, 2.0f, 0.0f));
	cylinder.selected = TRUE;
	shapes.push_back(&cylinder);
	Cone cone;
	cone.rotate(1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	cone.scale(glm::vec3(2.0f, 2.0f, 2.0f));
	cone.translate(glm::vec3(0.0f, 2.0f, 0.0f));
	shapes.push_back(&cone);
	Plane plane;
	plane.rotate(1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	plane.scale(glm::vec3(2.0f, 2.0f, 2.0f));
	plane.translate(glm::vec3(0.0f, -1.0f, 0.0f));
	//shapes.push_back(&plane);
	Sphere sphere;
	sphere.rotate(1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	sphere.scale(glm::vec3(2.0f, 2.0f, 2.0f));
	sphere.translate(glm::vec3(0.0f, -2.0f, 0.0f));
	shapes.push_back(&sphere);


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);


	
	initAxis();




	//cout << "Operation Mode: " << "PLATFORM_STYLE" << endl;
	//cout << "Display Mode: " << "FILL" << endl;

	// ��Ⱦѭ�� --------------------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		// ����ÿ֡ʱ��
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// ����
		processInput(window);  // ���ڰ������

		// �ӿ�
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		// �����ɫ����
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ��ʾģʽ
		glPolygonMode(GL_FRONT_AND_BACK, displayMode);

		glfwPollEvents();

		/* ��Ⱦ���� ------------------------------------*/

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));

		// �ذ�
		floorShader->setMat4("projection", projection);
		floorShader->setMat4("view", view);
		model = glm::translate(model, glm::vec3(0.0f, -0.002f, 0.0f));
		floorShader->setMat4("model", model);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(1.0f);
		floor.Draw(floorShader, 1);

		// ��������ϵ������
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
		basicShapeShader->setVec3("light.specular", 0.0f, 0.0f, 0.0f);
		//basicShapeShader->setFloat("light.constant", 1.0f);
		//basicShapeShader.setFloat("light.linear", 0.09f);
		//basicShapeShader.setFloat("light.quadratic", 0.032f);
		basicShapeShader->setVec3("viewPos", camera.Position);
		basicShapeShader->setVec3("material.ambient", 0.5f, 0.5f, 0.5f);
		basicShapeShader->setVec3("material.diffuse", 0.5f, 0.5f, 0.5f);
		basicShapeShader->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		basicShapeShader->setFloat("material.shininess", 64.0f);


		for (unsigned int s = 0; s < shapes.size(); s++) {
			glLineWidth(1.0f);
			
			glPolygonMode(GL_FRONT_AND_BACK, displayMode);
			if (displayMode == GL_FILL) {
				basicShapeShader->setMat4("model", shapes[s]->model_shape->model);
				basicShapeShader->setBool("selected", shapes[s]->selected);
				shapes[s]->model_shape->Draw(basicShapeShader, 0);
			}
			else if (displayMode == GL_LINE) {
				wireframeShader->setMat4("model", shapes[s]->model_shape->model);
				wireframeShader->setBool("selected", shapes[s]->selected);
				shapes[s]->model_shape->Draw(wireframeShader, 1);
			}
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (shapes[s]->selected && enableModelAxis) {
				glLineWidth(2.0f);
				drawModelAxis(shapes[s]);
			}
		}


		/* ��Ⱦ GUI ------------------------------------*/
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			//static int counter = 0;

			ImGui::Begin("Tools");                          // Create a window called "Hello, world!" and append into it.

			if (ImGui::Button("Enable/Disable World Axis"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				enableWorldAxis = !enableWorldAxis;
			if (ImGui::Button("Enable/Disable Model Axis"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				enableModelAxis = !enableModelAxis;
			if (ImGui::Button("Exchange Display Mode"))
				displayMode == GL_LINE ? displayMode = GL_FILL : displayMode = GL_LINE;
			//ImGui::Text("counter = %d", counter);

			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



		// ��鲢�����¼�����������
		glfwSwapBuffers(window);  // ������ɫ���壨˫���壩������һ����Ⱦ
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();  // �ͷ���Դ

	return 0;
}
