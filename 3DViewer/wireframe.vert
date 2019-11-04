#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

out vec3 outColor;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool selected = false;

void main()
{
    if (selected) {
	    outColor = aColor + vec3(0.5, 0.5, 0.5);
		if (outColor.r > 1.0) outColor.r = 1.0;
		if (outColor.g > 1.0) outColor.g = 1.0;
		if (outColor.b > 1.0) outColor.b = 1.0;
	}
	else {
        outColor = aColor;
	}
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}