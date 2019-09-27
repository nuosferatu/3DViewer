#ifndef SHADER_H
#define SHADER_H

// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>


// opengl & glfw
#include <GL/gl3w.h>

//#include "imgui.h"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_opengl3.h"
//
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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID; // 着色器id
	// 构建着色器（程序）
	// ------------------------------------------------------------------------
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
	{
		/*  1. 从文件中读取着色器代码  */
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		// 确保ifstream对象可以抛出异常：
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			/*  从文件中读取GLSL着色器代码  */
			// 顶点着色器
			vShaderFile.open(vertexPath);
			std::stringstream vShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			vShaderFile.close();
			vertexCode = vShaderStream.str();

			// 片段着色器
			fShaderFile.open(fragmentPath);
			std::stringstream fShaderStream;
			fShaderStream << fShaderFile.rdbuf();
			fShaderFile.close();
			fragmentCode = fShaderStream.str();

			// 几何着色器
			if (geometryPath != nullptr)
			{
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{   // 输出错误信息
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		/*  2. 编译着色器  */
		// 顶点着色器
		unsigned int vertex;
		const char* vShaderCode = vertexCode.c_str();
		vertex = glCreateShader(GL_VERTEX_SHADER);     // 创建着色器
		glShaderSource(vertex, 1, &vShaderCode, NULL); // 绑定着色器源码
		glCompileShader(vertex);                       // 编译着色器
		checkCompileErrors(vertex, "VERTEX");          // 检查着色器编译错误

		// 片段着色器
		unsigned int fragment;
		const char * fShaderCode = fragmentCode.c_str();
		fragment = glCreateShader(GL_FRAGMENT_SHADER);   // 创建着色器
		glShaderSource(fragment, 1, &fShaderCode, NULL); // 绑定着色器源码
		glCompileShader(fragment);                       // 编译着色器
		checkCompileErrors(fragment, "FRAGMENT");        // 检查着色器编译错误

		// 几何着色器
		unsigned int geometry;
		if (geometryPath != nullptr)
		{
			const char * gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);   // 创建着色器
			glShaderSource(geometry, 1, &gShaderCode, NULL); // 绑定着色器源码
			glCompileShader(geometry);                       // 编译着色器
			checkCompileErrors(geometry, "GEOMETRY");        // 检查着色器编译错误
		}

		// 着色器程序
		ID = glCreateProgram();            // 创建着色器程序
		glAttachShader(ID, vertex);        // 绑定顶点着色器
		glAttachShader(ID, fragment);      // 绑定片段着色器
		if (geometryPath != nullptr)
			glAttachShader(ID, geometry);  // 绑定几何着色器
		glLinkProgram(ID);                 // 链接着色器程序
		checkCompileErrors(ID, "PROGRAM"); // 检查着色器程序编译错误

		// 着色器程序链接成功后，释放着色器资源
		glDeleteShader(vertex);       // 释放顶点着色器
		glDeleteShader(fragment);     // 释放片段着色器
		if (geometryPath != nullptr)
			glDeleteShader(geometry); // 释放几何着色器
	}

	// 激活着色器
	// ------------------------------------------------------------------------
	void use()
	{
		glUseProgram(ID);
	}

	// 工具函数：操作uniform全局变量
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool value)
	{
		use();
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int value)
	{
		use();
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string &name, float value)
	{
		use();
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const std::string &name, const glm::vec2 &value)
	{
		use();
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string &name, float x, float y)
	{
		use();
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, const glm::vec3 &value)
	{
		use();
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string &name, float x, float y, float z)
	{
		use();
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, const glm::vec4 &value)
	{
		use();
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string &name, float x, float y, float z, float w)
	{
		use();
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string &name, const glm::mat2 &mat)
	{
		use();
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const std::string &name, const glm::mat3 &mat)
	{
		use();
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(const std::string &name, const glm::mat4 &mat)
	{
		use();
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

private:
	// 工具函数：着色器错误检查
	// ------------------------------------------------------------------------
	void checkCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{   // 着色器程序检查
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				// 输出错误信息
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
					<< infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{   // 着色器检查
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				// 输出错误信息
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
					<< infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};
#endif  // SHADER_H