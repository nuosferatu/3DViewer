#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

out vec3 outColor;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	//outColor = vec3(1.0, 0.0, 0.0);
	outColor = aColor;
	
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}