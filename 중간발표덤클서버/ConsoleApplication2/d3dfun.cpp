



#include "d3dfun.h"
#include <stdio.h>


CGameObject::CGameObject(int nMeshes)
{
	D3DXMatrixIdentity(&m_d3dxmtxLocal);
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	D3DXMatrixIdentity(&m_d3dxmtxboundmin);
	D3DXMatrixIdentity(&m_d3dxmtxboundmax);

	m_bcBoundingCube.Min.x = -8.73088; m_bcBoundingCube.Min.y = -0.499931; m_bcBoundingCube.Min.z = -13.5503;
	m_bcBoundingCube.Max.x = 7.60868; m_bcBoundingCube.Max.y = 21.2649; m_bcBoundingCube.Max.z = 5.11578;


	m_bcBoundingCubeHead.Max = D3DXVECTOR3(m_bcBoundingCube.Max.x / 4, m_bcBoundingCube.Max.y, m_bcBoundingCube.Max.z / 4);
	m_bcBoundingCubeHead.Min = D3DXVECTOR3(m_bcBoundingCube.Min.x / 4, m_bcBoundingCube.Max.y / 1.25, m_bcBoundingCube.Min.z / 4);
	m_bcBoundingCubeBody.Max = D3DXVECTOR3(m_bcBoundingCube.Max.x / 2, m_bcBoundingCube.Max.y / 1.25, m_bcBoundingCube.Max.z / 2);
	m_bcBoundingCubeBody.Min = D3DXVECTOR3(m_bcBoundingCube.Min.x / 2, m_bcBoundingCube.Max.y / 2, m_bcBoundingCube.Min.z / 4);
	m_bcBoundingCubeLhand.Max = D3DXVECTOR3(m_bcBoundingCube.Min.x / 2, m_bcBoundingCube.Max.y, m_bcBoundingCube.Max.z);
	m_bcBoundingCubeLhand.Min = D3DXVECTOR3(m_bcBoundingCube.Min.x, m_bcBoundingCube.Min.y, m_bcBoundingCube.Min.z);
	m_bcBoundingCubeRhand.Max = D3DXVECTOR3(m_bcBoundingCube.Max.x, m_bcBoundingCube.Max.y, m_bcBoundingCube.Max.z / 2);
	m_bcBoundingCubeRhand.Min = D3DXVECTOR3(m_bcBoundingCube.Max.x / 2, m_bcBoundingCube.Min.y, m_bcBoundingCube.Min.z / 4);
	m_bcBoundingCubeRleg.Max = D3DXVECTOR3(m_bcBoundingCube.Max.x / 2, m_bcBoundingCube.Max.y / 2, m_bcBoundingCube.Max.z);
	m_bcBoundingCubeRleg.Min = D3DXVECTOR3(1, m_bcBoundingCube.Min.y, m_bcBoundingCube.Min.z / 4);
	m_bcBoundingCubeLleg.Max = D3DXVECTOR3(-1, m_bcBoundingCube.Max.y / 2, m_bcBoundingCube.Max.z);
	m_bcBoundingCubeLleg.Min = D3DXVECTOR3(m_bcBoundingCube.Min.x / 2, m_bcBoundingCube.Min.y, m_bcBoundingCube.Min.z / 4);

	m_nReferences = 0;
	m_pChild = m_pSibling = m_pParent = NULL;

}


CGameObject::~CGameObject()
{
	if (m_pSibling) delete m_pSibling;
	if (m_pChild) delete m_pChild;
}


void CGameObject::SetChild(CGameObject *pChild)
{
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
	if (pChild) pChild->m_pParent = this;
}


void CGameObject::SetPosition(float x, float y, float z)
{
	m_d3dxmtxLocal._41 = x;
	m_d3dxmtxLocal._42 = y;
	m_d3dxmtxLocal._43 = z;

	
	//temp.Min = m_bcBoundingCube.Min;

	//	if (!m_pParent) { m_d3dxmtxWorld._41 = x; m_d3dxmtxWorld._42 = y; m_d3dxmtxWorld._43 = z; }
}


