#ifndef MODEL_H
#define MODEL_H

// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>


// opengl & glfw
#include <GL/gl3w.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <ctime>

using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model
{
public:
	/*  模型数据  */
	vector<Texture> textures_loaded; // 保存所有已加载的纹理
	vector<Mesh> meshes; // 保存该模型下所有网格
	string directory; // 模型所在目录
	bool gammaCorrection; // 伽马矫正

	glm::mat4 model;

	/*  函数  */
	// 构造函数，从文件加载模型
	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
		model = glm::mat4(1.0f);
		loadModel(path); // 从文件加载模型
	}

	// 绘制模型，即绘制该模型下的所有网格
	void Draw(Shader *shader, int drawType = 0)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader, drawType);
	}

	// 旋转（轴, 角）
	void inline rotate(float angle, glm::vec3 axis) {
		model = glm::rotate(model, angle, axis);
	}
	// 旋转（轴 -> 轴）
	void inline rotate(glm::vec3 src, glm::vec3 dst) {
		src = glm::normalize(src);
		dst = glm::normalize(dst);
		float dot = src.x*dst.x + src.y*dst.y + src.z*dst.z;
		float angle = std::acosf(dot);
		glm::vec3 axis = glm::normalize(glm::cross(src, dst));
		rotate(angle, axis);
	}
	// 位移
	void translate(glm::vec3 dxyz) {
		model = glm::translate(model, dxyz);
	}
	// 缩放
	void scale(glm::vec3 dxyz) {
		model = glm::scale(model, dxyz);
	}

private:
	/* 函数 */
	// 模型文件 -> Assimp -> Model/Mesh
	void loadModel(string const &path)
	{
		// 从文件中读取模型
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_FlipUVs | aiProcess_GenNormals);

		// 错误检查
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // 如果不等于0
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// 保存模型所在目录
		directory = path.substr(0, path.find_last_of('/'));

		// 从Assimp根节点开始，递归处理所有节点
		processNode(scene->mRootNode, scene);
	}

	// 处理单个aiNode下的所有aiMesh
	void processNode(aiNode *node, const aiScene *scene)
	{
		// 处理当前aiNode下的所有aiMesh
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// aiNode只保存了索引，所有数据都保存在aiScene
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; // 找到aiMesh在aiScene中的具体位置
			meshes.push_back(processMesh(mesh, scene));      // 将其转为自定义Mesh类，存入自定义Model类中
		}
		// 递归调用本函数处理当前aiNode的子节点
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	// 处理单个aiMesh
	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		// 构造Mesh类对象所需的数据
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		// 遍历当前mesh所有顶点
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex(glm::vec3(0.0f, 0.0f, 0.0f));
			glm::vec3 vector; // 把Assimp的顶点转存为glm::vec3
			glm::vec2 vec;

			// 位置
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;

			// 颜色
			if (mesh->HasVertexColors(0))
			{
				vector.x = mesh->mColors[0][i].r;
				vector.y = mesh->mColors[0][i].g;
				vector.z = mesh->mColors[0][i].b;
			}
			else
			{
				vertex.color = glm::vec3(0.5f, 0.5f, 0.5f);
			}

			// 法向量
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.normal = vector;
			}

			// 纹理坐标
			if (mesh->HasTextureCoords(0)) // 如果存在纹理
			{
				// 一个顶点最多可以有8个纹理坐标，我们只取第一个
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.texCoords = vec;
			}

			vertices.push_back(vertex);
		}

		// 遍历当前mesh所有三角形面
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// 遍历三角形面的所有顶点索引
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// 处理材质
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// 材质纹理贴图命名方式：
		// diffuse:  texture_diffuseN
		// specular: texture_specularN
		// normal:   texture_normalN

		// 1. diffuse maps
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		// 返回一个自定义Mesh类对象
		return Mesh(vertices, indices, textures);
	}

	// 加载某一类型材质贴图（如果还没有加载）
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		// 遍历所有该类型材质贴图
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// 检查材质是否已加载，如果已加载则跳过
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // 当前路径材质已加载，跳过
					break;
				}
			}
			if (!skip)
			{   // 如果当前路径材质还没加载过，那么加载它
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // 加载的材质保存在Model中，保证只加载一次，不重复加载
				cout << "已加载贴图: " << texture.id << " 路径: " << texture.path << " 类型: " << texture.type << endl;
			}
		}
		return textures;
	}
};


// 从文件中读取纹理贴图
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
	// 拼接文件路径
	string filename = string(path);
	filename = directory + '/' + filename;

	// 创建纹理对象
	unsigned int textureID;
	glGenTextures(1, &textureID);

	// 开始从文件读取（使用stb_image库）
	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		// 判断颜色通道数
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		// 绑定纹理对象
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// 纹理环绕和过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 释放资源
		stbi_image_free(data);
	}
	else
	{   // 读取失败
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID; // 返回纹理id
}
#endif  // MODEL_H