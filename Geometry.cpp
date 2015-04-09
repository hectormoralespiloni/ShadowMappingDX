///============================================================================
///@file	Geometry.cpp
///@brief	Class used for rendering the geometry in the scene.
///
///@author	Héctor Morales Piloni
///@date	January 17, 2007
///============================================================================

#include "Geometry.h"

///----------------------------------------------------------------------------
///Default constructor
///----------------------------------------------------------------------------
Geometry::Geometry() : m_Camera(0.0f, 0.0f, 0.0f),
					   m_DepthMapRenderTargetSurface(NULL),
					   m_DepthMapRenderTargetTexture(NULL),
					   m_DepthMapStencilSurface(NULL),
					   m_Light(),
					   m_Materials(NULL),
					   m_Mesh(NULL),
					   m_NumMaterials(0),
					   m_Textures(NULL)
{}

///----------------------------------------------------------------------------
///Default destructor
///----------------------------------------------------------------------------
Geometry::~Geometry()
{
	//perform cleanup
	Destroy();
}

///----------------------------------------------------------------------------
///Perform cleanup, unload mesh object
///----------------------------------------------------------------------------
void Geometry::Destroy()
{
	//delete the material list
	if(m_Materials)
		delete[] m_Materials;

	//deallocate each individual texture
	if(m_Textures)
	{
		for(DWORD i=0; i<m_NumMaterials; i++)
			SafeRelease(m_Textures[i]);
		
		//delete texture list
		delete[] m_Textures;
	}

	//delete the mesh object
	SafeRelease(m_Mesh);
}


///----------------------------------------------------------------------------
///Load a mesh from file
///@param	fileName - the mesh file name
///@param	device - D3D device object
///----------------------------------------------------------------------------
void Geometry::LoadMesh(LPCSTR fileName, LPDIRECT3DDEVICE9 device)
{
	ID3DXBuffer *matBuffer;

	//load our scene from X file
	if(FAILED(D3DXLoadMeshFromX(fileName, D3DXMESH_MANAGED, device, NULL, &matBuffer, NULL, &m_NumMaterials, &m_Mesh)))
	{
		MessageBox(NULL,"Error loading mesh","Error",MB_ICONERROR);
		exit(-1);
	}

	//get a pointer to materials data
	D3DXMATERIAL *XfileMats = (D3DXMATERIAL *)matBuffer->GetBufferPointer();

	m_Materials = new D3DMATERIAL9[m_NumMaterials];
	m_Textures = new LPDIRECT3DTEXTURE9[m_NumMaterials];

	//loop through all materials
	for(DWORD i=0; i<m_NumMaterials; i++)
	{
		//copy the material
		m_Materials[i] = XfileMats[i].MatD3D;
		
		//set the ambient color
		m_Materials[i].Ambient = m_Materials[i].Diffuse;

		//append the prefix to current texture filename
		const TCHAR* strPrefix = TEXT("data\\");
		TCHAR strTexture[MAX_PATH];
		strcpy(strTexture, strPrefix);
		strcat(strTexture, XfileMats[i].pTextureFilename);

		//create texture for the material
		if(FAILED(D3DXCreateTextureFromFile(device, strTexture, &m_Textures[i])))
			m_Textures[i] = NULL;
	}

	matBuffer->Release();
}

///----------------------------------------------------------------------------
///Render the mesh object
///----------------------------------------------------------------------------
void Geometry::Draw(LPDIRECT3DDEVICE9 device, LPD3DXEFFECT effect)
{
	if(!m_Mesh) return;

	device->BeginScene();
	{
		device->SetFVF(m_Mesh->GetFVF());

		for(DWORD i=0; i<m_NumMaterials; i++)
		{
			//device->SetMaterial(&m_Materials[i]);
			//device->SetTexture(0, m_Textures[i]);
			effect->SetTexture("sceneTexture", m_Textures[i]);
			effect->CommitChanges();
			m_Mesh->DrawSubset(i);
		}
	}
	device->EndScene();
}

