///============================================================================
///@file	ShadowMapping.fx
///@brief	The effects file for shadow mapping demo
///
///@author	Héctor Morales Piloni
///@date	January 22, 2007
///============================================================================

VECTOR lightPosition;				//light position
VECTOR cameraPosition;				//eye position
MATRIX matTexture;					//this matrix takes us from camera to light space
MATRIX CameraWorldViewProjection;	//camera world-view-projection matrix
MATRIX LightWorldViewProjection;	//light world-view-projection matrix
TEXTURE sceneTexture;				//each one of the scene textures
TEXTURE shadowMapTexture;			//shadow map texture

sampler2D sceneSampler = sampler_state
{
	Texture = <sceneTexture>;
	MinFilter = POINT;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

sampler2D shadowMapSampler = sampler_state
{
    Texture = <shadowMapTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

void RenderShadowMap_VS(float4 vPos : POSITION,
						out float4 oPos : POSITION,
						out float oDepth : TEXCOORD0)
{
	//output transformed position 
	oPos = mul(vPos, LightWorldViewProjection);
	
	//compute depth value
	oDepth = oPos.z/oPos.w;
}

float4 RenderShadowMap_PS(float iDepth : TEXCOORD0) : COLOR
{
	return float4(iDepth, iDepth, iDepth, iDepth);
}

void RenderScene_VS(float4 vPos : POSITION,
					float3 vNormal : NORMAL,
					float4 vCoords : TEXCOORD0,
					out float4 oPos : POSITION,
					out float4 sceneTexCoords : TEXCOORD0,
					out float4 depthTexCoords : TEXCOORD1,
					out float3 N : TEXCOORD2,
					out float3 L : TEXCOORD3,
					out float3 V : TEXCOORD4)
{
	//output transformed position
	oPos = mul(vPos, CameraWorldViewProjection);
	
	//output texture coordinates
	sceneTexCoords = vCoords;
	
	//compute depth texture coordinates
	depthTexCoords = mul(vPos, matTexture);
	
	//normal pass through
	N = vNormal;
	
	//compute the light vector
	L = lightPosition - vPos.xyz;
	
	//compute the vision vector
	V = cameraPosition - vPos.xyz;
}

float4 RenderScene_PS(float4 sceneTexCoords : TEXCOORD0,
					  float4 depthTexCoords : TEXCOORD1,
					  float3 N : TEXCOORD2,
					  float3 L : TEXCOORD3,
					  float3 V : TEXCOORD4) : COLOR 
{
	//get the shadow factor from shadow map
	float shadow = tex2Dproj(shadowMapSampler, depthTexCoords);
	float depth = (depthTexCoords.z /depthTexCoords.w) - 0.001f;
	
	shadow = (shadow < depth) ? 0.4 : 1.0;
	
	//get the texture color
	float4 color = tex2D(sceneSampler, sceneTexCoords);
	
	//normalize input vectors
	N = normalize(N);
	L = normalize(L);
	V = normalize(V);
	
	//compute the diffuse term
	float diffuse = max(dot(N,L), 0);
	
	//compute the specular term
	float H = normalize(L + V);
	float specular = pow(max(dot(N,H), 0), 1.3);
	
	if(diffuse <= 0) specular = 0;

	return (color * diffuse * shadow) + (color * specular * shadow);
	//return float4(shadow,shadow,shadow,1.0);
}

technique RenderShadowMap
{
    pass P0
    {          
        VertexShader = compile vs_2_0 RenderShadowMap_VS();
        PixelShader  = compile ps_2_0 RenderShadowMap_PS();
    }
}

technique RenderScene
{
    pass P0
    {          
        VertexShader = compile vs_2_0 RenderScene_VS();
        PixelShader  = compile ps_2_0 RenderScene_PS();
    }
}
