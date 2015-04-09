///============================================================================
///@file	GraphicsApp.cpp
///@brief	Windows Graphics Application Abstract Class Implementation
///
///@author	Héctor Morales Piloni
///@date	November 13, 2006
///============================================================================

#include "GraphicsApp.h"

///----------------------------------------------------------------------------
///Initializes this GraphicsApp instance
///----------------------------------------------------------------------------
bool GraphicsApp::InitInstance(HANDLE hInstance, LPCTSTR lpCmdLine, int iCmdShow)
{
	if(!CreateDisplay())
	{
		ShutDown();
		return false;
	}

	return true;
}

///----------------------------------------------------------------------------
///Starts the main application loop
///----------------------------------------------------------------------------
int GraphicsApp::StartApp()
{
	MSG msg;

	while(true)
	{
		//Did we receive a message or are we idling?
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//render the scene
			Render();
		}
	}

	return 0;
}

///----------------------------------------------------------------------------
///Creates the main rendering window and initializes graphics device
///----------------------------------------------------------------------------
bool GraphicsApp::CreateDisplay()
{
	RECT rc;

	//register the new window class
	WNDCLASS wc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hIcon			= LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDC_ICON));
	wc.hInstance		= (HINSTANCE)GetModuleHandle(NULL);
	wc.lpfnWndProc		= StaticWndProc;
	wc.lpszClassName	= m_WindowTitle;
	wc.lpszMenuName		= NULL;
	wc.style			= CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);

	//create the rendering window
	m_hWnd = CreateWindow(m_WindowTitle,		//lpClassName 
						  m_WindowTitle,		//lpWindowName
						  WS_OVERLAPPEDWINDOW,	//dwStyle
						  CW_USEDEFAULT,		//x
						  CW_USEDEFAULT,		//y
						  m_Width,				//width
						  m_Height,				//height
						  NULL,					//hWndParent
						  NULL,					//hMenu
						  wc.hInstance,			//hInstance
						  this);				//lParam
	
	if(!m_hWnd) return false;

	//show the window
	ShowWindow(m_hWnd, SW_SHOW);

	//initilizes the graphics device
	InitGraphics();

	return true;
}

///----------------------------------------------------------------------------
///Function through which Windows will route all messages, our application
///uses a static member function to distribute the window messages to the 
///correct instance of the class.
///----------------------------------------------------------------------------
LRESULT CALLBACK GraphicsApp::StaticWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	//In windows every window has a 4 byte user data area where you can store
	//application defined data to be associated with the window; this means 
	//the window itself stores the instance of GLApp for which it was created
	if(Msg == WM_CREATE)
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR*)lParam)->lpCreateParams);

	//we are limited for any other messages because we cannot access private members
	//under a static context, thus we retrieve the instance for which the message is intended
	GraphicsApp *Destination = (GraphicsApp *)GetWindowLong(hWnd, GWL_USERDATA);

	//Finally we forward the message to a non-static member of the class
	if(Destination)
		return Destination->DisplayWndProc(hWnd, Msg, wParam, lParam);

	//No destination found, defer to system...
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

///----------------------------------------------------------------------------
///Helper function used to render some information on screen (FPS, etc)
///@param	text pointer to string to render
///----------------------------------------------------------------------------
void GraphicsApp::RenderText(LPTSTR text)
{

}