///----------------------------------------------------------------------------
///Set the lights in the scene
///----------------------------------------------------------------------------
void Geometry::SetLights(D3DXVECTOR3 position, LPDIRECT3DDEVICE9 device)
{
	ZeroMemory(&m_Light, sizeof(D3DLIGHT9));

	m_Light.Type = D3DLIGHT_POINT; 
	
	m_Light.Ambient.r = 1.0f;
	m_Light.Ambient.g = 1.0f;
	m_Light.Ambient.b = 1.0f;
	m_Light.Ambient.a = 1.0f;

	m_Light.Diffuse.r = 1.0f;
	m_Light.Diffuse.g = 1.0f;
	m_Light.Diffuse.b = 1.0f;
	m_Light.Diffuse.a = 1.0f;
	
	m_Light.Specular.r = 1.0f;
	m_Light.Specular.g = 1.0f;
	m_Light.Specular.b = 1.0f;
	m_Light.Specular.a = 1.0f;

	m_Light.Range = 200;
	m_Light.Attenuation0 = 1.0;
	m_Light.Attenuation1 = 0.004;
	m_Light.Attenuation2 = 0.0007;
	m_Light.Position = position;

	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	device->LightEnable(0, TRUE);
	device->SetLight(0, &m_Light);
}

///----------------------------------------------------------------------------
///Set the materials of the objects in the scene
///----------------------------------------------------------------------------
void Geometry::SetMaterials(LPDIRECT3DDEVICE9 device)
{
	//No materials here because X file contains its own textures and materials
}

///----------------------------------------------------------------------------
///Sets the camera position.
///@param	pos - the camera position (x,y,z)
///----------------------------------------------------------------------------
void Geometry::SetCameraPosition(D3DXVECTOR3 pos)
{
	m_Camera = pos;
}

///----------------------------------------------------------------------------
///Gets the camera position.
///@return	the camera position vector (x,y,z)
///----------------------------------------------------------------------------
D3DXVECTOR3 Geometry::GetCameraPosition() const
{
	return m_Camera;
}

///----------------------------------------------------------------------------
///Gets the light position.
///@return	the light position vector (x,y,z)
///----------------------------------------------------------------------------
D3DXVECTOR3 Geometry::GetLightPosition() const
{
	return m_Light.Position;
}

///----------------------------------------------------------------------------
///GetShadowMapDS
///@return	shadow map depth stencil object pointer
///----------------------------------------------------------------------------
LPDIRECT3DSURFACE9 Geometry::GetDepthMapRenderTargetSurface() const
{
	return m_DepthMapRenderTargetSurface;
}

///----------------------------------------------------------------------------
///GetShadowMapTexture
///@return	shadow map texture object pointer
///----------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 Geometry::GetDepthMapRenderTargetTexture() const
{
	return m_DepthMapRenderTargetTexture;
}

///----------------------------------------------------------------------------
///GetShadowMapSurface
///@return	shadow map texture object pointer
///----------------------------------------------------------------------------
LPDIRECT3DSURFACE9 Geometry::GetDepthMapStencilSurface() const
{
	return m_DepthMapStencilSurface;
}

///----------------------------------------------------------------------------
///Set textures for shadow maps
///----------------------------------------------------------------------------
void Geometry::SetShadowTexture(LPDIRECT3DDEVICE9 device)
{
	//create render target texture
	device->CreateTexture(DEPTH_MAP_WIDTH,					//texture width
						  DEPTH_MAP_HEIGHT,					//texture height
						  1,								//number of leves in the texture
						  D3DUSAGE_RENDERTARGET,			//usage
						  D3DFMT_R32F,						//internal format
						  D3DPOOL_DEFAULT,					//memory class into which the texture should be placed
						  &m_DepthMapRenderTargetTexture,	//pointer to texture surface
						  NULL);							//reserved, this should be NULL

	//retrieve the specified texture surface level
	m_DepthMapRenderTargetTexture->GetSurfaceLevel(0, &m_DepthMapRenderTargetSurface);

	//create depth stencil surface
	device->CreateDepthStencilSurface(DEPTH_MAP_WIDTH,
									  DEPTH_MAP_HEIGHT,
									  D3DFMT_D24X8,
									  D3DMULTISAMPLE_NONE,
									  0,
									  TRUE,
									  &m_DepthMapStencilSurface,
									  NULL);
}
