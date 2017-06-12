//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once

#include "Mesh.h"
#include "Camera.h"

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20


class CMaterialColors
{
public:
	CMaterialColors();
	virtual ~CMaterialColors();

private:
	int								m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	D3DXCOLOR						m_d3dxcAmbient;
	D3DXCOLOR						m_d3dxcDiffuse;
	D3DXCOLOR						m_d3dxcSpecular; //(r,g,b,a=power)
	D3DXCOLOR						m_d3dxcEmissive;
};

class CTexture
{
public:
	CTexture(int nTextures = 1, int nSamplers = 1, int nTextureStartSlot = 0, int nSamplerStartSlot = 0);
	virtual ~CTexture();

private:
	int								m_nReferences;

	int								m_nTextures;
	ID3D11ShaderResourceView		**m_ppd3dsrvTextures;
	int								m_nTextureStartSlot;

	int								m_nSamplers;
	ID3D11SamplerState				**m_ppd3dSamplerStates;
	int								m_nSamplerStartSlot;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture);
	void SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);
	void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);
	void UpdateSamplerShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);

	static ID3D11Buffer				*m_pd3dcbTextureMatrix;

	static void CreateShaderVariables(ID3D11Device *pd3dDevice);
	static void ReleaseShaderVariables();
	static void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxTexture);
	static ID3D11ShaderResourceView *CreateTexture2DArraySRV(ID3D11Device *pd3dDevice, _TCHAR(*ppstrFilePaths)[128], UINT nTextures);
};

class CMaterial
{
public:
	CMaterial(CMaterialColors *pColors = NULL);
	virtual ~CMaterial();

private:
	int								m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetTexture(CTexture *pTexture);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);

	CMaterialColors					*m_pColors;
	CTexture						*m_pTexture;
};

class CShader;

class CGameObject
{
public:
	bool on = false;
	CGameObject(int nMeshes = 0);
	virtual ~CGameObject();
	bool m_bCheckPlayer = false;
private:
	int								m_nReferences;

	bool							m_bActive;
	bool							m_bIsVisible;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	bool titlevisivle;
public:
	D3DXMATRIX						m_d3dxmtxLocal;
	D3DXMATRIX						m_d3dxmtxWorld;

	static D3DXMATRIX				m_d3dxmtxIdentity;

	D3DXMATRIX						m_d3dxmtxShadow;

protected:
	CMesh							**m_ppMeshes;
	int								m_nMeshes;
	AABB							m_bcMeshBoundingCube;

	CMaterial						*m_pMaterial;

	CShader							*m_pShader;

	CGameObject 					*m_pChild;
	CGameObject 					*m_pSibling;
	CGameObject 					*m_pParent;

	AABB							m_bcBoundingCube;
	AABB							m_bcBoundingCubeHead;
	AABB							m_bcBoundingCubeBody;
	AABB							m_bcBoundingCubeRhand;
	AABB							m_bcBoundingCubeLhand;
	AABB							m_bcBoundingCubeRleg;
	AABB							m_bcBoundingCubeLleg;


	//바운딩 큐브를 렌더링하기위한  쉐이더
	CShader							*m_pBoundingShader;

public:
	int numofbullet=0;
public:
	ID3D11RasterizerState			*m_pd3dRasterizerState;
	ID3D11DepthStencilState			*m_pd3dDepthStencilState;
	ID3D11BlendState				*m_pd3dBlendState;

public:
	void SetMesh(CMesh *pMesh, int nIndex = 0);
	CMesh *GetMesh(int nIndex = 0) { return(m_ppMeshes[nIndex]); }
	UINT GetMeshType() { return((m_ppMeshes) ? m_ppMeshes[0]->GetType() : 0x00); }

	void SetShader(CShader *pShader);
	CShader *GetShader() { return(m_pShader); }


	//바운딩 쉐이더를 set,get
	void SetBoundingShader(CShader *pBoundingShader);
	CShader *GetBoundingShader() { return(m_pBoundingShader); }

	void SetShadowMatrix(D3DXMATRIX *pd3dxmtxShadow) { m_d3dxmtxShadow = *pd3dxmtxShadow; }
	void SetShadowMatrix(D3DXVECTOR4 d3dxvLight, D3DXPLANE d3dxPlane);

	void SetMaterial(CMaterial *pMaterial);
	CMaterial *GetMaterial() { return(m_pMaterial); }

	void SetChild(CGameObject *pChild);
	CGameObject *GetParent() { return(m_pParent); }

