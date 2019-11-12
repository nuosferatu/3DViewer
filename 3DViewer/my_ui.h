#ifndef MYUI_H
#define MYUI_H

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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "primitive.h"
#include "cone.h"
#include "cylinder.h"
#include "sphere.h"
#include "plane.h"
#include <Windows.h>

class MyUi {
public:
	// Window flags
	ImGuiWindowFlags window_flags;
	bool no_move;
	bool no_resize;
	bool no_background;
	bool no_menu;
	bool no_titlebar;

	// Window style
	enum UiColor {
		Light,
		Dark
	};
	UiColor window_color;

public:
	MyUi() : window_color(Dark) {
		// Setup Dear ImGui style
		if (window_color == Dark)
			ImGui::StyleColorsDark();
		else if (window_color == Light)
			ImGui::StyleColorsLight();

		// Setup window flags
		window_flags = 0;
		no_move = true;
		no_resize = true;
		no_background = false;
		no_menu = true;
		no_titlebar = true;
		updateWindowFlags();		
	}

	void static cleanUp() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void static startFrame() {
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();	
	}

	void static rendering() {
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void beginWindow(string _name) {
		updateWindowFlags();
		ImGui::Begin(_name.c_str(), NULL, window_flags);
	}

	void endWindow() {
		ImGui::End();
	}

private:
	void updateWindowFlags() {
		window_flags = 0;
		if (no_move)         window_flags |= ImGuiWindowFlags_NoMove;
		if (no_resize)       window_flags |= ImGuiWindowFlags_NoResize;
		if (no_background)   window_flags |= ImGuiWindowFlags_NoBackground;
		if (!no_menu)        window_flags |= ImGuiWindowFlags_MenuBar;
		if (no_titlebar)     window_flags |= ImGuiWindowFlags_NoTitleBar;
	}
};

#endif // !MYUI_H
