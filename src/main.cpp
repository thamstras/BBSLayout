#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "Gui.h"

#include "Layout.h"

// #### STRUCTS ####
struct WindDef
{
	GLFWwindow* pWindow;
	int width;
	int height;
	bool hasCursorLock;
};

struct MouseDef
{
	double xPos;
	double yPos;
	float deltaX;
	float deltaY;
	bool firstData;
	bool rawMotionAvailible;
};

// #### GLOBALS ####
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

const bool USE_ANTIALIASING = true;
const bool USE_ANISOTROPIC = false;

const float MAX_DELTA_TIME = 0.2f;

const bool DISABLE_MOUSELOCK = true;

WindDef g_window;
MouseDef g_mouse = { 0.0, 0.0, 0.0f, 0.0f, true, false };

float deltaTime = 0.0f;
float lastFrame = 0.0f;
double g_worldTime = 0.0f;

const char windowName[] = "BBSLayout";

// #### CALLBACKS ####
void _cbFramebufferSize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	g_window.width = width;
	g_window.height = height;
}

void lockCursor()
{
	glfwGetCursorPos(g_window.pWindow, &g_mouse.xPos, &g_mouse.yPos);
	glfwSetInputMode(g_window.pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (g_mouse.rawMotionAvailible)
		glfwSetInputMode(g_window.pWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	g_window.hasCursorLock = true;
}

void unlockCursor()
{
	glfwSetInputMode(g_window.pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	if (g_mouse.rawMotionAvailible)
		glfwSetInputMode(g_window.pWindow, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
	g_window.hasCursorLock = false;
}

void _cbCursorPos(GLFWwindow* window, double xpos, double ypos)
{
	if (g_mouse.firstData)
	{
		g_mouse.xPos = xpos;
		g_mouse.yPos = ypos;
		g_mouse.deltaX = 0.0f;
		g_mouse.deltaY = 0.0f;
		g_mouse.firstData = false;
	}

	g_mouse.deltaX = xpos - g_mouse.xPos;
	g_mouse.deltaY = g_mouse.yPos - ypos; // reversed since y-coordinates go from bottom to top

	g_mouse.xPos = xpos;
	g_mouse.yPos = ypos;

	if (ImGui::GetIO().WantCaptureMouse)
	{
		if (g_window.hasCursorLock) unlockCursor();
		return;
	}

	//if (g_window.hasCursorLock)
	//	g_camera.ProcessMouseMovement(g_mouse.deltaX, g_mouse.deltaY);
}

void _cbScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	/*if (g_window.hasCursorLock)*/
	if (!ImGui::GetIO().WantCaptureMouse)
	{
		//g_camera.ProcessMouseScroll(yoffset);
	}
}

// #### FUNCTIONS ####

bool Init()
{
	// GLFW

	// init glfw
	if (glfwInit() == GLFW_FALSE)
	{
		std::cerr << "[FATAL] Failed to init GLFW!" << std::endl;
		return false;
	}
	std::cout << "[GS] Compiled with GLFW " << glfwGetVersionString() << std::endl;
	
	// setup + create window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	if (USE_ANTIALIASING) glfwWindowHint(GLFW_SAMPLES, 4);
	
	GLFWwindow* wind = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, windowName, NULL, NULL);
	if (wind == NULL)
	{
		std::cerr << "[FATAL] Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return false;
	}
	g_window.pWindow = wind;
	
	glfwMakeContextCurrent(wind);
	glfwSwapInterval(1);
	
	if (glfwRawMouseMotionSupported()) g_mouse.rawMotionAvailible = true;

	// OPENGL

	// load function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "[FATAL] Failed to initialize GLAD!" << std::endl;
		glfwTerminate();
		return false;
	}
	
	// setup extensions
	if (GLAD_GL_EXT_texture_filter_anisotropic)
	{
		std::cout << "[GS] Loaded extention GL_EXT_texture_filter_anisotropic!" << std::endl;
		float max_anis;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anis);
		std::cout << "[GS] Max anisotropic samples: " << max_anis << std::endl;
	}

	// setup window
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	g_window.width = SCR_WIDTH;
	g_window.height = SCR_HEIGHT;
	glfwSetFramebufferSizeCallback(wind, _cbFramebufferSize);
	glfwSetCursorPosCallback(wind, _cbCursorPos);
	glfwSetScrollCallback(wind, _cbScroll);

	// get version
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "[GS] Renderer: " << renderer << std::endl;
	std::cout << "[GS] OpenGL version: " << version << std::endl;

	// init ogl state
	// Most of this is irrelevant to a 2D application, but we'll call it all anyway.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	if (USE_ANTIALIASING) glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glDepthFunc(GL_LEQUAL);

	// IMGUI

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigWindowsResizeFromEdges = true;
	io.ConfigDockingWithShift = true;
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsLight();
	ImGui::GetStyle().ColorButtonPosition = ImGuiDir_Left;
	ImGui::GetStyle().FrameBorderSize = 1.0f;
	ImGui_ImplGlfw_InitForOpenGL(g_window.pWindow, true);
	ImGui_ImplOpenGL3_Init();

	std::string fontPath = ".\\Resources\\Fonts\\Roboto-Medium.ttf";
	ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 13.0f);
	if (font == nullptr)
	{
		std::cerr << "[WARN] Failed to load Imgui font. Bad things?" << std::endl;
	}

	return true;
}