void CGameObject::SetPosition(D3DXVECTOR3 d3dxvPosition)
{
	SetPosition(d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z);
}



void CGameObject::MoveStrafe(float fDistance)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvRight = GetRight();
	d3dxvPosition += fDistance * d3dxvRight;
	CGameObject::SetPosition(d3dxvPosition);
}

void CGameObject::MoveUp(float fDistance)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvUp = GetUp();
	d3dxvPosition += fDistance * d3dxvUp;
	CGameObject::SetPosition(d3dxvPosition);
}

void CGameObject::MoveForward(float fDistance)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvLookAt = GetLook();
	d3dxvPosition += fDistance * d3dxvLookAt;
	CGameObject::SetPosition(d3dxvPosition);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	
	
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(fYaw), (float)D3DXToRadian(fPitch), (float)D3DXToRadian(fRoll));
	m_d3dxmtxLocal = mtxRotate * m_d3dxmtxLocal;
	D3DXVec3TransformNormal(&temp.Min, &m_bcBoundingCube.Min, &m_d3dxmtxLocal);
	D3DXVec3TransformNormal(&temp.Max, &m_bcBoundingCube.Max, &m_d3dxmtxLocal);
	//	if (!m_pParent) m_d3dxmtxWorld = m_d3dxmtxLocal;
}

void CGameObject::Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle)
{
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationAxis(&mtxRotate, pd3dxvAxis, (float)D3DXToRadian(fAngle));
	m_d3dxmtxLocal = mtxRotate * m_d3dxmtxLocal;
	//	if (!m_pParent) m_d3dxmtxWorld = m_d3dxmtxLocal;
}

D3DXVECTOR3 CGameObject::GetPosition(bool bIsLocal)
{
	return((bIsLocal) ? D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) : D3DXVECTOR3(m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43));
}

D3DXVECTOR3 CGameObject::GetRight(bool bIsLocal)
{
	D3DXVECTOR3 d3dxvRight = (bIsLocal) ? D3DXVECTOR3(m_d3dxmtxLocal._11, m_d3dxmtxLocal._12, m_d3dxmtxLocal._13) : D3DXVECTOR3(m_d3dxmtxWorld._11, m_d3dxmtxWorld._12, m_d3dxmtxWorld._13);
	D3DXVec3Normalize(&d3dxvRight, &d3dxvRight);
	return(d3dxvRight);
}

D3DXVECTOR3 CGameObject::GetUp(bool bIsLocal)
{
	D3DXVECTOR3 d3dxvUp = (bIsLocal) ? D3DXVECTOR3(m_d3dxmtxLocal._21, m_d3dxmtxLocal._22, m_d3dxmtxLocal._23) : D3DXVECTOR3(m_d3dxmtxWorld._21, m_d3dxmtxWorld._22, m_d3dxmtxWorld._23);
	D3DXVec3Normalize(&d3dxvUp, &d3dxvUp);
	return(d3dxvUp);
}

D3DXVECTOR3 CGameObject::GetLook(bool bIsLocal)
{
	D3DXVECTOR3 d3dxvLookAt = (bIsLocal) ? D3DXVECTOR3(m_d3dxmtxLocal._31, m_d3dxmtxLocal._32, m_d3dxmtxLocal._33) : D3DXVECTOR3(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33);
	D3DXVec3Normalize(&d3dxvLookAt, &d3dxvLookAt);
	return(d3dxvLookAt);
}

void CGameObject::Update()
{
	
	
	
}