	void SetPosition(float x, float y, float z);
	void SetPosition(D3DXVECTOR3 d3dxvPosition);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle);

	D3DXVECTOR3 GetPosition(bool bIsLocal = true);
	D3DXVECTOR3 GetLook(bool bIsLocal = true);
	D3DXVECTOR3 GetUp(bool bIsLocal = true);
	D3DXVECTOR3 GetRight(bool bIsLocal = true);

	bool IsVisible(CCamera *pCamera = NULL);

	void Update(D3DXMATRIX *pd3dxmtxParent);
	void PlayerUpdate(D3DXMATRIX *pd3dxmtxParent);


	virtual void Animate(float fTimeElapsed);
	virtual void Animate(float fTimeElapsed, D3DXMATRIX *pd3dxmtxParent);
	virtual void OnPrepareRender();
	virtual void RenderMesh(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

	void SetActive(bool bActive = false) { m_bActive = bActive; }
	void GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection);
	int PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo);


	//바운딩 큐브들 메쉬에서 뽑아서 set하는 함수

	void SetBoundingCubes(CMesh * pMesh);


public:
	static ID3D11Buffer				*m_pd3dcbWorldMatrix;
	static ID3D11Buffer				*m_pd3dcbMaterialColors;

public:
	static void CreateShaderVariables(ID3D11Device *pd3dDevice);
	static void ReleaseShaderVariables();
	static void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);
	static void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, CMaterialColors *pMaterialColors);

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void CreateDepthStencilState(ID3D11Device *pd3dDevice) { }
	virtual void CreateBlendState(ID3D11Device *pd3dDevice) { }
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes = 1);
	virtual ~CRotatingObject();

private:
	D3DXVECTOR3					m_d3dxvRotationAxis;
	float						m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(D3DXVECTOR3 d3dxvRotationAxis) { m_d3dxvRotationAxis = d3dxvRotationAxis; }

	virtual void Animate(float fTimeElapsed);
};

class CRevolvingObject : public CGameObject
{
public:
	CRevolvingObject(int nMeshes = 1);
	virtual ~CRevolvingObject();

private:
	D3DXVECTOR3					m_d3dxvRevolutionAxis;
	float						m_fRevolutionSpeed;

public:
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
	void SetRevolutionAxis(D3DXVECTOR3 d3dxvRevolutionAxis) { m_d3dxvRevolutionAxis = d3dxvRevolutionAxis; }

	virtual void Animate(float fTimeElapsed);
};

class CHeightMap
{
private:
	BYTE						*m_pHeightMapImage;
	int							m_nWidth;
	int							m_nLength;
	D3DXVECTOR3					m_d3dxvScale;

public:
	CHeightMap(LPCTSTR pFileName, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale);
	virtual ~CHeightMap();

	float GetHeight(float x, float z, bool bReverseQuad = false);
	D3DXVECTOR3 GetHeightMapNormal(int x, int z);
	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }

	BYTE *GetHeightMapImage() { return(m_pHeightMapImage); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D11Device *pd3dDevice, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, D3DXVECTOR3 d3dxvScale);
	virtual ~CHeightMapTerrain();

private:
	CHeightMap					*m_pHeightMap;

	int							m_nWidth;
	int							m_nLength;

	D3DXVECTOR3					m_d3dxvScale;

	CWaterGridMesh				**m_ppWaterMeshes;
	int							m_nWaterMeshes;

public:
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMap->GetHeight(x, z, bReverseQuad) * m_d3dxvScale.y); } //World
	D3DXVECTOR3 GetNormal(float x, float z) { return(m_pHeightMap->GetHeightMapNormal(int(x / m_d3dxvScale.x), int(z / m_d3dxvScale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMap->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMap->GetHeightMapLength()); }

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
	float GetWidth() { return(m_nWidth * m_d3dxvScale.x); }
	float GetLength() { return(m_nLength * m_d3dxvScale.z); }

	float GetPeakHeight() { return(m_bcMeshBoundingCube.m_d3dxvMaximum.y); }

#ifdef _WITH_TERRAIN_TEXTURE_ARRAY
	ID3D11Buffer				*m_pd3dcbTextureIndex;
#endif
};

class CTerrainWater : public CGameObject
{
public:
	CTerrainWater(ID3D11Device *pd3dDevice, int nWidth, int nLength, int nBlockWidth, int nBlockLength, D3DXVECTOR3 d3dxvScale);
	virtual ~CTerrainWater();

private:
	int							m_nWidth;
	int							m_nLength;

	D3DXVECTOR3					m_d3dxvScale;

public:
	D3DXMATRIX					m_d3dxmtxTexture;

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D11Device *pd3dDevice);
	virtual ~CSkyBox();

#ifdef _WITH_SKYBOX_TEXTURE_ARRAY
	ID3D11Buffer				*m_pd3dcbTextureIndex;
#endif

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};


