SHADOW MAPPING DX (Jan-2007)
----------------------------

_DirectX version of the classic shadow mapping algorithm to produce hard shadows_

![](https://github.com/hectormoralespiloni/ShadowMappingDX/blob/master/shadowMap_full.jpg)

1. SUMMARY 
    * This demo performs a classic shadow mapping using DirectX and HLSL shaders

2. REQUIREMENTS TO RUN THE EXE
	* DirectX 9.0
	* Video card that supports the shader model 2.0 
	
3. HOW TO PLAY THE DEMO
	* +/- => moves the camera 
	
4. HOW TO COMPILE
	* Microsoft Visual Studio 2005
	* Microsoft Platform SDK (only needed if using VS Express Edition)
	* DirectX SDK 9.0 (october 2006+ update)
	* Just open the .sln file and build the project.

5. CODE STURCTURE
	* The main program creates and starts an instance of a DXApp which 
	inherits from the abstract class GraphicsApp (which is used in 
	other of my demos either with OpenGL or DX).

	* "GraphicsApp" creates the main windows application loop and the 
	window display (i.e. the basic windows stuff).
 
	* "DXApp" takes care of processing the messages, initialize the DirectX
	engine and render the shadow mapped scene.

	* "Geometry" class methods are called from DXApp and they take care
	or rendering the actual x-file scene, set lights and cameras and
	materials. 

	* "Timer" class by Adam Hoult which handles all timing functionality 
	such as counting the number of frames per second, etc.	

	* This demo uses shaders, a file called ShadowMapping.fx contains the
	code for both vertex and fragment shaders.