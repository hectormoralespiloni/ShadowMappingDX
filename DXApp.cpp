///============================================================================
///@file	DXApp.cpp
///@brief	DirectX Application Class Implementation
///
///@author	Héctor Morales Piloni
///@date	January 15, 2007
///============================================================================

#include "DXApp.h"

///----------------------------------------------------------------------------
///Default constructor.
///----------------------------------------------------------------------------
DXApp::DXApp()
{
	//call overloaded constructor with default values
	DXApp("", 640, 480);
}

///----------------------------------------------------------------------------
///Overloaded constructor.
///@param	windowTitle - window title
///@param	width		- window width
///@param	height		- window height
///----------------------------------------------------------------------------
DXApp::DXApp(LPSTR windowTitle, USHORT width, USHORT height)
{
	//clear all required values
	m_hWnd	= NULL;
	m_hDC	= NULL;

	//set all required values
	m_WindowTitle	= windowTitle;
	m_Width			= width;
	m_Height		= height;
}

///----------------------------------------------------------------------------
///Default destructor.
///----------------------------------------------------------------------------
DXApp::~DXApp()
{
	//perform clean-up
	ShutDown();
}

///----------------------------------------------------------------------------
///Clean up resources.
///----------------------------------------------------------------------------
bool DXApp::ShutDown()
{
	m_Geometry.Destroy();

	return true;
}

///----------------------------------------------------------------------------
///This function handles messages for the GLApp object
///@param	hWnd - handle to window
///@param	Msg - the message sent
///@param	wParam - message parameter
///@param	lParam - message parameter
///----------------------------------------------------------------------------
LRESULT DXApp::DisplayWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_CREATE:
			break;

		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_SIZE:
			break;

		case WM_CHAR:
			switch(wParam)
			{
				case '+':
					Zoom(0.1);
					break;

				case '-':
					Zoom(-0.1);
					break;
			}
			break;

		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	return 0;
}

///----------------------------------------------------------------------------
///FindDepthStencilFormat ()
///This function simply determines the best depth format that is
///available for the specified mode.
///----------------------------------------------------------------------------
D3DFORMAT DXApp::FindDepthStencilFormat(ULONG AdapterOrdinal, D3DDISPLAYMODE Mode, D3DDEVTYPE DevType)
{
    //Test for 24 bith depth buffer
    if(SUCCEEDED(m_D3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL , D3DRTYPE_SURFACE , D3DFMT_D32)))
    {
        if(SUCCEEDED(m_D3D->CheckDepthStencilMatch (AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D32))) 
		return D3DFMT_D32;
    }

    //Test for 24 bit depth buffer
    if(SUCCEEDED(m_D3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL , D3DRTYPE_SURFACE , D3DFMT_D24X8)))
    {
        if(SUCCEEDED(m_D3D->CheckDepthStencilMatch (AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D24X8))) 
		return D3DFMT_D24X8;
	}

    //Test for 16 bit depth buffer
    if(SUCCEEDED(m_D3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL , D3DRTYPE_SURFACE , D3DFMT_D16)))
    {
        if(SUCCEEDED(m_D3D->CheckDepthStencilMatch (AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D16))) 
			return D3DFMT_D16;    
    }

    //No depth buffer supported
    return D3DFMT_UNKNOWN;
}

///----------------------------------------------------------------------------
///Initialize Direct3D
///----------------------------------------------------------------------------
bool DXApp::InitDirect3D()
{
	HRESULT					hRet;
	D3DPRESENT_PARAMETERS	presentParams;
	D3DCAPS9				caps;
	D3DDISPLAYMODE			currentMode;

	//create our D3D object
	m_D3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(!m_D3D) return false;

	//fill out a simple set of present parameters
	ZeroMemory(&presentParams, sizeof(D3DPRESENT_PARAMETERS));

	//set default presentation parameters
	m_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &currentMode);
	presentParams.BackBufferFormat		 = currentMode.Format;
	presentParams.PresentationInterval	 = D3DPRESENT_INTERVAL_IMMEDIATE;
	presentParams.SwapEffect			 = D3DSWAPEFFECT_DISCARD;
	presentParams.AutoDepthStencilFormat = FindDepthStencilFormat(D3DADAPTER_DEFAULT, currentMode, D3DDEVTYPE_HAL);
	presentParams.EnableAutoDepthStencil = TRUE;
	presentParams.Windowed				 = TRUE;
	presentParams.Flags					 = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

	//set creation flags
	unsigned long flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//check if HW T&L is available
	ZeroMemory(&caps, sizeof(D3DCAPS9));
	m_D3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

		if(caps.DevCaps & D3DDEVCAPS_PUREDEVICE)
			flags |= D3DCREATE_PUREDEVICE;
	}

	//attempt to create a HAL device
	if(FAILED(hRet = m_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, flags, &presentParams, &m_D3DDevice)))
	{
		//No HAL device, find REF buffer format
		presentParams.AutoDepthStencilFormat = FindDepthStencilFormat(D3DADAPTER_DEFAULT, currentMode, D3DDEVTYPE_REF);

		//check if HW T&L is available
		ZeroMemory(&caps, sizeof(D3DCAPS9));
		flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		m_D3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, &caps);

		if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
			flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

		//attempt to create a REF device
		if(FAILED(hRet = m_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_hWnd, flags, &presentParams, &m_D3DDevice)))
		{
			return false;
		}//failure REF

	}//failure HAL

	//store present params
	m_D3DPresentParams = presentParams;

	//create a font for message display
	D3DXCreateFont(m_D3DDevice, 16, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, 
				   OUT_TT_ONLY_PRECIS, 0, 0, "Verdana", &m_D3DFont);

	//load shader effects from fx file
	ID3DXBuffer* errors = 0;
	if(FAILED(D3DXCreateEffectFromFile(m_D3DDevice, "ShadowMapping.fx", NULL, NULL, NULL, NULL, &m_Effect, &errors)))
	{
		return false;
	}
	
	if(errors)
	{
		MessageBox(NULL, (char*)errors->GetBufferPointer(), "Shader compilation failed!", MB_ICONERROR);
		errors->Release();
		return false;
	}

	//success!
	return true;
}