void Shutdown()
{
	std::cout << "[GS] Shutting down..." << std::endl;
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(g_window.pWindow);
	glfwTerminate();

	std::cout << "[GS] Goodbye!" << std::endl;
}

void ProcessInput(GLFWwindow* window)
{
	if (ImGui::GetIO().WantCaptureKeyboard)
	{
		if (g_window.hasCursorLock) unlockCursor();
		return;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void render_StartFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

enum eViewModes
{
	VIEW_SPRITE,
	VIEW_SEQUENCE,
	VIEW_LAYOUT
};

int main(int argc, char** argv)
{
	if (!Init())
		exit(EXIT_FAILURE);

	atexit(Shutdown);

	lastFrame = glfwGetTime();

	g_layout = new Layout();
	std::shared_ptr<ShaderLibrary> shaderLibrary = std::make_shared<ShaderLibrary>();

	ShaderDef spriteShaderDef =
	{
		"sprite", ".\\Resources\\Shaders\\sprite.vert.glsl", ".\\Resources\\Shaders\\sprite.frag.glsl"
	};

	shaderLibrary->AddShader(spriteShaderDef);
	g_layout->Setup(shaderLibrary);
	//g_layout->LoadL2D("C:\\BBS\\Dump\\arc\\menu\\CAMP\\camp.l2d");
	//g_layout->LoadL2D("C:\\BBS\Dump\\arc\\menu\\TITLE_V\\title_v.l2d");
	//g_layout->LoadL2D("C:\\BBS\\Dump\\arc\\pc\\P00COMMON\\gauge_01.l2d");
	g_layout->LoadL2D("C:\\BBS\\PC_DUMP\\original\\arc\\menu\\JF_title_tx\\title_txt.l2d");

	int set = 0, sprite = 0;
	eViewModes viewMode = VIEW_SPRITE;

	while (!glfwWindowShouldClose(g_window.pWindow))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		if (deltaTime > MAX_DELTA_TIME) deltaTime = MAX_DELTA_TIME;
		lastFrame = currentFrame;
		g_worldTime += deltaTime;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ProcessInput(g_window.pWindow);

		render_StartFrame();

		//g_layout->Tick(deltaTime, g_worldTime);
		//g_layout->Render();
		g_layout->DrawSingleSprite(set, sprite);

		if (ImGui::Begin("setting"))
		{
			//ImGui::SliderInt("Sprite Set", &set, 0, g_layout->GetSpriteSetCount());
			ImGui::InputInt("SpriteSet", &set);
			if (set < 0) set = 0;
			if (set >= g_layout->GetSpriteSetCount()) set = g_layout->GetSpriteSetCount() - 1;
			//ImGui::SliderInt("Sprite", &sprite, 0, g_layout->GetSpriteCount(set));
			ImGui::InputInt("Sprite", &sprite);
			if (sprite < 0) sprite = 0;
			if (sprite >= g_layout->GetSpriteCount(set)) sprite = g_layout->GetSpriteCount(set) - 1;

			ImGui::End();
		}

		g_layout->gui_SequencesList();
		g_layout->gui_KeysList();

		gui_main();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(g_window.pWindow);
		glfwPollEvents();
	}
}