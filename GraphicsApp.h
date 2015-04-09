///============================================================================
///@file	GraphicsApp.h
///@brief	Defines a Windows Graphics Application Abstract Class
///
///@author	Héctor Morales Piloni
///@date	November 13, 2006
///============================================================================

#ifndef GRAPHICSAPP_H
#define GRAPHICSAPP_H

#include <windows.h>

class GraphicsApp
{
public:
	//-------------------------------------------------------------------------
	//Public methods
	//-------------------------------------------------------------------------
	int		StartApp();
	bool	InitInstance(HANDLE hInstance, LPCTSTR lpCmdLine, int iCmdShow);
	bool	CreateDisplay();
	virtual void	InitGraphics() = 0;
	virtual void	Render() = 0;
	virtual void	RenderText(LPTSTR text);
	virtual bool	ShutDown() = 0;
	virtual LRESULT DisplayWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) = 0;

protected:
	//-------------------------------------------------------------------------
	//Protected methods
	//-------------------------------------------------------------------------
	static	LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	//-------------------------------------------------------------------------
	//Protected members
	//-------------------------------------------------------------------------
	HWND	m_hWnd;			///> Main Window Handler
	LPTSTR	m_WindowTitle;	///> Main Window Title
	USHORT	m_Width;		///> Main Window Width
	USHORT	m_Height;		///> Main Window Height
	HDC		m_hDC;			///> Handle to Device Context
};

#endif