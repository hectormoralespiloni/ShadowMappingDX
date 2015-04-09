///============================================================================
///@file	DXApp.h
///@brief	Defines the main DirectX application class
///			This class inherits from Application abstract class which
///			takes care of the windows creation and main render loop
///
///@author	Héctor Morales Piloni
///@date	January 15, 2007
///============================================================================

#ifndef DXAPP_H
#define DXAPP_H

#include <D3DX9.h>
#include <stdio.h>

#include "GraphicsApp.h"
#include "Geometry.h"
#include "Timer.h"

class DXApp : public GraphicsApp
{
public:
	DXApp();
	DXApp(LPSTR windowTitle, USHORT width, USHORT height);
	~DXApp();

	//-------------------------------------------------------------------------
	//Public methods
	//-------------------------------------------------------------------------
	virtual void InitGraphics();
	virtual void Render();
	virtual void RenderText(LPTSTR text);
	virtual bool ShutDown();
	virtual LRESULT DisplayWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

private:
	//-------------------------------------------------------------------------
	//Private methods
	//-------------------------------------------------------------------------
	bool InitDirect3D();
	void CreateShadowMap();
	void CreateTextureMatrix();
	void Reshape(int w,int h);
	void Zoom(float zoomFactor);
	D3DFORMAT FindDepthStencilFormat(ULONG AdapterOrdinal, D3DDISPLAYMODE Mode, D3DDEVTYPE DevType);


	//-------------------------------------------------------------------------
	//Private members
	//-------------------------------------------------------------------------
	LPDIRECT3D9				m_D3D;				///> Direct3D Object
	LPDIRECT3DDEVICE9		m_D3DDevice;		///> Direct3D Device Object
	LPD3DXFONT				m_D3DFont;			///> Direct3D Font Object
	LPD3DXEFFECT			m_Effect;			///> HLSL effects object (shaders)
	D3DPRESENT_PARAMETERS	m_D3DPresentParams;	///> Direct3D Present Params
	Geometry				m_Geometry;			///> Used to draw all the geometry in the scene
	Timer					m_Timer;			///> GL Application timer

	D3DXMATRIX				m_WorldMatrix;				///> World matrix
	D3DXMATRIX				m_CameraProjectionMatrix;	///> Camera projection matrix
	D3DXMATRIX				m_CameraViewMatrix;			///> Camera model-view matrix
	D3DXMATRIX				m_LightProjectionMatrix;	///> Light projection matrix
	D3DXMATRIX				m_LightViewMatrix;			///> Light model-view matrix
};

#endif