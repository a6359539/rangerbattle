#pragma once


#include <D3DX10Math.h>
#pragma comment(lib, "d3dx10.lib") 

using namespace std;

struct boundbox
{
	D3DXVECTOR3 Min;
	D3DXVECTOR3 Max;
};



class CGameObject
{
public:
	CGameObject(int nMeshes = 0);
	virtual ~CGameObject();
	boundbox						temp;
	boundbox						Boundmain;
	boundbox							m_bcBoundingCube;
	boundbox							m_bcBoundingCubeHead;
	boundbox							m_bcBoundingCubeBody;
	boundbox							m_bcBoundingCubeRhand;
	boundbox							m_bcBoundingCubeLhand;
	boundbox							m_bcBoundingCubeRleg;
	boundbox							m_bcBoundingCubeLleg;

private:
	int								m_nReferences;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	D3DXMATRIX						m_d3dxmtxLocal;
	D3DXMATRIX						m_d3dxmtxWorld;
	D3DXMATRIX						m_d3dxmtxboundmin;
	D3DXMATRIX						m_d3dxmtxboundmax;
	
protected:
	
	CGameObject 					*m_pChild;
	CGameObject 					*m_pSibling;
	CGameObject 					*m_pParent;
	
public:
	
	
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

	D3DXVECTOR3 GetMainboundingbox(bool type);
	D3DXVECTOR3 GetBoundingCubeHead(bool type);
	D3DXVECTOR3 GetBoundingCubeBody(bool type);
	D3DXVECTOR3 GetBoundingCubeRhand(bool type);
	D3DXVECTOR3 GetBoundingCubeLhand(bool type);
	D3DXVECTOR3 GetBoundingCubeRleg(bool type);
	D3DXVECTOR3 GetBoundingCubeLleg(bool type);


	void Update();

	
	//void GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection);
	//int PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo);
};

