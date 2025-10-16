// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "LightShader.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void gui();

private:
	LightShader* shader;
	SphereMesh* mesh;
	Light* light;

	float lightPosition[3];
	float lightDirection[3];
	float lightColor[4];
	float ambientColor[4];
	float innerConeAngle;
	float outerConeAngle;
};

#endif