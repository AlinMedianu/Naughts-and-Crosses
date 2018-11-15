#include <windows.h>
#include <string>
#include <cassert>
#include <d3d11.h>
#include <D3D11SDKLayers.h>

#include "WindowUtils.h"
#include "D3DUtil.h"
#include "D3D.h"
#include "SimpleMath.h"
#include "ShaderTypes.h"
#include "FX.h"
#include "Game.h"
#include "Controller.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


ID3D11InputLayout* gInputLayout;
ID3D11Buffer* gCubeVB;
ID3D11Buffer* gCubeIB;
ID3D11VertexShader* pVertexShader = nullptr;
ID3D11PixelShader* pPixelShader = nullptr;

Matrix gWorld;
Matrix gView;
Matrix gProj;

GridRegionMessenger regionMessenger;

GfxParamsPerObj gGfxData;
ID3D11Buffer* gpGfxConstsPerObjBuffer;

void BuildCubeBuffers() //a simple cube buffer that will be translated, rotated and scaled to make the various graphics
{
	// Create vertex buffer for a cube 
	VertexPosColour vertices[] =
	{
		{ Vector3(-0.5f, +0.5f, -0.5f), Colors::Red },
		{ Vector3(-0.5f, +0.5f, +0.5f), Colors::Green },
		{ Vector3(+0.5f, +0.5f, +0.5f), Colors::Red },
		{ Vector3(+0.5f, +0.5f, -0.5f), Colors::Green },
		{ Vector3(-0.5f, -0.5f, -0.5f), Colors::Green },
		{ Vector3(-0.5f, -0.5f, +0.5f), Colors::Red },
		{ Vector3(+0.5f, -0.5f, +0.5f), Colors::Green },
		{ Vector3(+0.5f, -0.5f, -0.5f), Colors::Red }
	};

	CreateVertexBuffer(sizeof(VertexPosColour) * 8, vertices, gCubeVB);


	// Create the index buffer

	UINT indices[] = 
	{
		// top face
		0, 1, 2,
		0, 2, 3,
		// left face
		0, 5, 1,
		0, 4, 5,
		// right face
		2, 7, 3,
		2, 6, 7,
		// bottom face
		4, 7, 5,
		5, 7, 6,
		// front face
		0, 3, 4,
		3, 7, 4,
		// back face
		1, 6, 2,
		1, 5, 6
	};

	CreateIndexBuffer(sizeof(UINT) * 36, indices, gCubeIB);
}


void UpdateConstsPerObj()
{
	gGfxData.wvp = gWorld * gView * gProj;
	gd3dImmediateContext->UpdateSubresource(gpGfxConstsPerObjBuffer, 0, nullptr, &gGfxData, 0, 0);
}

bool BuildFX()
{
	CheckShaderModel5Supported(gd3dDevice);

	// Create the constant buffers for the variables defined in the vertex shader.
	CreateConstantBuffer(sizeof(GfxParamsPerObj), &gpGfxConstsPerObjBuffer);

	//load in a pre-compiled vertex shader
	char* pBuff = nullptr;
	unsigned int bytes = 0;
	pBuff = ReadAndAllocate("../bin/data/SimpleVS.cso", bytes);
	CreateVertexShader(pBuff, bytes, pVertexShader);
	//create a link between our data and the vertex shader
	CreateInputLayout(VertexPosColour::sVertexDesc, 2, pBuff, bytes, &gInputLayout);
	delete[] pBuff;

	//load in a pre-compiled pixel shader	
	pBuff = ReadAndAllocate("../bin/data/SimplePS.cso", bytes);
	CreatePixelShader(pBuff, bytes, pPixelShader);
	delete[] pBuff;


	return true;

}



void InitGame()
{
	BuildCubeBuffers();
	BuildFX();
	gWorld = Matrix::Identity;
	CreateProjectionMatrix(gProj, 0.25f * PI, GetAspectRatio(), 1, 1000.f);
	CreateViewMatrix(gView, Vector3(0, 0, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
}

void ReleaseGame()
{
	ReleaseCOM(pVertexShader);
	ReleaseCOM(pPixelShader);
	ReleaseCOM(gpGfxConstsPerObjBuffer);
	ReleaseCOM(gCubeVB);
	ReleaseCOM(gCubeIB);
	ReleaseCOM(gInputLayout);
}

void RenderCube()
{
	UpdateConstsPerObj();
	gd3dImmediateContext->VSSetConstantBuffers(0, 1, &gpGfxConstsPerObjBuffer);
	gd3dImmediateContext->DrawIndexed(36, 0, 0);
}


void Render(float dTime)
{
	gd3dImmediateContext->ClearRenderTargetView(gRenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));
	gd3dImmediateContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	InitInputAssembler(gInputLayout, gCubeVB, sizeof(VertexPosColour), gCubeIB);
	gd3dImmediateContext->VSSetShader(pVertexShader, nullptr, 0);
	gd3dImmediateContext->PSSetShader(pPixelShader, nullptr, 0);

	Paint(dTime);

	HR(gSwapChain->Present(0, 0));
}

void OnResize(int screenWidth, int screenHeight)
{
	OnResize_Default(screenWidth, screenHeight);
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//do something game specific here
	switch (msg)
	{
		// Respond to a keyboard event.
		case WM_LBUTTONUP:
		{
			RECT screen;
			GetWindowRect(hwnd, &screen);
			POINT mousePosition;
			mousePosition.x = LOWORD(lParam);
			mousePosition.y = HIWORD(lParam);
			ClientToScreen(hwnd, &mousePosition);
			regionMessenger.SetScreen(screen);				//set up the controller class
			regionMessenger.SetRegion(mousePosition);		//and acknoledge the player's input
		}		
			break;
		case WM_CHAR:
			switch (wParam)
			{
				case 27:
				case 'q':
				case 'Q':
					PostQuitMessage(0);
					return 0;
			}
	}

	//default message handling (resize window, full screen, etc)
	return DefaultMssgHandler(hwnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{

	if(!InitMainWindow(800,600, hInstance, "Naughts And Crosses", MainWndProc))
		assert(false);

	if (!InitDirect3D(OnResize))
		assert(false);

	InitGame();
	Run(Update,Render);
	ReleaseGame();
	ReleaseD3D();
	return 0;
}