///----------------------------------------------------------------------------
///Initialize DirectX graphic device context.
///----------------------------------------------------------------------------
void DXApp::InitGraphics()
{
	//initialize Direct3D
	if(!InitDirect3D())
	{
		MessageBox(NULL, "Direct3D Initialization failed!", "ERROR", MB_ICONERROR);
		return;
	}

	//set light & camera position
	m_Geometry.SetLights(D3DXVECTOR3(15.0, 10.0, 15.0), m_D3DDevice);
	m_Geometry.SetCameraPosition(D3DXVECTOR3(10.0, 10.0, -10.0));
	m_Geometry.SetShadowTexture(m_D3DDevice);

    //set camera matrices
    D3DXMatrixPerspectiveFovLH(&m_CameraProjectionMatrix, D3DXToRadian(45.0f), (float)m_Width/(float)m_Height, 1.0f, 100.0f );
	D3DXMatrixLookAtLH(&m_CameraViewMatrix, 
					   &m_Geometry.GetCameraPosition(), //Eye-vector 
					   &D3DXVECTOR3(0.0, 0.0, 0.0),		//At-vector
					   &D3DXVECTOR3(0.0, 1.0, 0.0));	//Up-vector

	//this scene mesh requires a world translation for better viewing
	D3DXMatrixTranslation(&m_WorldMatrix, -7.0f, -2.0f, 0.0f);

	//set light matrices
    D3DXMatrixPerspectiveFovLH(&m_LightProjectionMatrix, D3DXToRadian(45.0f), 1.0, 1.0f, 100.0f );
	D3DXMatrixLookAtLH(&m_LightViewMatrix, 
					   &m_Geometry.GetLightPosition(),	//Eye-vector 
					   &D3DXVECTOR3(0.0, 0.0, 0.0),		//At-vector
					   &D3DXVECTOR3(0.0, 1.0, 0.0));	//Up-vector

    //setup our D3D Device initial states
    m_D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	//set effect's light & camera positions for lighting calculations
	m_Effect->SetVector("lightPosition", (D3DXVECTOR4 *)&m_Geometry.GetLightPosition());
	m_Effect->SetVector("cameraPosition", (D3DXVECTOR4 *)&m_Geometry.GetCameraPosition());

	//load mesh object
	m_Geometry.LoadMesh("data\\scene.x", m_D3DDevice);
}

///----------------------------------------------------------------------------
///Creates the shadow map texture based on light's point of view.
///----------------------------------------------------------------------------
void DXApp::CreateShadowMap()
{
	UINT numPasses = 0;

	//save the current render target & stencil surface
	LPDIRECT3DSURFACE9 windowRenderTarget = NULL;
	LPDIRECT3DSURFACE9 windowDepthSurface = NULL;
	m_D3DDevice->GetRenderTarget(0, &windowRenderTarget);
	m_D3DDevice->GetDepthStencilSurface(&windowDepthSurface);

	//set the new render target and depth stencil surface
	m_D3DDevice->SetRenderTarget(0, m_Geometry.GetDepthMapRenderTargetSurface());
	m_D3DDevice->SetDepthStencilSurface(m_Geometry.GetDepthMapStencilSurface());

	//clear buffers
	m_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0, 0);

	//set the light model view matrix
	D3DXMATRIX lightWVP = m_WorldMatrix * m_LightViewMatrix * m_LightProjectionMatrix;
	m_Effect->SetMatrix("LightWorldViewProjection", &lightWVP);

	//render the scene 
	m_Effect->SetTechnique("RenderShadowMap");
	m_Effect->Begin(&numPasses, 0);
	{
		m_Effect->BeginPass(0);
		m_Geometry.Draw(m_D3DDevice, m_Effect);
		m_Effect->EndPass();
	}
	m_Effect->End();

	//restore render target & depth surface
	m_D3DDevice->SetDepthStencilSurface(windowDepthSurface);
	m_D3DDevice->SetRenderTarget(0, windowRenderTarget);
	windowDepthSurface->Release();
	windowRenderTarget->Release();
}

