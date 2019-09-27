#ifndef MESH_H
#define MESH_H

// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>


// opengl & glfw
#include <GL/gl3w.h>


//#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
//#include <GL/gl3w.h>    // Initialize with gl3wInit()
//#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
//#include <GL/glew.h>    // Initialize with glewInit()
//#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
//#include <glad/glad.h>  // Initialize with gladLoadGL()
//#else
//#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
//#endif
//
//// Include glfw3.h after our OpenGL definitions
//#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;


// 顶点
struct Vertex {
	glm::vec3 position;   // 位置
	glm::vec3 color;      // 颜色
	glm::vec3 normal;     // 法向量
	glm::vec2 texCoords;  // 纹理坐标
};

// 纹理
struct Texture {
	unsigned int id;  // 纹理id
	string type;      // 纹理类型，diffuse（漫反射）、specular（镜面反射）等
	string path;      // 纹理文件路径
};

class Mesh {
public:
	/*  网格数据  */
	vector<Vertex> m_vertices;       // 顶点列表
	vector<unsigned int> m_indices;  // 索引列表
	vector<Texture> m_textures;      // 纹理列表
	unsigned int m_VAO;              // 顶点数组对象

	glm::mat4 model;

	/*  函数  */
	// 构造函数
	Mesh(vector<Vertex> _vertices, vector<unsigned int> _indices, vector<Texture> _textures)
	{
		model = glm::mat4(1.0f);
		this->m_vertices = _vertices;
		this->m_indices = _indices;
		this->m_textures = _textures;
		// 初始化缓冲对象，设置属性指针
		setupMesh();
	}

	// 旋转
	void rotate(float angle, glm::vec3 axis) {
		model = glm::rotate(model, angle, axis);
	}
	// 位移
	void translate(glm::vec3 dxyz) {
		model = glm::translate(model, dxyz);
	}
	// 缩放
	void scale(glm::vec3 dxyz) {
		model = glm::scale(model, dxyz);
	}

	// 渲染网格
	void Draw(Shader *shader, int drawType)
	{
		shader->use();

		// 绑定纹理
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < m_textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // 绑定纹理前，激活对应的纹理单元

			/*  拼接纹理id，比如：diffuse_texture + N = diffuse_textureN  */
			// 把序号转成字符串
			string number;
			string name = m_textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++);
			else if (name == "texture_height")
				number = std::to_string(heightNr++);
			// 把纹理单元传入着色器中对应的采样器
			glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), i);
			//cout << "传入采样器: " << name << number << endl;
			// 绑定纹理单元
			glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
		}

		// 绘制网格

		glBindVertexArray(m_VAO);
		if (drawType == 0)
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
		else if (drawType == 1)
			glDrawElements(GL_LINES, m_indices.size(), GL_UNSIGNED_INT, 0);
		//cout << m_indices.size() << endl;
		glBindVertexArray(0);

		// 绘制完以后，切换回默认纹理
		glActiveTexture(GL_TEXTURE0);
	}


private:
	/*  渲染数据  */
	unsigned int m_VBO;  // 顶点缓冲对象
	unsigned int m_EBO;  // 索引缓冲对象

	/*  函数    */
	// 初始化各种缓冲对象
	void setupMesh()
	{
		// 生成缓冲对象
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);

		glBindVertexArray(m_VAO);
		// 加载数据到VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

		// 加载数据到EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

		// 设置顶点属性指针
		// 顶点位置
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		// 顶点颜色
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		// 顶点法向量
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		// 顶点纹理坐标
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		// 切换回默认VAO
		glBindVertexArray(0);
	}
};
#endif  // MESH_H