D3DXVECTOR3 CGameObject::GetMainboundingbox(bool type)
{

	Boundmain.Min = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + temp.Min;
	Boundmain.Max = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + temp.Max;

	D3DXVECTOR3 min, max;
	if (Boundmain.Min.x < Boundmain.Max.x)
	{
		min.x = Boundmain.Min.x;
		max.x = Boundmain.Max.x;
	}
	else
	{
		min.x = Boundmain.Max.x;
		max.x = Boundmain.Min.x;
	}
	min.y = Boundmain.Min.y;
	max.y = Boundmain.Max.y;
	if (Boundmain.Min.z < Boundmain.Max.z)
	{
		min.z = Boundmain.Min.z;
		max.z = Boundmain.Max.z;
	}
	else
	{
		min.z = Boundmain.Max.z;
		max.z = Boundmain.Min.z;

	}

	if (type == 0)
		return min;
	else
		return max;
}


	
D3DXVECTOR3 CGameObject::GetBoundingCubeHead(bool type)
{

	m_bcBoundingCubeHead.Max = D3DXVECTOR3(temp.Max.x / 4, temp.Max.y, temp.Max.z / 4);
	m_bcBoundingCubeHead.Min = D3DXVECTOR3(temp.Min.x / 4, temp.Max.y / 1.25, temp.Min.z/ 4);
	
	Boundmain.Min = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + m_bcBoundingCubeHead.Min;
	Boundmain.Max = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + m_bcBoundingCubeHead.Max;

	D3DXVECTOR3 min, max;
	if (Boundmain.Min.x < Boundmain.Max.x)
	{
		min.x = Boundmain.Min.x;
		max.x = Boundmain.Max.x;
	}
	else
	{
		min.x = Boundmain.Max.x;
		max.x = Boundmain.Min.x;
	}
	min.y = Boundmain.Min.y;
	max.y = Boundmain.Max.y;
	if (Boundmain.Min.z < Boundmain.Max.z)
	{
		min.z = Boundmain.Min.z;
		max.z = Boundmain.Max.z;
	}
	else
	{
		min.z = Boundmain.Max.z;
		max.z = Boundmain.Min.z;

	}

	if (type == 0)
		return min;
	else
		return max;



}

D3DXVECTOR3 CGameObject::GetBoundingCubeBody(bool type)
{


	m_bcBoundingCubeBody.Max = D3DXVECTOR3(temp.Max.x / 2, temp.Max.y / 1.25, temp.Max.z/ 2);
	m_bcBoundingCubeBody.Min = D3DXVECTOR3(temp.Min.x/ 2, temp.Max.y / 2, temp.Min.z/ 4);
	
	Boundmain.Min = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + m_bcBoundingCubeBody.Min;
	Boundmain.Max = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + m_bcBoundingCubeBody.Max;

	D3DXVECTOR3 min, max;
	if (Boundmain.Min.x < Boundmain.Max.x)
	{
		min.x = Boundmain.Min.x;
		max.x = Boundmain.Max.x;
	}
	else
	{
		min.x = Boundmain.Max.x;
		max.x = Boundmain.Min.x;
	}
	min.y = Boundmain.Min.y;
	max.y = Boundmain.Max.y;
	if (Boundmain.Min.z < Boundmain.Max.z)
	{
		min.z = Boundmain.Min.z;
		max.z = Boundmain.Max.z;
	}
	else
	{
		min.z = Boundmain.Max.z;
		max.z = Boundmain.Min.z;

	}

	if (type == 0)
		return min;
	else
		return max;

}

D3DXVECTOR3 CGameObject::GetBoundingCubeRhand(bool type)
{

	
	m_bcBoundingCubeRhand.Max = D3DXVECTOR3(temp.Max.x, temp.Max.y, temp.Max.z / 2);
	m_bcBoundingCubeRhand.Min = D3DXVECTOR3(temp.Max.x / 2, temp.Min.y, temp.Min.z / 4);

	Boundmain.Min = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + m_bcBoundingCubeRhand.Min;
	Boundmain.Max = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + m_bcBoundingCubeRhand.Max;

	D3DXVECTOR3 min, max;
	if (Boundmain.Min.x < Boundmain.Max.x)
	{
		min.x = Boundmain.Min.x;
		max.x = Boundmain.Max.x;
	}
	else
	{
		min.x = Boundmain.Max.x;
		max.x = Boundmain.Min.x;
	}
	min.y = Boundmain.Min.y;
	max.y = Boundmain.Max.y;
	if (Boundmain.Min.z < Boundmain.Max.z)
	{
		min.z = Boundmain.Min.z;
		max.z = Boundmain.Max.z;
	}
	else
	{
		min.z = Boundmain.Max.z;
		max.z = Boundmain.Min.z;

	}

	if (type == 0)
		return min;
	else
		return max;

}