class BoundingObject
{
public:
	BoundingObject();
	virtual ~BoundingObject();
	virtual void Animate(float fTimeElapsed);
};

class CBulletObject : public CGameObject
{
public:
	CBulletObject(int nMeshes = 1);
	virtual ~CBulletObject();
	
	int time = 0;
private:
	D3DXVECTOR3					m_d3dxvRotationAxis;
	float						m_fRotationSpeed;
	D3DXVECTOR3					m_d3dxvDirection;
	float						m_fMovingSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(D3DXVECTOR3 d3dxvRotationAxis) { m_d3dxvRotationAxis = d3dxvRotationAxis; }
	void SetMovingSpeed(float fMovingSpeed) { m_fMovingSpeed = fMovingSpeed; }
	void SetMovingDirection(D3DXVECTOR3 d3dxvMovingDirection) { m_d3dxvDirection = d3dxvMovingDirection; }
	void SetLookvector(D3DXVECTOR3 d3dxvMovingDirection) { m_d3dxvDirection = d3dxvMovingDirection; }
	//virtual void Animate(float fTimeElapsed);
};

class CAnimationObject : public CGameObject
{
public:
	CAnimationObject();
	virtual ~CAnimationObject();

private:
	int m_nReferences{ 0 };

public:
	void AddRef() { ++m_nReferences; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual void SetMesh(CMarineMeshDiffused *pMesh)
	{
		if (m_pMesh) m_pMesh->Release();
		pMesh->AddRef();
		m_pMesh = pMesh;
	}

	virtual void SetAnimation(string animName)
	{
		auto iter = m_mapAnimationSet.find(animName);

		if (iter == m_mapAnimationSet.end())
		{
			printf("잘못된 애니메이션 이름입니다.\n");
		}
		else if (animName == m_strCurrentAnimation)
		{
			printf("같은 애니메이션을 호출했으니 그대로 진행합니다.");
		}
		else //애니메이션을 찾았다면
		{
			int startKeyFrameNum = iter->second.first;
			m_fAnimationElapsedTime = m_pMesh->GetKeyFrameTime(0, startKeyFrameNum);
			m_strCurrentAnimation = animName;
		}
	}

	virtual void Animate(float fTimeElapsed)
	{
#ifndef ANIMATION_INTERPOLATION
		//********************
		//고정 프레임
		m_fAnimationElapsedTime += fTimeElapsed;

		static int nCount = 0;

		while (m_fAnimationElapsedTime > 0.033333f)
		{
			nCount++;
			m_fAnimationElapsedTime -= 0.033333f;
		}

		m_pMesh->UpdateBoneTransform(0, nCount);

		if (nCount >= m_pMesh->GetMaxFrame())
		{
			nCount = 0;
			m_fAnimationElapsedTime = 0.0f;
		}
#else
		//********************
		//보간 진행할 시
		m_fAnimationElapsedTime += fTimeElapsed;

		m_pMesh->InterpolateBoneTransform(0, m_fAnimationElapsedTime);
		for (int i = 0; i < m_pMesh->GetNumBones(); ++i)
		{
			m_d3dxmtxFinalTransform[i] = m_pMesh->GetBoneTransform()[i];
		}

		int startKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.first;
		int endKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.second;
		float endTime = m_pMesh->GetKeyFrameTime(0, endKeyFrameNum);

		if (endTime <= m_fAnimationElapsedTime)
			m_fAnimationElapsedTime = m_pMesh->GetKeyFrameTime(0, startKeyFrameNum);
#endif
	}

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera* pCamera)
	{
		m_pMesh->Render(pd3dDeviceContext);
	}

	D3DXMATRIX m_d3dxmtxWorld;
	CMarineMeshDiffused* m_pMesh{ nullptr };

	string							m_strCurrentAnimation;
	map<string, pair<int, int>>		m_mapAnimationSet;

	float		m_fAnimationElapsedTime{ 0.0f };

	D3DXMATRIX	m_d3dxmtxFinalTransform[MAX_BONE_COUNT];
};