///----------------------------------------------------------------------------
///We need texture coordinates as if the light source were the eye point.
///----------------------------------------------------------------------------
void DXApp::CreateTextureMatrix()
{
	D3DXMATRIX textureMatrix;
	D3DXMATRIX cameraInverse;

	float fOffsetX = 0.5f + (0.5f / m_Geometry.DEPTH_MAP_WIDTH);
	float fOffsetY = 0.5f + (0.5f / m_Geometry.DEPTH_MAP_HEIGHT);

	//compute bias matrix
	D3DXMATRIX biasMatrix( 0.5f,		0.0f,		0.0f,		0.0f,
						   0.0f,	   -0.5f,		0.0f,		0.0f,
						   0.0f,		0.0f,		1.0f,		0.0f,
						   fOffsetX,	fOffsetY,	0.0f,		1.0f );

	//compute camera's view inverse
	D3DXMatrixInverse(&cameraInverse, NULL, &m_CameraViewMatrix);
	D3DXMatrixTranspose(&cameraInverse, &cameraInverse);
	
	//concatenate matrices to compute texture matrix
	textureMatrix = m_WorldMatrix * m_LightViewMatrix * m_LightProjectionMatrix * biasMatrix;
	
	//set matrix in effect shaders
	m_Effect->SetMatrix("matTexture", &textureMatrix);
}

///----------------------------------------------------------------------------
///Overriden Render function (draws the scene).
///----------------------------------------------------------------------------
void DXApp::Render()
{
	static bool created = false;

	//num of render passes (used for FX techniques)
	UINT numPasses = 0;

	//lock timer to 60 fps
	m_Timer.Tick(60.0);

	if(!created)
	{
	CreateShadowMap();
	CreateTextureMatrix();
	created = true;
	}

	//clear buffers
	m_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0, 0);

	//set the camera model view matrix
	D3DXMATRIX cameraWVP = m_WorldMatrix * m_CameraViewMatrix * m_CameraProjectionMatrix;
	m_Effect->SetMatrix("CameraWorldViewProjection", &cameraWVP);

	//render the scene
	m_Effect->SetTechnique("RenderScene");
	m_Effect->SetTexture("shadowMapTexture", m_Geometry.GetDepthMapRenderTargetTexture());
	m_Effect->Begin(&numPasses, 0);
	{
		m_Effect->BeginPass(0);
		m_Geometry.Draw(m_D3DDevice, m_Effect);
		m_Effect->EndPass();
	}
	m_Effect->End();

	RenderText("Use: +/- to move the camera");

	//swap buffers
	m_D3DDevice->Present(NULL, NULL, NULL, NULL);
}

///----------------------------------------------------------------------------
///Draws some text in the scene (i.e. FPS, etc)
///----------------------------------------------------------------------------
void DXApp::RenderText(LPTSTR text)
{
	RECT rc = {5, 5, 0, 0};
	m_D3DFont->DrawTextA(NULL, text, -1, &rc, DT_NOCLIP, 0xFFAA0000);
}

///----------------------------------------------------------------------------
///Reset the viewport when window size changes
///@param	w - window width
///@param	h - window height
///----------------------------------------------------------------------------
void DXApp::Reshape(int w, int h)
{

}

///----------------------------------------------------------------------------
///Zoom in/out the camera
///@param	zoomFactor - how much zoom are we going to apply
///			if positive the camera gets closer to the scene,
///			if negative the camera gets further.
///----------------------------------------------------------------------------
void DXApp::Zoom(float zoomFactor)
{
	D3DXVECTOR3 newCam = m_Geometry.GetCameraPosition();
	newCam.z += zoomFactor;
	m_Geometry.SetCameraPosition(newCam);

	D3DXMatrixLookAtLH(&m_CameraViewMatrix, 
					   &newCam, 
					   &D3DXVECTOR3(0.0, 0.0, 0.0),
					   &D3DXVECTOR3(0.0, 1.0, 0.0));
	m_D3DDevice->SetTransform(D3DTS_VIEW, &m_CameraViewMatrix);
}