D3DXVECTOR3 CGameObject::GetBoundingCubeLhand(bool type)
{


	m_bcBoundingCubeLhand.Max = D3DXVECTOR3(temp.Min.x / 2, temp.Max.y, temp.Max.z);
	m_bcBoundingCubeLhand.Min = D3DXVECTOR3(temp.Min.x, temp.Min.y, temp.Min.z);
	
	Boundmain.Min = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + m_bcBoundingCubeLhand.Min;
	Boundmain.Max = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + m_bcBoundingCubeLhand.Max;

	D3DXVECTOR3 min, max;
	if (Boundmain.Min.x < Boundmain.Max.x)
	{
		min.x = Boundmain.Min.x;
		max.x = Boundmain.Max.x;
	}
	else
	{
		min.x = Boundmain.Max.x;
		max.x = Boundmain.Min.x;
	}
	min.y = Boundmain.Min.y;
	max.y = Boundmain.Max.y;
	if (Boundmain.Min.z < Boundmain.Max.z)
	{
		min.z = Boundmain.Min.z;
		max.z = Boundmain.Max.z;
	}
	else
	{
		min.z = Boundmain.Max.z;
		max.z = Boundmain.Min.z;

	}

	if (type == 0)
		return min;
	else
		return max;

}
D3DXVECTOR3 CGameObject::GetBoundingCubeRleg(bool type)
{
	m_bcBoundingCubeRleg.Max = D3DXVECTOR3(temp.Max.x / 2, temp.Max.y / 2, temp.Max.z);
	m_bcBoundingCubeRleg.Min = D3DXVECTOR3(1, temp.Min.y, temp.Min.z / 4);
	
	Boundmain.Min = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + m_bcBoundingCubeRleg.Min;
	Boundmain.Max = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + m_bcBoundingCubeRleg.Max;

	D3DXVECTOR3 min, max;
	if (Boundmain.Min.x < Boundmain.Max.x)
	{
		min.x = Boundmain.Min.x;
		max.x = Boundmain.Max.x;
	}
	else
	{
		min.x = Boundmain.Max.x;
		max.x = Boundmain.Min.x;
	}
	min.y = Boundmain.Min.y;
	max.y = Boundmain.Max.y;
	if (Boundmain.Min.z < Boundmain.Max.z)
	{
		min.z = Boundmain.Min.z;
		max.z = Boundmain.Max.z;
	}
	else
	{
		min.z = Boundmain.Max.z;
		max.z = Boundmain.Min.z;

	}

	if (type == 0)
		return min;
	else
		return max;

}
D3DXVECTOR3 CGameObject::GetBoundingCubeLleg(bool type)
{

	
	m_bcBoundingCubeLleg.Max = D3DXVECTOR3(-1, temp.Max.y / 2, temp.Max.z);
	m_bcBoundingCubeLleg.Min = D3DXVECTOR3(temp.Min.x / 2, temp.Min.y, temp.Min.z / 4);
	Boundmain.Min = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + m_bcBoundingCubeLleg.Min;
	Boundmain.Max = D3DXVECTOR3(m_d3dxmtxLocal._41, m_d3dxmtxLocal._42, m_d3dxmtxLocal._43) + m_bcBoundingCubeLleg.Max;

	D3DXVECTOR3 min, max;
	if (Boundmain.Min.x < Boundmain.Max.x)
	{
		min.x = Boundmain.Min.x;
		max.x = Boundmain.Max.x;
	}
	else
	{
		min.x = Boundmain.Max.x;
		max.x = Boundmain.Min.x;
	}
	min.y = Boundmain.Min.y;
	max.y = Boundmain.Max.y;
	if (Boundmain.Min.z < Boundmain.Max.z)
	{
		min.z = Boundmain.Min.z;
		max.z = Boundmain.Max.z;
	}
	else
	{
		min.z = Boundmain.Max.z;
		max.z = Boundmain.Min.z;

	}

	if (type == 0)
		return min;
	else
		return max;

}

