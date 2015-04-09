///============================================================================
///@file	Geometry.h
///@brief	Class used for rendering the geometry in the scene.
///
///@author	Héctor Morales Piloni
///@date	January 16, 2007
///============================================================================

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <D3DX9.h>
#include <math.h>

template <typename T> inline void SafeRelease(T& x)
{
	if(x)
	{
		x->Release();
		x = NULL;
	}
}

class Geometry
{
public:
	//-------------------------------------------------------------------------
	//Constructors and destructors
	//-------------------------------------------------------------------------
	Geometry();
	~Geometry();

	//-------------------------------------------------------------------------
	//Public methods
	//-------------------------------------------------------------------------
	void LoadMesh(LPCSTR fileName, LPDIRECT3DDEVICE9 device);
	void Draw(LPDIRECT3DDEVICE9 device, LPD3DXEFFECT effect);
	void SetLights(D3DXVECTOR3 position, LPDIRECT3DDEVICE9 device);
	void SetCameraPosition(D3DXVECTOR3 position);
	void SetMaterials(LPDIRECT3DDEVICE9 device);
	void SetShadowTexture(LPDIRECT3DDEVICE9 device);
	void Destroy();
	D3DXVECTOR3 GetCameraPosition() const;
	D3DXVECTOR3 GetLightPosition() const;
	LPDIRECT3DTEXTURE9 GetDepthMapRenderTargetTexture() const;
	LPDIRECT3DSURFACE9 GetDepthMapRenderTargetSurface() const;
	LPDIRECT3DSURFACE9 GetDepthMapStencilSurface() const;

	//-------------------------------------------------------------------------
	//Public members
	//-------------------------------------------------------------------------
	static const unsigned int DEPTH_MAP_WIDTH  = 512;	///> Depth map width
	static const unsigned int DEPTH_MAP_HEIGHT = 512;	///> Depth map height

private:
	//-------------------------------------------------------------------------
	//Private members
	//-------------------------------------------------------------------------
	D3DLIGHT9 m_Light;		///> Light object
	D3DXVECTOR3 m_Camera;	///> Camera's position
	LPD3DXMESH m_Mesh;		///> Mesh object
	DWORD m_NumMaterials;	///> Number of mesh materials
	D3DMATERIAL9 *m_Materials;		///> List of mesh materials
	LPDIRECT3DTEXTURE9 *m_Textures;	///> List of mesh textures

	LPDIRECT3DSURFACE9 m_DepthMapStencilSurface;		///> surface object to access the depth map texture
	LPDIRECT3DTEXTURE9 m_DepthMapRenderTargetTexture;	///> texture used as a render target
	LPDIRECT3DSURFACE9 m_DepthMapRenderTargetSurface;	///> surface object to access the texture
};

#endif
