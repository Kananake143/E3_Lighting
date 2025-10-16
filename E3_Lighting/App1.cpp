// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	mesh = nullptr;
	shader = nullptr;

	// Set initial spotlight position (hovering above scene center)
	lightPosition[0] = 0.0f;   // Center on X axis
	lightPosition[1] = 10.0f;  // Elevated on Y axis
	lightPosition[2] = 0.0f;   // Center on Z axis

	// Set spotlight direction (pointing downward)
	lightDirection[0] = 0.0f;
	lightDirection[1] = -1.0f; // Negative Y means pointing down
	lightDirection[2] = 0.0f;

	// Configure spotlight color (bright white)
	lightColor[0] = 1.0f;
	lightColor[1] = 1.0f;
	lightColor[2] = 1.0f;
	lightColor[3] = 1.0f;

	// Configure ambient color (subtle blue tint)
	ambientColor[0] = 0.2f;
	ambientColor[1] = 0.2f;
	ambientColor[2] = 0.3f;
	ambientColor[3] = 1.0f;

	// Set cone angles for spotlight shape
	innerConeAngle = 15.0f;  // Full intensity zone
	outerConeAngle = 25.0f;  // Fade-out boundary
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Load texture
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");

	// Create Mesh object and shader object
	mesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	shader = new LightShader(renderer->getDevice(), hwnd);

	// Initialise light

	light = new Light();
	light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setAmbientColour(0.1f, 0.1f, 0.2f, 1.0f);
	light->setPosition(0.0f, 10.0f, 0.0f);
	light->setDirection(0.0f, -1.0f, 0.0f);
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}

	if (shader)
	{
		delete shader;
		shader = 0;
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();


	// Sync light properties with UI control values
	light->setPosition(lightPosition[0], lightPosition[1], lightPosition[2]);
	light->setDirection(lightDirection[0], lightDirection[1], lightDirection[2]);
	light->setDiffuseColour(lightColor[0], lightColor[1], lightColor[2], lightColor[3]);
	light->setAmbientColour(ambientColor[0], ambientColor[1], ambientColor[2], ambientColor[3]);


	// Send geometry data, set shader parameters, render object with shader
	mesh->sendData(renderer->getDeviceContext());
	shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), light);
	shader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();

	return true;
}

void App1::gui()
{
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	ImGui::Separator();
	ImGui::Text("Spotlight Configuration");



	// Position controls
	if (ImGui::CollapsingHeader("Position Controls", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SliderFloat("X Position", &lightPosition[0], -20.0f, 20.0f);
		ImGui::SliderFloat("Y Position", &lightPosition[1], 0.0f, 30.0f);
		ImGui::SliderFloat("Z Position", &lightPosition[2], -20.0f, 20.0f);


	}

	// Direction controls
	if (ImGui::CollapsingHeader("Direction Controls"))
	{
		ImGui::SliderFloat("X Direction", &lightDirection[0], -1.0f, 1.0f);
		ImGui::SliderFloat("Y Direction", &lightDirection[1], -1.0f, 1.0f);
		ImGui::SliderFloat("Z Direction", &lightDirection[2], -1.0f, 1.0f);

		if (ImGui::Button("Normalize Direction"))
		{
			float length = sqrt(lightDirection[0] * lightDirection[0] +
				lightDirection[1] * lightDirection[1] +
				lightDirection[2] * lightDirection[2]);
			if (length > 0.0001f)
			{
				lightDirection[0] /= length;
				lightDirection[1] /= length;
				lightDirection[2] /= length;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Point Down"))
		{
			lightDirection[0] = 0.0f;
			lightDirection[1] = -1.0f;
			lightDirection[2] = 0.0f;
		}
	}

	// Color controls - FIXED: Force float mode (0.0-1.0)
	if (ImGui::CollapsingHeader("Color Settings"))
	{
		// Use ImGuiColorEditFlags_Float to force 0.0-1.0 range
		ImGui::ColorEdit4("Spotlight Color", lightColor, ImGuiColorEditFlags_Float);
		ImGui::ColorEdit4("Ambient Color", ambientColor, ImGuiColorEditFlags_Float);

		if (ImGui::Button("White Light"))
		{
			lightColor[0] = 1.0f;
			lightColor[1] = 1.0f;
			lightColor[2] = 1.0f;
			lightColor[3] = 1.0f;
		}
		ImGui::SameLine();
		if (ImGui::Button("Blue Ambient"))
		{
			ambientColor[0] = 0.1f;
			ambientColor[1] = 0.1f;
			ambientColor[2] = 0.2f;
			ambientColor[3] = 1.0f;
		}
	}

	// Cone controls
	if (ImGui::CollapsingHeader("Cone Shape"))
	{
		ImGui::SliderFloat("Inner Cone", &innerConeAngle, 5.0f, 45.0f);
		ImGui::SliderFloat("Outer Cone", &outerConeAngle, 10.0f, 60.0f);

		if (outerConeAngle < innerConeAngle)
		{
			outerConeAngle = innerConeAngle + 5.0f;
		}

		ImGui::Text("Inner: %.1f deg, Outer: %.1f deg", innerConeAngle, outerConeAngle);
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

