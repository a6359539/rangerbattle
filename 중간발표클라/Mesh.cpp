//-----------------------------------------------------------------------------
// File: CMesh.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Mesh.h"
#include "Object.h"


#include<fstream>
#include<string>
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////
//
void AABB::Merge(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum)
{
	if (d3dxvMinimum.x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = d3dxvMinimum.x;
	if (d3dxvMinimum.y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = d3dxvMinimum.y;
	if (d3dxvMinimum.z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = d3dxvMinimum.z;
	if (d3dxvMaximum.x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = d3dxvMaximum.x;
	if (d3dxvMaximum.y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = d3dxvMaximum.y;
	if (d3dxvMaximum.z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = d3dxvMaximum.z;
}

void AABB::Merge(AABB *pAABB)
{
	Merge(pAABB->m_d3dxvMinimum, pAABB->m_d3dxvMaximum);
}

void AABB::Update(D3DXMATRIX *pmtxTransform)
{
	D3DXVECTOR3 vVertices[8];
	vVertices[0] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z);
	vVertices[1] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z);
	vVertices[2] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z);
	vVertices[3] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z);
	vVertices[4] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z);
	vVertices[5] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z);
	vVertices[6] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z);
	vVertices[7] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z);
	m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (int i = 0; i < 8; i++)
	{
		D3DXVec3TransformCoord(&vVertices[i], &vVertices[i], pmtxTransform);
		if (vVertices[i].x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = vVertices[i].x;
		if (vVertices[i].y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = vVertices[i].y;
		if (vVertices[i].z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = vVertices[i].z;
		if (vVertices[i].x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = vVertices[i].x;
		if (vVertices[i].y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = vVertices[i].y;
		if (vVertices[i].z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = vVertices[i].z;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_nType = VERTEX_POSITION_ELEMENT;

	m_nVertices = 0;
	m_nBuffers = 0;
	m_pd3dPositionBuffer = NULL;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nSlot = 0;
	m_nStartVertex = 0;

	m_pd3dIndexBuffer = NULL;
	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;
	m_nIndexOffset = 0;
	m_dxgiIndexFormat = DXGI_FORMAT_R32_UINT;

	m_pd3dxvPositions = NULL;
	m_pnIndices = NULL;

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	m_bcBoundingCubeHead.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCubeHead.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	m_bcBoundingCubeBody.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCubeBody.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	m_bcBoundingCubeRhand.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCubeRhand.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	m_bcBoundingCubeLhand.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCubeLhand.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	m_bcBoundingCubeRleg.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCubeRleg.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	m_bcBoundingCubeLleg.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCubeLleg.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	m_nReferences = 0;
}

CMesh::~CMesh()
{
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();

	if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
	if (m_pnVertexStrides) delete[] m_pnVertexStrides;
	if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;

	if (m_pd3dxvPositions) delete[] m_pd3dxvPositions;
	if (m_pnIndices) delete[] m_pnIndices;
}

ID3D11Buffer *CMesh::CreateBuffer(ID3D11Device *pd3dDevice, UINT nStride, int nElements, void *pBufferData, UINT nBindFlags, D3D11_USAGE d3dUsage, UINT nCPUAccessFlags)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = d3dUsage;
	d3dBufferDesc.ByteWidth = nStride * nElements;
	d3dBufferDesc.BindFlags = nBindFlags;
	d3dBufferDesc.CPUAccessFlags = nCPUAccessFlags;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pBufferData;

	ID3D11Buffer *pd3dBuffer = NULL;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &pd3dBuffer);
	return(pd3dBuffer);
}

ID3D11Buffer * CMesh::CreateBoundingBuffer(ID3D11Device * pd3dDevice, UINT nStride, int nElements, void * pBufferData, UINT nBindFlags, D3D11_USAGE d3dUsage, UINT nCPUAccessFlags)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = d3dUsage;
	d3dBufferDesc.ByteWidth = nStride * nElements;
	d3dBufferDesc.BindFlags = nBindFlags;
	d3dBufferDesc.CPUAccessFlags = nCPUAccessFlags;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pBufferData;

	ID3D11Buffer *pd3dBuffer = NULL;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &pd3dBuffer);
	return(pd3dBuffer);
}

void CMesh::AssembleToVertexBuffer(int nBuffers, ID3D11Buffer **ppd3dBuffers, UINT *pnBufferStrides, UINT *pnBufferOffsets)
{
	ID3D11Buffer **ppd3dNewVertexBuffers = new ID3D11Buffer*[m_nBuffers + nBuffers];
	UINT *pnNewVertexStrides = new UINT[m_nBuffers + nBuffers];
	UINT *pnNewVertexOffsets = new UINT[m_nBuffers + nBuffers];

	if (m_nBuffers > 0)
	{
		for (int i = 0; i < m_nBuffers; i++)
		{
			ppd3dNewVertexBuffers[i] = m_ppd3dVertexBuffers[i];
			pnNewVertexStrides[i] = m_pnVertexStrides[i];
			pnNewVertexOffsets[i] = m_pnVertexOffsets[i];
		}
		if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
		if (m_pnVertexStrides) delete[] m_pnVertexStrides;
		if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;
	}

	for (int i = 0; i < nBuffers; i++)
	{
		ppd3dNewVertexBuffers[m_nBuffers + i] = ppd3dBuffers[i];
		pnNewVertexStrides[m_nBuffers + i] = pnBufferStrides[i];
		pnNewVertexOffsets[m_nBuffers + i] = pnBufferOffsets[i];
	}

	m_nBuffers += nBuffers;
	m_ppd3dVertexBuffers = ppd3dNewVertexBuffers;
	m_pnVertexStrides = pnNewVertexStrides;
	m_pnVertexOffsets = pnNewVertexOffsets;
}

void CMesh::OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
}

void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	OnPrepareRender(pd3dDeviceContext);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);
}

void CMesh::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	OnPrepareRender(pd3dDeviceContext);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, nInstances, m_nStartIndex, m_nBaseVertex, nStartInstance);
	else
		pd3dDeviceContext->DrawInstanced(m_nVertices, nInstances, m_nStartVertex, nStartInstance);
}

bool RayIntersectTriangle(D3DXVECTOR3 *pd3dxvOrigin, D3DXVECTOR3 *pd3dxvDirection, D3DXVECTOR3 *pd3dxvP0, D3DXVECTOR3 *pd3dxvP1, D3DXVECTOR3 *pd3dxvP2, float *pfU, float *pfV, float *pfRayToTriangle)
{
	D3DXVECTOR3 d3dxvEdge1 = *pd3dxvP1 - *pd3dxvP0;
	D3DXVECTOR3 d3dxvEdge2 = *pd3dxvP2 - *pd3dxvP0;
	D3DXVECTOR3 d3dxvP, d3dxvQ;
	D3DXVec3Cross(&d3dxvP, pd3dxvDirection, &d3dxvEdge2);
	float a = D3DXVec3Dot(&d3dxvEdge1, &d3dxvP);
	if (::IsZero(a)) return(false);
	float f = 1.0f / a;
	D3DXVECTOR3 d3dxvP0ToOrigin = *pd3dxvOrigin - *pd3dxvP0;
	*pfU = f * D3DXVec3Dot(&d3dxvP0ToOrigin, &d3dxvP);
	if ((*pfU < 0.0f) || (*pfU > 1.0f)) return(false);
	D3DXVec3Cross(&d3dxvQ, &d3dxvP0ToOrigin, &d3dxvEdge1);
	*pfV = f * D3DXVec3Dot(pd3dxvDirection, &d3dxvQ);
	if ((*pfV < 0.0f) || ((*pfU + *pfV) > 1.0f)) return(false);
	*pfRayToTriangle = f * D3DXVec3Dot(&d3dxvEdge2, &d3dxvQ);
	return(*pfRayToTriangle >= 0.0f);
}

#define _WITH_D3DX_LIBRARY

int CMesh::CheckRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	int nIntersections = 0;
	BYTE *pbPositions = (BYTE *)m_pd3dxvPositions + m_pnVertexOffsets[0];

	int nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	int nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	D3DXVECTOR3 v0, v1, v2;
	float fuHitBaryCentric, fvHitBaryCentric, fHitDistance, fNearHitDistance = FLT_MAX;
	for (int i = 0; i < nPrimitives; i++)
	{
		v0 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 0]) : ((i*nOffset) + 0)) * m_pnVertexStrides[0]);
		v1 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 1]) : ((i*nOffset) + 1)) * m_pnVertexStrides[0]);
		v2 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 2]) : ((i*nOffset) + 2)) * m_pnVertexStrides[0]);
#ifdef _WITH_D3DX_LIBRARY
		if (D3DXIntersectTri(&v0, &v1, &v2, pd3dxvRayPosition, pd3dxvRayDirection, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
#else
		if (::RayIntersectTriangle(pd3dxvRayPosition, pd3dxvRayDirection, &v0, &v1, &v2, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
#endif
		{
			if (fHitDistance < fNearHitDistance)
			{
				fNearHitDistance = fHitDistance;
				if (pd3dxIntersectInfo)
				{
					pd3dxIntersectInfo->m_dwFaceIndex = i;
					pd3dxIntersectInfo->m_fU = fuHitBaryCentric;
					pd3dxIntersectInfo->m_fV = fvHitBaryCentric;
					pd3dxIntersectInfo->m_fDistance = fHitDistance;
				}
			}
			nIntersections++;
		}
	}
	return(nIntersections);
}

void CMesh::CalculateBoundingCube()
{
	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (int i = 0; i < m_nVertices; i++)
	{
		if (m_pd3dxvPositions[i].x < m_bcBoundingCube.m_d3dxvMinimum.x) m_bcBoundingCube.m_d3dxvMinimum.x = m_pd3dxvPositions[i].x;
		if (m_pd3dxvPositions[i].x > m_bcBoundingCube.m_d3dxvMaximum.x) m_bcBoundingCube.m_d3dxvMaximum.x = m_pd3dxvPositions[i].x;
		if (m_pd3dxvPositions[i].y < m_bcBoundingCube.m_d3dxvMinimum.y) m_bcBoundingCube.m_d3dxvMinimum.y = m_pd3dxvPositions[i].y;
		if (m_pd3dxvPositions[i].y > m_bcBoundingCube.m_d3dxvMaximum.y) m_bcBoundingCube.m_d3dxvMaximum.y = m_pd3dxvPositions[i].y;
		if (m_pd3dxvPositions[i].z < m_bcBoundingCube.m_d3dxvMinimum.z) m_bcBoundingCube.m_d3dxvMinimum.z = m_pd3dxvPositions[i].z;
		if (m_pd3dxvPositions[i].z > m_bcBoundingCube.m_d3dxvMaximum.z) m_bcBoundingCube.m_d3dxvMaximum.z = m_pd3dxvPositions[i].z;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CMeshDiffused::CMeshDiffused(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_nType |= VERTEX_COLOR_ELEMENT;
	m_pd3dColorBuffer = NULL;
}

CMeshDiffused::~CMeshDiffused()
{
	if (m_pd3dColorBuffer) m_pd3dColorBuffer->Release();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CMeshIlluminated::CMeshIlluminated(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_nType |= VERTEX_NORMAL_ELEMENT;
	m_pd3dNormalBuffer = NULL;
}

CMeshIlluminated::~CMeshIlluminated()
{
	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
}

D3DXVECTOR3 CMeshIlluminated::CalculateTriAngleNormal(UINT nIndex0, UINT nIndex1, UINT nIndex2)
{
	D3DXVECTOR3 d3dxvNormal(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 d3dxvP0 = m_pd3dxvPositions[nIndex0];
	D3DXVECTOR3 d3dxvP1 = m_pd3dxvPositions[nIndex1];
	D3DXVECTOR3 d3dxvP2 = m_pd3dxvPositions[nIndex2];
	D3DXVECTOR3 d3dxvEdge1 = d3dxvP1 - d3dxvP0;
	D3DXVECTOR3 d3dxvEdge2 = d3dxvP2 - d3dxvP0;
	D3DXVec3Cross(&d3dxvNormal, &d3dxvEdge1, &d3dxvEdge2);
	D3DXVec3Normalize(&d3dxvNormal, &d3dxvNormal);
	return(d3dxvNormal);
}

void CMeshIlluminated::SetTriAngleListVertexNormal(D3DXVECTOR3 *pd3dxvNormals)
{
	D3DXVECTOR3 d3dxvNormal;
	D3DXVECTOR3 *pd3dxvPositions = NULL;
	int nPrimitives = m_nVertices / 3;
	for (int i = 0; i < nPrimitives; i++)
	{
		d3dxvNormal = CalculateTriAngleNormal((i * 3 + 0), (i * 3 + 1), (i * 3 + 2));
		pd3dxvPositions = m_pd3dxvPositions + (i * 3 + 0);
		pd3dxvNormals[i * 3 + 0] = d3dxvNormal;
		pd3dxvPositions = m_pd3dxvPositions + (i * 3 + 1);
		pd3dxvNormals[i * 3 + 1] = d3dxvNormal;
		pd3dxvPositions = m_pd3dxvPositions + (i * 3 + 2);
		pd3dxvNormals[i * 3 + 2] = d3dxvNormal;
	}
}

void CMeshIlluminated::SetAverageVertexNormal(D3DXVECTOR3 *pd3dxvNormals, int nPrimitives, int nOffset, bool bStrip)
{
	D3DXVECTOR3 d3dxvSumOfNormal(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 *pd3dxvPositions = NULL;
	UINT nIndex0, nIndex1, nIndex2;

	for (int j = 0; j < m_nVertices; j++)
	{
		d3dxvSumOfNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < nPrimitives; i++)
		{
			nIndex0 = (bStrip) ? (((i % 2) == 0) ? (i*nOffset + 0) : (i*nOffset + 1)) : (i*nOffset + 0);
			if (m_pnIndices) nIndex0 = m_pnIndices[nIndex0];
			nIndex1 = (bStrip) ? (((i % 2) == 0) ? (i*nOffset + 1) : (i*nOffset + 0)) : (i*nOffset + 1);
			if (m_pnIndices) nIndex1 = m_pnIndices[nIndex1];
			nIndex2 = (m_pnIndices) ? m_pnIndices[i*nOffset + 2] : (i*nOffset + 2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)) d3dxvSumOfNormal += CalculateTriAngleNormal(nIndex0, nIndex1, nIndex2);
		}
		D3DXVec3Normalize(&d3dxvSumOfNormal, &d3dxvSumOfNormal);
		pd3dxvNormals[j] = d3dxvSumOfNormal;
		pd3dxvPositions = m_pd3dxvPositions + j;
	}
}

void CMeshIlluminated::CalculateVertexNormal(D3DXVECTOR3 *pd3dxvNormals)
{
	switch (m_d3dPrimitiveTopology)
	{
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (!m_pnIndices)
			SetTriAngleListVertexNormal(pd3dxvNormals);
		else
			SetAverageVertexNormal(pd3dxvNormals, (m_nIndices / 3), 3, false);
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		SetAverageVertexNormal(pd3dxvNormals, (m_pnIndices) ? (m_nIndices - 2) : (m_nVertices - 2), 1, true);
		break;
	default:
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CMeshTextured::CMeshTextured(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_nType |= VERTEX_TEXTURE_ELEMENT_0;
	m_pd3dTexCoordBuffer = NULL;
}

CMeshTextured::~CMeshTextured()
{
	if (m_pd3dTexCoordBuffer) m_pd3dTexCoordBuffer->Release();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CMeshDetailTextured::CMeshDetailTextured(ID3D11Device *pd3dDevice) : CMeshTextured(pd3dDevice)
{
	m_nType |= VERTEX_TEXTURE_ELEMENT_1;
	m_pd3dDetailTexCoordBuffer = NULL;
}

CMeshDetailTextured::~CMeshDetailTextured()
{
	if (m_pd3dDetailTexCoordBuffer) m_pd3dDetailTexCoordBuffer->Release();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CMeshTexturedIlluminated::CMeshTexturedIlluminated(ID3D11Device *pd3dDevice) : CMeshIlluminated(pd3dDevice)
{
	m_nType |= VERTEX_TEXTURE_ELEMENT_0;
	m_pd3dTexCoordBuffer = NULL;
}

CMeshTexturedIlluminated::~CMeshTexturedIlluminated()
{
	if (m_pd3dTexCoordBuffer) m_pd3dTexCoordBuffer->Release();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CMeshDetailTexturedIlluminated::CMeshDetailTexturedIlluminated(ID3D11Device *pd3dDevice) : CMeshIlluminated(pd3dDevice)
{
	m_nType |= (VERTEX_TEXTURE_ELEMENT_0 | VERTEX_TEXTURE_ELEMENT_1);
	m_pd3dTexCoordBuffer = NULL;
	m_pd3dDetailTexCoordBuffer = NULL;
}

CMeshDetailTexturedIlluminated::~CMeshDetailTexturedIlluminated()
{
	if (m_pd3dTexCoordBuffer) m_pd3dTexCoordBuffer->Release();
	if (m_pd3dDetailTexCoordBuffer) m_pd3dDetailTexCoordBuffer->Release();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxColor) : CMeshDiffused(pd3dDevice)
{
	m_nVertices = 24 * 3;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];

	float x1 = fx * 0.2f, z1 = fz * 0.2f, x2 = fx * 0.1f, z3 = fz * 0.3f, z2 = ((z1 - (fz - z3)) / x1) * x2 + (fz - z3);
	int i = 0;
	//Upper Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +fy, +(fz + z3));
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, +fy, -z1);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +fy, 0.0f);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +fy, +(fz + z3));
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +fy, 0.0f);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, +fy, -z1);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +fy, +z2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -z3);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, +fy, -z1);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +fy, +z2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, +fy, -z1);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, -z3);

	//Lower Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, -fy, +(fz + z3));
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, -fy, 0.0f);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -fy, -z1);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, -fy, +(fz + z3));
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -fy, -z1);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, -fy, 0.0f);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, -fy, +z2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -fy, -z1);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, -z3);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, -fy, +z2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -z3);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -fy, -z1);

	//Right Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +fy, +(fz + z3));
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, -fy, +(fz + z3));
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +fy, +z2);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +fy, +z2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, -fy, +(fz + z3));
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, -fy, +z2);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +fy, +z2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, -fy, +z2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -z3);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -z3);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, -fy, +z2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, -z3);

	//Back/Right Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, +fy, -z1);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -z3);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, -z3);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, +fy, -z1);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, -z3);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -fy, -z1);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +fy, 0.0f);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, +fy, -z1);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -fy, -z1);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +fy, 0.0f);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -fy, -z1);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, -fy, 0.0f);

	//Left Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, -fy, +(fz + z3));
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +fy, +(fz + z3));
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +fy, +z2);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, -fy, +(fz + z3));
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +fy, +z2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, -fy, +z2);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, -fy, +z2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +fy, +z2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, -z3);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, -fy, +z2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, -z3);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -z3);

	//Back/Left Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +fy, 0.0f);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, -fy, 0.0f);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -fy, -z1);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +fy, 0.0f);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -fy, -z1);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, +fy, -z1);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, +fy, -z1);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -fy, -z1);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -z3);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, +fy, -z1);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -z3);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, -z3);

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	D3DXCOLOR pd3dxColors[24 * 3];
	for (int j = 0; j < m_nVertices; j++) pd3dxColors[j] = d3dxColor + RANDOM_COLOR;
	pd3dxColors[0] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[3] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[6] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[9] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[12] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[15] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[18] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[21] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

	m_pd3dColorBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXCOLOR), m_nVertices, pd3dxColors, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	ID3D11Buffer *ppd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, ppd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CAirplaneMeshDiffused::~CAirplaneMeshDiffused()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMeshDiffused::CCubeMeshDiffused(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxColor) : CMeshDiffused(pd3dDevice)
{
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];

	m_pd3dxvPositions[0] = D3DXVECTOR3(-fx, +fy, -fz);
	m_pd3dxvPositions[1] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[2] = D3DXVECTOR3(+fx, +fy, +fz);
	m_pd3dxvPositions[3] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[4] = D3DXVECTOR3(-fx, -fy, -fz);
	m_pd3dxvPositions[5] = D3DXVECTOR3(+fx, -fy, -fz);
	m_pd3dxvPositions[6] = D3DXVECTOR3(+fx, -fy, +fz);
	m_pd3dxvPositions[7] = D3DXVECTOR3(-fx, -fy, +fz);

	D3DXCOLOR pd3dxColors[8];
	for (int i = 0; i < 8; i++) pd3dxColors[i] = d3dxColor + RANDOM_COLOR;

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dColorBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXCOLOR), m_nVertices, pd3dxColors, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

#define _WITH_CORRECT_STRIP
#ifdef _WITH_CORRECT_STRIP
	m_nIndices = 18;
	m_pnIndices = new UINT[m_nIndices];

	m_pnIndices[0] = 5; //5,6,4 - cw
	m_pnIndices[1] = 6; //6,4,7 - ccw
	m_pnIndices[2] = 4; //4,7,0 - cw
	m_pnIndices[3] = 7; //7,0,3 - ccw
	m_pnIndices[4] = 0; //0,3,1 - cw
	m_pnIndices[5] = 3; //3,1,2 - ccw
	m_pnIndices[6] = 1; //1,2,2 - cw 
	m_pnIndices[7] = 2; //2,2,3 - ccw
	m_pnIndices[8] = 2; //2,3,3 - cw  - Degenerated Index
	m_pnIndices[9] = 3; //3,3,7 - ccw - Degenerated Index
	m_pnIndices[10] = 3;//3,7,2 - cw  - Degenerated Index
	m_pnIndices[11] = 7;//7,2,6 - ccw
	m_pnIndices[12] = 2;//2,6,1 - cw
	m_pnIndices[13] = 6;//6,1,5 - ccw
	m_pnIndices[14] = 1;//1,5,0 - cw
	m_pnIndices[15] = 5;//5,0,4 - ccw
	m_pnIndices[16] = 0;
	m_pnIndices[17] = 4;
#else
	m_nIndices = 16;
	m_pnIndices = new UINT[m_nIndices];

	m_pnIndices[0] = 5; //5,6,4 - cw
	m_pnIndices[1] = 6; //6,4,7 - ccw
	m_pnIndices[2] = 4; //4,7,0 - cw
	m_pnIndices[3] = 7; //7,0,3 - ccw
	m_pnIndices[4] = 0; //0,3,1 - cw
	m_pnIndices[5] = 3; //3,1,2 - ccw
	m_pnIndices[6] = 1; //1,2,3 - cw 
	m_pnIndices[7] = 2; //2,3,7 - ccw
	m_pnIndices[8] = 3; //3,7,2 - cw - Degenerated Index
	m_pnIndices[9] = 7; //7,2,6 - ccw
	m_pnIndices[10] = 2;//2,6,1 - cw
	m_pnIndices[11] = 6;//6,1,5 - ccw
	m_pnIndices[12] = 1;//1,5,0 - cw
	m_pnIndices[13] = 5;//5,0,4 - ccw
	m_pnIndices[14] = 0;
	m_pnIndices[15] = 4;
#endif

	m_pd3dIndexBuffer = CreateBuffer(pd3dDevice, sizeof(UINT), m_nIndices, m_pnIndices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CSphereMeshDiffused::CSphereMeshDiffused(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks, D3DXCOLOR d3dxColor) : CMeshDiffused(pd3dDevice)
{
	m_nVertices = (nSlices * nStacks) * 3 * 2;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];

	float theta_i, theta_ii, phi_j, phi_jj, fRadius_j, fRadius_jj, y_j, y_jj;
	for (int j = 0, k = 0; j < nStacks; j++)
	{
		phi_j = float(D3DX_PI / nStacks) * j;
		phi_jj = float(D3DX_PI / nStacks) * (j + 1);
		fRadius_j = fRadius * sinf(phi_j);
		fRadius_jj = fRadius * sinf(phi_jj);
		y_j = fRadius*cosf(phi_j);
		y_jj = fRadius*cosf(phi_jj);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = float(2 * D3DX_PI / nSlices) * i;
			theta_ii = float(2 * D3DX_PI / nSlices) * (i + 1);
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_i), y_j, fRadius_j*sinf(theta_i));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_ii), y_jj, fRadius_jj*sinf(theta_ii));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
		}
	}

	D3DXCOLOR *pd3dxColors = new D3DXCOLOR[m_nVertices];
	for (int i = 0; i < m_nVertices; i++) pd3dxColors[i] = d3dxColor + RANDOM_COLOR;

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dColorBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXCOLOR), m_nVertices, pd3dxColors, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	delete[] pd3dxColors;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fRadius, -fRadius, -fRadius);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fRadius, +fRadius, +fRadius);
}

CSphereMeshDiffused::~CSphereMeshDiffused()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMeshIlluminated::CCubeMeshIlluminated(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshIlluminated(pd3dDevice)
{
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];

	m_pd3dxvPositions[0] = D3DXVECTOR3(-fx, +fy, -fz);
	m_pd3dxvPositions[1] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[2] = D3DXVECTOR3(+fx, +fy, +fz);
	m_pd3dxvPositions[3] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[4] = D3DXVECTOR3(-fx, -fy, -fz);
	m_pd3dxvPositions[5] = D3DXVECTOR3(+fx, -fy, -fz);
	m_pd3dxvPositions[6] = D3DXVECTOR3(+fx, -fy, +fz);
	m_pd3dxvPositions[7] = D3DXVECTOR3(-fx, -fy, +fz);

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	D3DXVECTOR3 pd3dxvNormals[8];
	for (int i = 0; i < 8; i++) pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_nIndices = 36;
	m_pnIndices = new UINT[m_nIndices];

	m_pnIndices[0] = 3; m_pnIndices[1] = 1; m_pnIndices[2] = 0;
	m_pnIndices[3] = 2; m_pnIndices[4] = 1; m_pnIndices[5] = 3;
	m_pnIndices[6] = 0; m_pnIndices[7] = 5; m_pnIndices[8] = 4;
	m_pnIndices[9] = 1; m_pnIndices[10] = 5; m_pnIndices[11] = 0;
	m_pnIndices[12] = 3; m_pnIndices[13] = 4; m_pnIndices[14] = 7;
	m_pnIndices[15] = 0; m_pnIndices[16] = 4; m_pnIndices[17] = 3;
	m_pnIndices[18] = 1; m_pnIndices[19] = 6; m_pnIndices[20] = 5;
	m_pnIndices[21] = 2; m_pnIndices[22] = 6; m_pnIndices[23] = 1;
	m_pnIndices[24] = 2; m_pnIndices[25] = 7; m_pnIndices[26] = 6;
	m_pnIndices[27] = 3; m_pnIndices[28] = 7; m_pnIndices[29] = 2;
	m_pnIndices[30] = 6; m_pnIndices[31] = 4; m_pnIndices[32] = 5;
	m_pnIndices[33] = 7; m_pnIndices[34] = 4; m_pnIndices[35] = 6;

	CalculateVertexNormal(pd3dxvNormals);

	m_pd3dNormalBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, pd3dxvNormals, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_pd3dIndexBuffer = CreateBuffer(pd3dDevice, sizeof(UINT), m_nIndices, m_pnIndices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CCubeMeshIlluminated::~CCubeMeshIlluminated()
{
}

#define _WITH_INDEX_BUFFER
/////////////////////////////////////////////////////////////////////////////////////////////////
//
CSphereMeshIlluminated::CSphereMeshIlluminated(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks) : CMeshIlluminated(pd3dDevice)
{
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fDeltaPhi = float(D3DX_PI / nStacks);
	float fDeltaTheta = float((2.0f * D3DX_PI) / nSlices);
	int k = 0;

#ifdef _WITH_INDEX_BUFFER
	m_nVertices = 2 + (nSlices * (nStacks - 1));
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];

	m_pd3dxvPositions[k] = D3DXVECTOR3(0.0f, +fRadius, 0.0f);
	D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
	float theta_i, phi_j;
	for (int j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*sinf(phi_j)*cosf(theta_i), fRadius*cosf(phi_j), fRadius*sinf(phi_j)*sinf(theta_i));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		}
	}
	m_pd3dxvPositions[k] = D3DXVECTOR3(0.0f, -fRadius, 0.0f);
	D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dNormalBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, pd3dxvNormals, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	if (pd3dxvNormals) delete[] pd3dxvNormals;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	k = 0;
	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pnIndices = new UINT[m_nIndices];
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = 0;
		m_pnIndices[k++] = 1 + ((i + 1) % nSlices);
		m_pnIndices[k++] = 1 + i;
	}
	for (int j = 0; j < nStacks - 2; j++)
	{
		for (int i = 0; i < nSlices; i++)
		{
			m_pnIndices[k++] = 1 + (i + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = (m_nVertices - 1);
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}

	m_pd3dIndexBuffer = CreateBuffer(pd3dDevice, sizeof(UINT), m_nIndices, m_pnIndices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
#else
	m_nVertices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];

	float theta_i, theta_ii, phi_j = 0.0f, phi_jj = fDeltaPhi;
	for (int i = 0; i < nSlices; i++)
	{
		theta_i = fDeltaTheta * i;
		theta_ii = fDeltaTheta * (i + 1);
		m_pd3dxvPositions[k] = D3DXVECTOR3(0.0f, +fRadius, 0.0f);
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_ii)*sinf(phi_jj));
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
	}

	for (int j = 1; j < nStacks - 1; j++)
	{
		phi_j = fDeltaPhi * j;
		phi_jj = fDeltaPhi * (j + 1);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			theta_ii = fDeltaTheta * (i + 1);
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_i)*sinf(phi_j));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_ii)*sinf(phi_jj));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		}
	}
	phi_j = fDeltaPhi * (nStacks - 1);
	for (int i = 0; i < nSlices; i++)
	{
		theta_i = fDeltaTheta * i;
		theta_ii = fDeltaTheta * (i + 1);
		m_pd3dxvPositions[k] = D3DXVECTOR3(0.0f, -fRadius, 0.0f);
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_i)*sinf(phi_j));
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
	}

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dNormalBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, pd3dxvNormals, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	if (pd3dxvNormals) delete[] pd3dxvNormals;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
#endif

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fRadius, -fRadius, -fRadius);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fRadius, +fRadius, +fRadius);
}

CSphereMeshIlluminated::~CSphereMeshIlluminated()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMeshTextured::CCubeMeshTextured(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshTextured(pd3dDevice)
{
	m_nVertices = 36;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	int i = 0;
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, -fz);

	D3DXVECTOR2 pd3dxvTexCoords[36];
	i = 0;
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, pd3dxvTexCoords, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CCubeMeshTextured::~CCubeMeshTextured()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CSphereMeshTextured::CSphereMeshTextured(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks) : CMeshTextured(pd3dDevice)
{
	m_nVertices = (nSlices * nStacks) * 3 * 2;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];

	float theta_i, theta_ii, phi_j, phi_jj, fRadius_j, fRadius_jj, y_j, y_jj;
	for (int j = 0, k = 0; j < nStacks; j++)
	{
		phi_j = float(D3DX_PI / nStacks) * j;
		phi_jj = float(D3DX_PI / nStacks) * (j + 1);
		fRadius_j = fRadius * sinf(phi_j);
		fRadius_jj = fRadius * sinf(phi_jj);
		y_j = fRadius*cosf(phi_j);
		y_jj = fRadius*cosf(phi_jj);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = float(2 * D3DX_PI / nSlices) * i;
			theta_ii = float(2 * D3DX_PI / nSlices) * (i + 1);
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_j*cosf(theta_i), y_j, fRadius_j*sinf(theta_i));
			pd3dxvTexCoords[k++] = D3DXVECTOR2(float(i) / float(nSlices), float(j) / float(nStacks));
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			pd3dxvTexCoords[k++] = D3DXVECTOR2(float(i) / float(nSlices), float(j + 1) / float(nStacks));
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
			pd3dxvTexCoords[k++] = D3DXVECTOR2(float(i + 1) / float(nSlices), float(j) / float(nStacks));
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			pd3dxvTexCoords[k++] = D3DXVECTOR2(float(i) / float(nSlices), float(j + 1) / float(nStacks));
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_jj*cosf(theta_ii), y_jj, fRadius_jj*sinf(theta_ii));
			pd3dxvTexCoords[k++] = D3DXVECTOR2(float(i + 1) / float(nSlices), float(j + 1) / float(nStacks));
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
			pd3dxvTexCoords[k++] = D3DXVECTOR2(float(i + 1) / float(nSlices), float(j) / float(nStacks));
		}
	}

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, pd3dxvTexCoords, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	delete[] pd3dxvTexCoords;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fRadius, -fRadius, -fRadius);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fRadius, +fRadius, +fRadius);
}

CSphereMeshTextured::~CSphereMeshTextured()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMeshTexturedIlluminated::CCubeMeshTexturedIlluminated(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshTexturedIlluminated(pd3dDevice)
{
	m_nVertices = 36;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	int i = 0;
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -fy, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -fy, -fz);

	D3DXVECTOR2 pd3dxvTexCoords[36];
	i = 0;
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	D3DXVECTOR3 pd3dxvNormals[36];
	//	for (int i = 0; i < 36; i++) pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	CalculateVertexNormal(pd3dxvNormals);

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dNormalBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, pd3dxvNormals, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, pd3dxvTexCoords, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	ID3D11Buffer *pd3dBuffers[3] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[3] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[3] = { 0, 0, 0 };
	AssembleToVertexBuffer(3, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CCubeMeshTexturedIlluminated::~CCubeMeshTexturedIlluminated()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CSphereMeshTexturedIlluminated::CSphereMeshTexturedIlluminated(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks) : CMeshTexturedIlluminated(pd3dDevice)
{
	m_nVertices = (nSlices * nStacks) * 3 * 2;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];

	float theta_i, theta_ii, phi_j, phi_jj, fRadius_j, fRadius_jj, y_j, y_jj;
	for (int j = 0, k = 0; j < nStacks; j++)
	{
		phi_j = float(D3DX_PI / nStacks) * j;
		phi_jj = float(D3DX_PI / nStacks) * (j + 1);
		fRadius_j = fRadius * sinf(phi_j);
		fRadius_jj = fRadius * sinf(phi_jj);
		y_j = fRadius*cosf(phi_j);
		y_jj = fRadius*cosf(phi_jj);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = float(2 * D3DX_PI / nSlices) * i;
			theta_ii = float(2 * D3DX_PI / nSlices) * (i + 1);
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_j*cosf(theta_i), y_j, fRadius_j*sinf(theta_i));
			pd3dxvTexCoords[k] = D3DXVECTOR2(float(i) / float(nSlices), float(j) / float(nStacks));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			pd3dxvTexCoords[k] = D3DXVECTOR2(float(i) / float(nSlices), float(j + 1) / float(nStacks));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
			pd3dxvTexCoords[k] = D3DXVECTOR2(float(i + 1) / float(nSlices), float(j) / float(nStacks));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			pd3dxvTexCoords[k] = D3DXVECTOR2(float(i) / float(nSlices), float(j + 1) / float(nStacks));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_jj*cosf(theta_ii), y_jj, fRadius_jj*sinf(theta_ii));
			pd3dxvTexCoords[k] = D3DXVECTOR2(float(i + 1) / float(nSlices), float(j + 1) / float(nStacks));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
			pd3dxvTexCoords[k] = D3DXVECTOR2(float(i + 1) / float(nSlices), float(j) / float(nStacks));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		}
	}

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dNormalBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, pd3dxvNormals, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, pd3dxvTexCoords, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	delete[] pd3dxvNormals;
	delete[] pd3dxvTexCoords;

	ID3D11Buffer *pd3dBuffers[3] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[3] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[3] = { 0, 0, 0 };
	AssembleToVertexBuffer(3, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fRadius, -fRadius, -fRadius);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fRadius, +fRadius, +fRadius);
}

CSphereMeshTexturedIlluminated::~CSphereMeshTexturedIlluminated()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHeightMapGridMesh
CHeightMapGridMesh::CHeightMapGridMesh(ID3D11Device *pd3dDevice, int xStart, int zStart, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale, void *pContext, D3D11_USAGE d3dUsage) : CMeshDetailTexturedIlluminated(pd3dDevice)
{
	m_nVertices = nWidth * nLength;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];
	D3DXVECTOR2 *pd3dxvDetailTexCoords = new D3DXVECTOR2[m_nVertices];

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_d3dxvScale = d3dxvScale;

	CHeightMap *pHeightMap = (CHeightMap *)pContext;
	int cxHeightMap = (pHeightMap) ? pHeightMap->GetHeightMapWidth() : nWidth;
	int czHeightMap = (pHeightMap) ? pHeightMap->GetHeightMapLength() : nLength;
	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			fHeight = OnGetHeight(x, z, pContext);
			m_pd3dxvPositions[i] = D3DXVECTOR3((x*m_d3dxvScale.x), fHeight, (z*m_d3dxvScale.z));
			pd3dxvNormals[i] = (pHeightMap) ? pHeightMap->GetHeightMapNormal(x, z) : D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			pd3dxvTexCoords[i] = D3DXVECTOR2(float(x) / float(cxHeightMap - 1), float(czHeightMap - 1 - z) / float(czHeightMap - 1));
			pd3dxvDetailTexCoords[i] = D3DXVECTOR2(float(x) / float(m_d3dxvScale.x*0.125f), float(z) / float(m_d3dxvScale.z*0.125f));
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, d3dUsage, (d3dUsage == D3D11_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0);
	m_pd3dNormalBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, pd3dxvNormals, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, pd3dxvTexCoords, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dDetailTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, pd3dxvDetailTexCoords, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	if (pd3dxvNormals) delete[] pd3dxvNormals;
	if (pd3dxvTexCoords) delete[] pd3dxvTexCoords;
	if (pd3dxvDetailTexCoords) delete[] pd3dxvDetailTexCoords;

	ID3D11Buffer *pd3dBuffers[4] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_pd3dTexCoordBuffer, m_pd3dDetailTexCoordBuffer };
	UINT pnBufferStrides[4] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[4] = { 0, 0, 0, 0 };
	AssembleToVertexBuffer(4, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_nIndices = ((nWidth * 2)*(nLength - 1)) + ((nLength - 1) - 1);
	m_pnIndices = new UINT[m_nIndices];
	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			for (int x = 0; x < nWidth; x++)
			{
				if ((x == 0) && (z > 0)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			for (int x = nWidth - 1; x >= 0; x--)
			{
				if (x == (nWidth - 1)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	m_pd3dIndexBuffer = CreateBuffer(pd3dDevice, sizeof(UINT), m_nIndices, m_pnIndices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(xStart*m_d3dxvScale.x, fMinHeight, zStart*m_d3dxvScale.z);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3((xStart + nWidth)*m_d3dxvScale.x, fMaxHeight, (zStart + nLength)*m_d3dxvScale.z);
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

float CHeightMapGridMesh::OnGetHeight(int x, int z, void *pContext)
{
	float fHeight = 0.0f;
	if (pContext)
	{
		CHeightMap *pHeightMap = (CHeightMap *)pContext;
		BYTE *pHeightMapImage = pHeightMap->GetHeightMapImage();
		D3DXVECTOR3 d3dxvScale = pHeightMap->GetScale();
		int nWidth = pHeightMap->GetHeightMapWidth();
		fHeight = pHeightMapImage[x + (z*nWidth)] * d3dxvScale.y;
	}
	return(fHeight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkyBoxMesh::CSkyBoxMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshTextured(pd3dDevice)
{
	m_nVertices = 24;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];

	int i = 0;
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
	// Front Quad (remember all quads point inward)
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, +fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, +fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, +fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, +fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Back Quad																
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, -fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, -fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, -fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, -fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Left Quad																
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, -fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, +fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, +fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, -fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Right Quad																
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, +fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, -fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, -fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, +fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Top Quad																	
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, -fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, -fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, +fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, +fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Bottom Quad																
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, +fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, +fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, -fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, -fx); pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, pd3dxvTexCoords, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	delete[] pd3dxvTexCoords;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_nIndices = 4;
	m_pnIndices = new UINT[m_nIndices];

	m_pnIndices[0] = 0;
	m_pnIndices[1] = 1;
	m_pnIndices[2] = 3;
	m_pnIndices[3] = 2;

	m_pd3dIndexBuffer = CreateBuffer(pd3dDevice, sizeof(UINT), m_nIndices, m_pnIndices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CSkyBoxMesh::~CSkyBoxMesh()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWaterGridMesh
CWaterGridMesh::CWaterGridMesh(ID3D11Device *pd3dDevice, int xStart, int zStart, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale, void *pContext, D3D11_USAGE d3dUsage) : CHeightMapGridMesh(pd3dDevice, xStart, zStart, nWidth, nLength, d3dxvScale, pContext, d3dUsage)
{
	m_xStart = xStart;
	m_zStart = zStart;

	m_pd3dxvPreviousPositions = NULL;
	m_pd3dxvCurrentPositions = NULL;
	m_pd3dxvNormalVectors = NULL;
	m_pd3dxvTempPositions = NULL;

	OnPrepareAnimation(0.00375f, 1.0f, 1.25f, 0.00001f);
}

CWaterGridMesh::~CWaterGridMesh()
{
	if (m_pd3dxvPreviousPositions) delete[] m_pd3dxvPreviousPositions;
	if (m_pd3dxvCurrentPositions) delete[] m_pd3dxvCurrentPositions;
	if (m_pd3dxvNormalVectors) delete[] m_pd3dxvNormalVectors;
	if (m_pd3dxvTempPositions) delete[] m_pd3dxvTempPositions;
}

void CWaterGridMesh::OnPrepareAnimation(float fTimeDelta, float fSpatialDelta, float fSpeed, float fDamping)
{
	m_fTimeDelta = fTimeDelta;
	m_fSpatialDelta = fSpatialDelta;

	m_fAccumulatedTime = 0.0f;

	float d = (fDamping * m_fTimeDelta) + 2.0f;
	float e = (fSpeed * fSpeed) * (m_fTimeDelta * m_fTimeDelta) / (m_fSpatialDelta * m_fSpatialDelta);
	m_fK1 = ((fDamping * m_fTimeDelta) - 2.0f) / d;
	m_fK2 = (4.0f - (8.0f * e)) / d;
	m_fK3 = (2.0f * e) / d;

	m_pd3dxvPreviousPositions = new D3DXVECTOR3[m_nLength*m_nWidth];
	m_pd3dxvCurrentPositions = new D3DXVECTOR3[m_nLength*m_nWidth];
	m_pd3dxvTempPositions = new D3DXVECTOR3[m_nLength*m_nWidth];
	m_pd3dxvNormalVectors = new D3DXVECTOR3[m_nLength*m_nWidth];

	for (int i = 0, z = m_zStart; z < (m_zStart + m_nLength); i++, z++)
	{
		for (int j = 0, x = m_xStart; x < (m_xStart + m_nWidth); j++, x++)
		{
			m_pd3dxvPreviousPositions[(i*m_nWidth) + j] = D3DXVECTOR3((x*m_d3dxvScale.x), 0.0f, (z*m_d3dxvScale.z));
			m_pd3dxvCurrentPositions[(i*m_nWidth) + j] = D3DXVECTOR3((x*m_d3dxvScale.x), 0.0f, (z*m_d3dxvScale.z));
			m_pd3dxvNormalVectors[(i*m_nWidth) + j] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		}
	}

	for (int k = 0; k < 10; k++)
	{
		int i = 5 + (rand() % (m_nLength - 10));
		int j = 5 + (rand() % (m_nWidth - 10));

		float fMagnitude = (0.5f + ((float)(rand()) / (float)RAND_MAX) * (1.25f - 0.5f));

		SetDisturbingForce(i, j, fMagnitude);
	}
}

void CWaterGridMesh::SetDisturbingForce(int i, int j, float fMagnitude)
{
	if (((i > 1) && (i < m_nLength - 2)) && ((j > 1) && (j < m_nWidth - 2)))
	{
		m_pd3dxvCurrentPositions[(i*m_nWidth) + j].y += fMagnitude;
		m_pd3dxvCurrentPositions[(i*m_nWidth) + j + 1].y += fMagnitude * 0.5f;
		m_pd3dxvCurrentPositions[(i*m_nWidth) + j - 1].y += fMagnitude * 0.5f;
		m_pd3dxvCurrentPositions[((i + 1)*m_nWidth) + j].y += fMagnitude * 0.5f;
		m_pd3dxvCurrentPositions[((i - 1)*m_nWidth) + j].y += fMagnitude * 0.5f;
	}
}

float CWaterGridMesh::OnGetHeight(int x, int z, void *pContext)
{
	return(0.0f);
}

void CWaterGridMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	Animate(pd3dDeviceContext, 0.00003f);

	CHeightMapGridMesh::Render(pd3dDeviceContext);
}

void CWaterGridMesh::Animate(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed)
{
	m_fAccumulatedTime += fTimeElapsed;
	if (m_fAccumulatedTime >= m_fTimeDelta)
	{
		D3DXVECTOR3 d3dxvPosition(0.0f, 1.0f, 0.0f), d3dxvNormal(0.0f, 1.0f, 0.0f);
		float fyPreviousPosition = 0.0f;
		for (int i = 1; i < m_nLength - 1; i++)
		{
			for (int j = 1; j < m_nWidth - 1; j++)
			{
				m_pd3dxvPreviousPositions[(i*m_nWidth) + j].y = m_fK1 * m_pd3dxvPreviousPositions[(i*m_nWidth) + j].y + m_fK2 * m_pd3dxvCurrentPositions[(i*m_nWidth) + j].y + m_fK3 * (m_pd3dxvCurrentPositions[((i + 1)*m_nWidth) + j].y + m_pd3dxvCurrentPositions[((i - 1)*m_nWidth) + j].y + m_pd3dxvCurrentPositions[(i*m_nWidth) + j + 1].y + m_pd3dxvCurrentPositions[(i*m_nWidth) + j - 1].y);
			}
		}

		memcpy(m_pd3dxvTempPositions, m_pd3dxvPreviousPositions, sizeof(D3DXVECTOR3)*m_nWidth*m_nLength);
		memcpy(m_pd3dxvPreviousPositions, m_pd3dxvCurrentPositions, sizeof(D3DXVECTOR3)*m_nWidth*m_nLength);
		memcpy(m_pd3dxvCurrentPositions, m_pd3dxvTempPositions, sizeof(D3DXVECTOR3)*m_nWidth*m_nLength);

		for (int i = 1; i < m_nLength - 1; i++)
		{
			for (int j = 1; j < m_nWidth - 1; j++)
			{
				// Find two tangent vectors, one in +x and one in -z direction. Then take cross product to get normal. Use finite difference to compute tangents.  
				float l = m_pd3dxvCurrentPositions[(i*m_nWidth) + j - 1].y;
				float r = m_pd3dxvCurrentPositions[(i*m_nWidth) + j + 1].y;
				float t = m_pd3dxvCurrentPositions[((i - 1)*m_nWidth) + j - 1].y;
				float b = m_pd3dxvCurrentPositions[((i + 1)*m_nWidth) + j + 1].y;
				d3dxvNormal = D3DXVECTOR3((-r + l), (2.0f * m_fSpatialDelta), (b - t));
				D3DXVec3Normalize(&d3dxvNormal, &d3dxvNormal);
				m_pd3dxvNormalVectors[(i*m_nWidth) + j] = d3dxvNormal;
			}
		}

		D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
		pd3dDeviceContext->Map(m_pd3dPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
		D3DXVECTOR3 *pd3dxvPositions = (D3DXVECTOR3 *)d3dMappedResource.pData;
		D3DXVECTOR2 d3dxvTexture1, d3dxvTexture2;
		for (int i = 0, z = m_zStart; z < (m_zStart + m_nLength); z++, i++)
		{
			for (int j = 0, x = m_xStart; x < (m_xStart + m_nWidth); x++, j++)
			{
				pd3dxvPositions[(i*m_nWidth) + j] = m_pd3dxvCurrentPositions[(i*m_nWidth) + j];
			}
		}
		pd3dDeviceContext->Unmap(m_pd3dPositionBuffer, 0);

		m_fAccumulatedTime = 0.0f;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CTextureToScreenRectMesh::CTextureToScreenRectMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight) : CMeshTextured(pd3dDevice)
{
	m_nVertices = 6;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];

	m_pd3dxvPositions[0] = D3DXVECTOR3(+fx, +fy, 0.0f); pd3dxvTexCoords[0] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[1] = D3DXVECTOR3(+fx, -fy, 0.0f); pd3dxvTexCoords[1] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[2] = D3DXVECTOR3(-fx, -fy, 0.0f); pd3dxvTexCoords[2] = D3DXVECTOR2(0.0f, 1.0f);
	m_pd3dxvPositions[3] = D3DXVECTOR3(-fx, -fy, 0.0f); pd3dxvTexCoords[3] = D3DXVECTOR2(0.0f, 1.0f);
	m_pd3dxvPositions[4] = D3DXVECTOR3(-fx, +fy, 0.0f); pd3dxvTexCoords[4] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[5] = D3DXVECTOR3(+fx, +fy, 0.0f); pd3dxvTexCoords[5] = D3DXVECTOR2(1.0f, 0.0f);

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, pd3dxvTexCoords, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	delete[] pd3dxvTexCoords;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, 0);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, 0);
}

CTextureToScreenRectMesh::~CTextureToScreenRectMesh()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexturedRectMesh::CTexturedRectMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fLength) : CMeshTextured(pd3dDevice)
{
	m_nVertices = 6;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fLength*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];

	if ((fx != 0.0f) && (fz != 0.0f) && (fy == 0.0f))
	{
		m_pd3dxvPositions[0] = D3DXVECTOR3(+fx, 0.0f, +fz); pd3dxvTexCoords[0] = D3DXVECTOR2(1.0f, 0.0f);
		m_pd3dxvPositions[1] = D3DXVECTOR3(+fx, 0.0f, -fz); pd3dxvTexCoords[1] = D3DXVECTOR2(1.0f, 1.0f);
		m_pd3dxvPositions[2] = D3DXVECTOR3(-fx, 0.0f, -fz); pd3dxvTexCoords[2] = D3DXVECTOR2(0.0f, 1.0f);
		m_pd3dxvPositions[3] = D3DXVECTOR3(-fx, 0.0f, -fz); pd3dxvTexCoords[3] = D3DXVECTOR2(0.0f, 1.0f);
		m_pd3dxvPositions[4] = D3DXVECTOR3(-fx, 0.0f, +fz); pd3dxvTexCoords[4] = D3DXVECTOR2(0.0f, 0.0f);
		m_pd3dxvPositions[5] = D3DXVECTOR3(+fx, 0.0f, +fz); pd3dxvTexCoords[5] = D3DXVECTOR2(1.0f, 0.0f);
	}
	else if ((fx != 0.0f) && (fy != 0.0f) && (fz == 0.0f))
	{
		m_pd3dxvPositions[0] = D3DXVECTOR3(+fx, +fy, 0.0f); pd3dxvTexCoords[0] = D3DXVECTOR2(1.0f, 0.0f);
		m_pd3dxvPositions[1] = D3DXVECTOR3(+fx, -fy, 0.0f); pd3dxvTexCoords[1] = D3DXVECTOR2(1.0f, 1.0f);
		m_pd3dxvPositions[2] = D3DXVECTOR3(-fx, -fy, 0.0f); pd3dxvTexCoords[2] = D3DXVECTOR2(0.0f, 1.0f);
		m_pd3dxvPositions[3] = D3DXVECTOR3(-fx, -fy, 0.0f); pd3dxvTexCoords[3] = D3DXVECTOR2(0.0f, 1.0f);
		m_pd3dxvPositions[4] = D3DXVECTOR3(-fx, +fy, 0.0f); pd3dxvTexCoords[4] = D3DXVECTOR2(0.0f, 0.0f);
		m_pd3dxvPositions[5] = D3DXVECTOR3(+fx, +fy, 0.0f); pd3dxvTexCoords[5] = D3DXVECTOR2(1.0f, 0.0f);
	}
	else
	{
		m_pd3dxvPositions[0] = D3DXVECTOR3(0.0f, +fy, -fz); pd3dxvTexCoords[0] = D3DXVECTOR2(1.0f, 0.0f);
		m_pd3dxvPositions[1] = D3DXVECTOR3(0.0f, -fy, -fz); pd3dxvTexCoords[1] = D3DXVECTOR2(1.0f, 1.0f);
		m_pd3dxvPositions[2] = D3DXVECTOR3(0.0f, -fy, +fz); pd3dxvTexCoords[2] = D3DXVECTOR2(0.0f, 1.0f);
		m_pd3dxvPositions[3] = D3DXVECTOR3(0.0f, -fy, +fz); pd3dxvTexCoords[3] = D3DXVECTOR2(0.0f, 1.0f);
		m_pd3dxvPositions[4] = D3DXVECTOR3(0.0f, +fy, +fz); pd3dxvTexCoords[4] = D3DXVECTOR2(0.0f, 0.0f);
		m_pd3dxvPositions[5] = D3DXVECTOR3(0.0f, +fy, -fz); pd3dxvTexCoords[5] = D3DXVECTOR2(1.0f, 0.0f);
	}

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, pd3dxvTexCoords, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	delete[] pd3dxvTexCoords;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CTexturedRectMesh::~CTexturedRectMesh()
{
}



CSkinMeshDiffused::CSkinMeshDiffused(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth,string filename) : CMeshTextured (pd3dDevice)
{
	m_nType |= VERTEX_NORMAL_ELEMENT;
	ifstream fin(filename);
	string ignore;

	if (!fin.fail())
	{
		// 데이터를 읽어와 필요한 정점, 인덱스, 본, 애니메이션 수 파악
		fin >> ignore;//[FBX_META_DATA]
		fin >> ignore >> ignore; //"MeshCount: " , "1"

		fin >> ignore;	//[MESH_DATA]
		fin >> ignore >> m_nVertices;	//"VertexCount: "
		fin >> ignore >> m_nIndices;	//"IndexCount: "
		fin >> ignore >> ignore;	//"BoneCount: "
		fin >> ignore >> ignore;	//"AnimationClips: "

											// 정점 데이터를 저장
		m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
				m_pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
				m_pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];
				m_pd3dxvBoneIndices = new D3DXVECTOR4[m_nVertices];
				m_pd3dxvBoneWeights = new D3DXVECTOR4[m_nVertices];


		fin >> ignore;	// [VERTEX_DATA]

		for (int i = 0; i < m_nVertices; i++)
		{
			fin >> ignore >> m_pd3dxvPositions[i].x >> m_pd3dxvPositions[i].y >> m_pd3dxvPositions[i].z;
			fin >> ignore >> m_pd3dxvNormals[i].x >> m_pd3dxvNormals[i].y >> m_pd3dxvNormals[i].z;
			fin >> ignore >> m_pd3dxvTexCoords[i].x >> m_pd3dxvTexCoords[i].y;
			fin >> ignore >> m_pd3dxvBoneIndices[i].x >> m_pd3dxvBoneIndices[i].y >> m_pd3dxvBoneIndices[i].z >> m_pd3dxvBoneIndices[i].w;
			fin >> ignore >> m_pd3dxvBoneWeights[i].x >> m_pd3dxvBoneWeights[i].y >> m_pd3dxvBoneWeights[i].z >> m_pd3dxvBoneWeights[i].w;
		}

		m_pnIndices = new UINT[m_nIndices];
		fin >> ignore;	//[INDEX_DATA]
		for (int i = 0; i < m_nIndices; ++i)
			fin >> m_pnIndices[i];
		fin.close();

		//바운딩 큐브용
		D3DXVECTOR3 Min = {0.f,0.f,0.f}, Max = { 0.f,0.f,0.f };
		for (int i = 0; i < m_nVertices; i++)
		{
			if(filename==BANSHEE_TXT)
				m_pd3dxvPositions[i] *= 10.0f;
			else
			m_pd3dxvPositions[i] *= 25.0f;
			if (Min.x > m_pd3dxvPositions[i].x)Min.x = m_pd3dxvPositions[i].x;
			if (Min.y > m_pd3dxvPositions[i].y)Min.y = m_pd3dxvPositions[i].y;
			if (Min.z > m_pd3dxvPositions[i].z)Min.z = m_pd3dxvPositions[i].z;
			if (Max.x < m_pd3dxvPositions[i].x)Max.x = m_pd3dxvPositions[i].x;
			if (Max.y < m_pd3dxvPositions[i].y)Max.y = m_pd3dxvPositions[i].y;
			if (Max.z < m_pd3dxvPositions[i].z)Max.z = m_pd3dxvPositions[i].z;
			
			
		}










		//	m_nVertices = 8;
		m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

		//m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];

		//m_pd3dxvPositions[0] = D3DXVECTOR3(-fx, +fy, -fz);
		//m_pd3dxvPositions[1] = D3DXVECTOR3(+fx, +fy, -fz);
		//m_pd3dxvPositions[2] = D3DXVECTOR3(+fx, +fy, +fz);
		//m_pd3dxvPositions[3] = D3DXVECTOR3(-fx, +fy, +fz);
		//m_pd3dxvPositions[4] = D3DXVECTOR3(-fx, -fy, -fz);
		//m_pd3dxvPositions[5] = D3DXVECTOR3(+fx, -fy, -fz);
		//m_pd3dxvPositions[6] = D3DXVECTOR3(+fx, -fy, +fz);
		//m_pd3dxvPositions[7] = D3DXVECTOR3(-fx, -fy, +fz);

		/*D3DXCOLOR *pd3dxColors = new D3DXCOLOR[m_nVertices];
		for (int i = 0; i < m_nVertices; i++) pd3dxColors[i] = d3dxColor + RANDOM_COLOR;
*/
		m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
		//m_pd3dColorBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXCOLOR), m_nVertices, pd3dxColors, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

		/*ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
		UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
		UINT pnBufferOffsets[2] = { 0, 0 };
		AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);*/
		//
		//#define _WITH_CORRECT_STRIP
		//#ifdef _WITH_CORRECT_STRIP
		//	m_nIndices = 18;
		//	m_pnIndices = new UINT[m_nIndices];
		//
		//	m_pnIndices[0] = 5; //5,6,4 - cw
		//	m_pnIndices[1] = 6; //6,4,7 - ccw
		//	m_pnIndices[2] = 4; //4,7,0 - cw
		//	m_pnIndices[3] = 7; //7,0,3 - ccw
		//	m_pnIndices[4] = 0; //0,3,1 - cw
		//	m_pnIndices[5] = 3; //3,1,2 - ccw
		//	m_pnIndices[6] = 1; //1,2,2 - cw 
		//	m_pnIndices[7] = 2; //2,2,3 - ccw
		//	m_pnIndices[8] = 2; //2,3,3 - cw  - Degenerated Index
		//	m_pnIndices[9] = 3; //3,3,7 - ccw - Degenerated Index
		//	m_pnIndices[10] = 3;//3,7,2 - cw  - Degenerated Index
		//	m_pnIndices[11] = 7;//7,2,6 - ccw
		//	m_pnIndices[12] = 2;//2,6,1 - cw
		//	m_pnIndices[13] = 6;//6,1,5 - ccw
		//	m_pnIndices[14] = 1;//1,5,0 - cw
		//	m_pnIndices[15] = 5;//5,0,4 - ccw
		//	m_pnIndices[16] = 0;
		//	m_pnIndices[17] = 4;
		//#else
		//	m_nIndices = 16;
		//	m_pnIndices = new UINT[m_nIndices];
		//
		//	m_pnIndices[0] = 5; //5,6,4 - cw
		//	m_pnIndices[1] = 6; //6,4,7 - ccw
		//	m_pnIndices[2] = 4; //4,7,0 - cw
		//	m_pnIndices[3] = 7; //7,0,3 - ccw
		//	m_pnIndices[4] = 0; //0,3,1 - cw
		//	m_pnIndices[5] = 3; //3,1,2 - ccw
		//	m_pnIndices[6] = 1; //1,2,3 - cw 
		//	m_pnIndices[7] = 2; //2,3,7 - ccw
		//	m_pnIndices[8] = 3; //3,7,2 - cw - Degenerated Index
		//	m_pnIndices[9] = 7; //7,2,6 - ccw
		//	m_pnIndices[10] = 2;//2,6,1 - cw
		//	m_pnIndices[11] = 6;//6,1,5 - ccw
		//	m_pnIndices[12] = 1;//1,5,0 - cw
		//	m_pnIndices[13] = 5;//5,0,4 - ccw
		//	m_pnIndices[14] = 0;
		//	m_pnIndices[15] = 4;
		//#endif

		m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, m_pd3dxvTexCoords, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
		m_pd3dNormalBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvNormals, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
		ID3D11Buffer *pd3dBuffers[3] = { m_pd3dPositionBuffer,m_pd3dNormalBuffer, m_pd3dTexCoordBuffer };
		UINT pnBufferStrides[3] = { sizeof(D3DXVECTOR3),sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
		UINT pnBufferOffsets[3] = { 0, 0,0 };
		AssembleToVertexBuffer(3, pd3dBuffers, pnBufferStrides, pnBufferOffsets);


		m_pd3dIndexBuffer = CreateBuffer(pd3dDevice, sizeof(UINT), m_nIndices, m_pnIndices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
		//if (m_pd3dNormalBuffer) delete[] m_pd3dNormalBuffer;
		//if (m_pd3dTexCoordBuffer) delete[] m_pd3dTexCoordBuffer;
		m_bcBoundingCube.m_d3dxvMinimum = Min;
		m_bcBoundingCube.m_d3dxvMaximum = Max;

	}
}
CSkinMeshDiffused::~CSkinMeshDiffused()
{
}

CTitileMesh::CTitileMesh(ID3D11Device *pd3dDevice, float ul, float ur, float vu, float vd) : CMeshTexturedIlluminated(pd3dDevice)
{
	m_nVertices = 6;
	D3DXVECTOR2 pd3dxvTexCoords[6];
	D3DXVECTOR3 m_pd3dxvPositions[6];
	//m_pnVertexStrides
	//int m_nStride = sizeof(CTexturedVertex);
	m_nIndexOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pd3dxvPositions[0] = D3DXVECTOR3(ul, vu, 0.0f);
	pd3dxvTexCoords[0] = D3DXVECTOR2(0.0f, 1.0f);
	m_pd3dxvPositions[1] = D3DXVECTOR3(ul, vd, 0.0f);
	pd3dxvTexCoords[1] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[2] = D3DXVECTOR3(ur, vd, 0.0f);
	pd3dxvTexCoords[2] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[3] = D3DXVECTOR3(ur, vd, 0.0f);
	pd3dxvTexCoords[3] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[4] = D3DXVECTOR3(ur, vu, 0.0f);
	pd3dxvTexCoords[4] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[5] = D3DXVECTOR3(ul, vu, 0.0f);
	pd3dxvTexCoords[5] = D3DXVECTOR2(0.0f, 1.0f);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
}

CTitileMesh::~CTitileMesh()
{

}

CBoundingCubeMesh::CBoundingCubeMesh(ID3D11Device * pd3dDevice, D3DXVECTOR3 Max, D3DXVECTOR3 Min, D3DXCOLOR d3dxColor) :CMeshDiffused(pd3dDevice)
{
	m_nVertices = 216;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;



	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	int i = 0;
	////일반적인 바운딩박스
	////위
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Max.y,Max.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x,Max.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x,Max.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Max.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Max.y,Max.z);

	////앞
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Max.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x,Max.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x,Min.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x,Min.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Max.y,Min.z);
	////왼
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Max.y,Max.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Max.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Min.y,Max.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Min.y,Max.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Max.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min);
	////오
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x,Max.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x,Min.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x,Min.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x,Min.y,Max.z);
	////뒤
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Max.y,Max.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Min.y,Max.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Min.y,Max.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Min.y, Max.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max);
	////아래
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x,Min.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Min.y,Max.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x,Min.y,Max.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x,Min.y,Min.z);
	//m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x,Min.y,Max.z);


	//머리 바운딩박스
	//위
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y, Max.z/4);

	//앞										
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y / 1.25, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y / 1.25, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y / 1.25, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y, Min.z/4);
	//왼										
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y / 1.25, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y / 1.25, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y / 1.25, Min.z/4);
	//오										
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y / 1.25, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y / 1.25, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y / 1.25, Max.z/4);
	//뒤										
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y / 1.25, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y / 1.25, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y / 1.25, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y, Max.z/4);
	//아래									
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y / 1.25, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y / 1.25, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y / 1.25, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 4, Max.y / 1.25, Max.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y / 1.25, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 4, Max.y / 1.25, Max.z/4);






	//몸통 바운딩 박스
	//위
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 1.25, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 1.25, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 1.25, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 1.25, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 1.25, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 1.25, Max.z / 2);

	//앞										
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 1.25, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 1.25, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 1.25, Min.z / 4);
	//왼										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 1.25, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 1.25, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 2, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 2, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 1.25, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 2, Min.z / 4);
	//오										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 1.25, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 1.25, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 1.25, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 2, Max.z / 2);
	//뒤										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 1.25, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 1.25, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 2, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 2, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 2, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 1.25, Max.z / 2);
	//아래										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 2, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 2, Max.z / 2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y / 2, Max.z / 2);


	//왼쪽 손
	//위
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Max.y , Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/ 2, Max.y , Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/ 2, Max.y , Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/ 2, Max.y , Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Max.y , Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Max.y , Max.z );

	//앞										
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Max.y , Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y , Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2,Min.y, Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2,Min.y, Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x ,Min.y, Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Max.y , Min.z );
	//왼										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Max.y , Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Max.y , Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Min.y, Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Min.y, Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Max.y , Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Min.y, Min.z );
	//오										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y , Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y , Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Min.y, Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Min.y, Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y , Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Min.y,  Max.z );
	//뒤										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y , Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Max.y , Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Min.y, Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Min.y, Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Min.y, Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y, Max.z );
	//아래										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x, Min.y, Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Min.y, Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Min.y, Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x , Min.y, Max.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Min.y, Min.z );
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Min.y, Max.z );

	//오른쪽 손
	//위
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Max.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Max.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Max.y, Max.z/2);

	//앞										
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Max.y, Min.z/4);
	//왼										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Max.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Min.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Min.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Min.y, Min.z/4);
	//오										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Max.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Max.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Max.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Min.y, Max.z/2);
	//뒤										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Max.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Max.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Min.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Min.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Min.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Max.y, Max.z/2);
	//아래										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Min.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Min.y, Max.z/2);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x, Min.y, Max.z/2);

	//오른쪽 다리
	//위
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Max.y/2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Max.y/2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x /2, Max.y/2, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y/2, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Max.y/2, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Max.y/2, Max.z);

	//앞										
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Max.y/2, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Max.y/2, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Max.y/2, Min.z/4);
	//왼										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Max.y/2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Max.y/2, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Max.y/2, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Min.y, Min.z/4);
	//오										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Max.y/2, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Max.y/2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Max.y/2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x/2, Min.y, Max.z);
	//뒤										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x /2, Max.y/2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Max.y/2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x /2, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x /2, Max.y/2, Max.z);
	//아래										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(1, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Min.y, Min.z/4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Max.x / 2, Min.y, Max.z);

	//왼쪽 다리
	//위
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/2, Max.y / 2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Max.y / 2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 2, Max.z);

	//앞										
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Min.y, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Min.y, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Min.y, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Max.y / 2, Min.z / 4);
	//왼										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/2, Max.y / 2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/2, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/2, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/2, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/2, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/2, Min.y, Min.z / 4);
	//오										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Max.y / 2, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Max.y / 2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Min.y, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Min.y, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Max.y / 2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Min.y, Max.z);
	//뒤										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Max.y / 2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/2, Max.y / 2, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/2, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/2, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Max.y / 2, Max.z);
	//아래										 
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x / 2, Min.y, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Min.y, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/2, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(Min.x/2, Min.y, Max.z);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Min.y, Min.z / 4);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-1, Min.y, Max.z);




	D3DXCOLOR pd3dxColors[216] = {0.f,0.f,0.f,0.f};
	for (int i = 0; i < 216; i++) pd3dxColors[i] = D3DXCOLOR(1.f,1.f,0.f,1.f);

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dColorBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXCOLOR), m_nVertices, pd3dxColors, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

//#define _WITH_CORRECT_STRIP
//#ifdef _WITH_CORRECT_STRIP
//	m_nIndices = 18;
//	m_pnIndices = new UINT[m_nIndices];
//
//	m_pnIndices[0] = 5; //5,6,4 - cw
//	m_pnIndices[1] = 6; //6,4,7 - ccw
//	m_pnIndices[2] = 4; //4,7,0 - cw
//	m_pnIndices[3] = 7; //7,0,3 - ccw
//	m_pnIndices[4] = 0; //0,3,1 - cw
//	m_pnIndices[5] = 3; //3,1,2 - ccw
//	m_pnIndices[6] = 1; //1,2,2 - cw 
//	m_pnIndices[7] = 2; //2,2,3 - ccw
//	m_pnIndices[8] = 2; //2,3,3 - cw  - Degenerated Index
//	m_pnIndices[9] = 3; //3,3,7 - ccw - Degenerated Index
//	m_pnIndices[10] = 3;//3,7,2 - cw  - Degenerated Index
//	m_pnIndices[11] = 7;//7,2,6 - ccw
//	m_pnIndices[12] = 2;//2,6,1 - cw
//	m_pnIndices[13] = 6;//6,1,5 - ccw
//	m_pnIndices[14] = 1;//1,5,0 - cw
//	m_pnIndices[15] = 5;//5,0,4 - ccw
//	m_pnIndices[16] = 0;
//	m_pnIndices[17] = 4;
//#else
//	m_nIndices = 16;
//	m_pnIndices = new UINT[m_nIndices];
//
//	m_pnIndices[0] = 5; //5,6,4 - cw
//	m_pnIndices[1] = 6; //6,4,7 - ccw
//	m_pnIndices[2] = 4; //4,7,0 - cw
//	m_pnIndices[3] = 7; //7,0,3 - ccw
//	m_pnIndices[4] = 0; //0,3,1 - cw
//	m_pnIndices[5] = 3; //3,1,2 - ccw
//	m_pnIndices[6] = 1; //1,2,3 - cw 
//	m_pnIndices[7] = 2; //2,3,7 - ccw
//	m_pnIndices[8] = 3; //3,7,2 - cw - Degenerated Index
//	m_pnIndices[9] = 7; //7,2,6 - ccw
//	m_pnIndices[10] = 2;//2,6,1 - cw
//	m_pnIndices[11] = 6;//6,1,5 - ccw
//	m_pnIndices[12] = 1;//1,5,0 - cw
//	m_pnIndices[13] = 5;//5,0,4 - ccw
//	m_pnIndices[14] = 0;
//	m_pnIndices[15] = 4;
//#endif
//
//	m_pd3dIndexBuffer = CreateBuffer(pd3dDevice, sizeof(UINT), m_nIndices, m_pnIndices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(Min);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(Max);
	m_bcBoundingCubeHead.m_d3dxvMaximum = D3DXVECTOR3(Max.x / 4, Max.y, Max.z / 4);
	m_bcBoundingCubeHead.m_d3dxvMinimum = D3DXVECTOR3(Min.x / 4, Max.y / 1.25, Min.z / 4);
	m_bcBoundingCubeBody.m_d3dxvMaximum = D3DXVECTOR3(Max.x / 2, Max.y / 1.25, Max.z / 2);
	m_bcBoundingCubeBody.m_d3dxvMinimum= D3DXVECTOR3(Min.x / 2, Max.y / 2, Min.z / 4);
	m_bcBoundingCubeLhand.m_d3dxvMaximum = D3DXVECTOR3(Min.x / 2, Max.y, Max.z);
	m_bcBoundingCubeLhand.m_d3dxvMinimum = D3DXVECTOR3(Min.x, Min.y, Min.z);
	m_bcBoundingCubeRhand.m_d3dxvMaximum = D3DXVECTOR3(Max.x, Max.y, Max.z/2);
	m_bcBoundingCubeRhand.m_d3dxvMinimum = D3DXVECTOR3(Max.x/2, Min.y, Min.z/4);
	m_bcBoundingCubeRleg.m_d3dxvMaximum = D3DXVECTOR3(Max.x/2, Max.y/2, Max.z);
	m_bcBoundingCubeRleg.m_d3dxvMinimum = D3DXVECTOR3(1, Min.y, Min.z/4);
	m_bcBoundingCubeLleg.m_d3dxvMaximum = D3DXVECTOR3(-1, Max.y / 2, Max.z);
	m_bcBoundingCubeLleg.m_d3dxvMinimum = D3DXVECTOR3(Min.x/2, Min.y, Min.z / 4);
}

CBoundingCubeMesh::~CBoundingCubeMesh()
{
}



CBulletMeshDiffused::CBulletMeshDiffused(ID3D11Device * pd3dDevice, string filename) : CMeshTextured(pd3dDevice)
{
	m_nType |= VERTEX_NORMAL_ELEMENT;
	ifstream fin(filename);
	string ignore;

	if (!fin.fail())
	{
		// 데이터를 읽어와 필요한 정점, 인덱스, 본, 애니메이션 수 파악
		fin >> ignore;//[FBX_META_DATA]
		fin >> ignore >> ignore; //"MeshCount: " , "1"

		fin >> ignore;	//[MESH_DATA]
		fin >> ignore >> m_nVertices;	//"VertexCount: "
		fin >> ignore >> m_nIndices;	//"IndexCount: "
		fin >> ignore >> ignore;	//"BoneCount: "
		fin >> ignore >> ignore;	//"AnimationClips: "

									// 정점 데이터를 저장
		m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
		m_pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
		m_pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];
		//		m_pd3dxvBoneIndices = new D3DXVECTOR4[m_nVertices];
		//		m_pd3dxvBoneWeights = new D3DXVECTOR4[m_nVertices];


		fin >> ignore;	// [VERTEX_DATA]

		for (int i = 0; i < m_nVertices; i++)
		{
			fin >> ignore >> m_pd3dxvPositions[i].x >> m_pd3dxvPositions[i].y >> m_pd3dxvPositions[i].z;
			fin >> ignore >> m_pd3dxvNormals[i].x >> m_pd3dxvNormals[i].y >> m_pd3dxvNormals[i].z;
			fin >> ignore >> m_pd3dxvTexCoords[i].x >> m_pd3dxvTexCoords[i].y;
			//	fin >> ignore >> m_pd3dxvBoneIndices[i].x >> m_pd3dxvBoneIndices[i].y >> m_pd3dxvBoneIndices[i].z >> m_pd3dxvBoneIndices[i].w;
			//	fin >> ignore >> m_pd3dxvBoneWeights[i].x >> m_pd3dxvBoneWeights[i].y >> m_pd3dxvBoneWeights[i].z >> m_pd3dxvBoneWeights[i].w;
		}

		m_pnIndices = new UINT[m_nIndices];
		fin >> ignore;	//[INDEX_DATA]
		for (int i = 0; i < m_nIndices; ++i)
			fin >> m_pnIndices[i];
		fin.close();

		//바운딩 큐브용
		D3DXVECTOR3 Min = { 0.f,0.f,0.f }, Max = { 0.f,0.f,0.f };
		for (int i = 0; i < m_nVertices; i++)
		{
			m_pd3dxvPositions[i] *= .1f;
			if (Min.x > m_pd3dxvPositions[i].x)Min.x = m_pd3dxvPositions[i].x;
			if (Min.y > m_pd3dxvPositions[i].y)Min.y = m_pd3dxvPositions[i].y;
			if (Min.z > m_pd3dxvPositions[i].z)Min.z = m_pd3dxvPositions[i].z;
			if (Max.x < m_pd3dxvPositions[i].x)Max.x = m_pd3dxvPositions[i].x;
			if (Max.y < m_pd3dxvPositions[i].y)Max.y = m_pd3dxvPositions[i].y;
			if (Max.z < m_pd3dxvPositions[i].z)Max.z = m_pd3dxvPositions[i].z;


		}











		m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
		//m_pd3dColorBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXCOLOR), m_nVertices, pd3dxColors, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

		/*ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
		UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
		UINT pnBufferOffsets[2] = { 0, 0 };
		AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);*/


		m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, m_pd3dxvTexCoords, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
		m_pd3dNormalBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvNormals, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
		ID3D11Buffer *pd3dBuffers[3] = { m_pd3dPositionBuffer,m_pd3dNormalBuffer, m_pd3dTexCoordBuffer };
		UINT pnBufferStrides[3] = { sizeof(D3DXVECTOR3),sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
		UINT pnBufferOffsets[3] = { 0, 0,0 };
		AssembleToVertexBuffer(3, pd3dBuffers, pnBufferStrides, pnBufferOffsets);


		m_pd3dIndexBuffer = CreateBuffer(pd3dDevice, sizeof(UINT), m_nIndices, m_pnIndices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
		//if (m_pd3dNormalBuffer) delete[] m_pd3dNormalBuffer;
		//if (m_pd3dTexCoordBuffer) delete[] m_pd3dTexCoordBuffer;
		m_bcBoundingCube.m_d3dxvMinimum = Min;
		m_bcBoundingCube.m_d3dxvMaximum = Max;
	}
}

CBulletMeshDiffused::~CBulletMeshDiffused()
{

}


CMarineMeshDiffused::CMarineMeshDiffused(ID3D11Device * pd3dDevice, string filename) : CMeshTextured(pd3dDevice)
{
	m_nVertices = 0;
	m_nBuffers = 0;
	m_pd3dPositionBuffer = NULL;

	//	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nSlot = 0;
	m_nStartVertex = 0;

	m_pd3dIndexBuffer = NULL;
	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;
	m_nIndexOffset = 0;
	m_dxgiIndexFormat = DXGI_FORMAT_R32_UINT;

	m_pd3dxvPositions = NULL;
	m_pnIndices = NULL;





	m_nFBXMaxFrameNum = 231;//MODEL_NUM_KEYFRAME 마린 키프레임수
	m_fFBXAnimationTime = 0.0f;
	m_nFBXAnimationNum = 0;
	D3DXMatrixIdentity(&m_d3dxmtxLocalTransform);
	D3DXMATRIX mtxScale;
	D3DXMATRIX mtxRotate;
	D3DXMatrixScaling(&mtxScale, 25.f, 25.f, 25.f);
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(0.0f), (float)D3DXToRadian(-0.0f), (float)D3DXToRadian(0.0f));
	m_d3dxmtxLocalTransform = mtxScale * mtxRotate;

	//	m_nType |= VERTEX_NORMAL_ELEMENT;
	ifstream fin(filename);
	string ignore;






	if (!fin.fail())
	{
		// 데이터를 읽어와 필요한 정점, 인덱스, 본, 애니메이션 수 파악
		fin >> ignore;//[FBX_META_DATA]
		fin >> ignore >> ignore; //"MeshCount: " , "1"

		fin >> ignore;	//[MESH_DATA]
		fin >> ignore >> m_nVertices;	//"VertexCount: "
		fin >> ignore >> m_nIndices;	//"IndexCount: "
		fin >> ignore >> m_nBoneCount;	//"BoneCount: "
		fin >> ignore >> m_nAnimationClip;	//"AnimationClips: "

											// 정점 데이터를 저장
		m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
		m_pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
		m_pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];
		m_pd3dxvBoneIndices = new D3DXVECTOR4[m_nVertices];
		m_pd3dxvBoneWeights = new D3DXVECTOR4[m_nVertices];


		fin >> ignore;	// [VERTEX_DATA]

		for (int i = 0; i < m_nVertices; i++)
		{
			fin >> ignore >> m_pd3dxvPositions[i].x >> m_pd3dxvPositions[i].y >> m_pd3dxvPositions[i].z;
			fin >> ignore >> m_pd3dxvNormals[i].x >> m_pd3dxvNormals[i].y >> m_pd3dxvNormals[i].z;
			fin >> ignore >> m_pd3dxvTexCoords[i].x >> m_pd3dxvTexCoords[i].y;
			fin >> ignore >> m_pd3dxvBoneIndices[i].x >> m_pd3dxvBoneIndices[i].y >> m_pd3dxvBoneIndices[i].z >> m_pd3dxvBoneIndices[i].w;
			fin >> ignore >> m_pd3dxvBoneWeights[i].x >> m_pd3dxvBoneWeights[i].y >> m_pd3dxvBoneWeights[i].z >> m_pd3dxvBoneWeights[i].w;
		}

		m_pnIndices = new UINT[m_nIndices];
		fin >> ignore;	//[INDEX_DATA]
		for (int i = 0; i < m_nIndices; ++i)
			fin >> m_pnIndices[i];

		// (애니메이션을 포함한 메쉬일 경우) 본 정보와 애니메이션 정보 저장
		if (0 != m_nBoneCount)
		{
			m_pBoneHierarchy = new UINT[m_nBoneCount];
			m_pd3dxmtxBoneOffsets = new D3DXMATRIX[m_nBoneCount];
			m_pd3dxmtxSQTTransform = new D3DXMATRIX[m_nBoneCount];
			m_pd3dxmtxFinalBone = new D3DXMATRIX[m_nBoneCount];

			//뼈대 기하구조 저장
			fin >> ignore;	//[BONE_HIERARCHY]
			for (int i = 0; i < m_nBoneCount; ++i)
			{
				fin >> ignore >> m_pBoneHierarchy[i];
			}

			// 뼈대 자체의 오프셋 행렬을 저장
			fin >> ignore; //[OFFSET_MATRIX]
			for (int i = 0; i < m_nBoneCount; i++)
				fin >> ignore >> m_pd3dxmtxBoneOffsets[i]._11 >> m_pd3dxmtxBoneOffsets[i]._12 >> m_pd3dxmtxBoneOffsets[i]._13 >> m_pd3dxmtxBoneOffsets[i]._14
				>> m_pd3dxmtxBoneOffsets[i]._21 >> m_pd3dxmtxBoneOffsets[i]._22 >> m_pd3dxmtxBoneOffsets[i]._23 >> m_pd3dxmtxBoneOffsets[i]._24
				>> m_pd3dxmtxBoneOffsets[i]._31 >> m_pd3dxmtxBoneOffsets[i]._32 >> m_pd3dxmtxBoneOffsets[i]._33 >> m_pd3dxmtxBoneOffsets[i]._34
				>> m_pd3dxmtxBoneOffsets[i]._41 >> m_pd3dxmtxBoneOffsets[i]._42 >> m_pd3dxmtxBoneOffsets[i]._43 >> m_pd3dxmtxBoneOffsets[i]._44;

			//애니메이션 행렬 저장
			fin >> ignore;//[ANIMATION_CLIPS]

			m_ppBoneAnimationData = new BoneAnimationData*[m_nAnimationClip];
			BoneAnimationData *pBoneAnimationData;
			for (int k = 0; k < m_nAnimationClip; k++)
			{
				pBoneAnimationData = new BoneAnimationData[m_nBoneCount];

				fin >> ignore >> ignore >> ignore;//"AnimationClip", "Take_001", "{"

				for (int i = 0; i < m_nBoneCount; i++)//bone 개수만큼
				{
					//		"Bone"   "i"		"키프레임 개수"							"{"
					fin >> ignore >> ignore >> pBoneAnimationData[i].m_nFrameCount >> ignore;

					pBoneAnimationData[i].m_pfAniTime = new float[pBoneAnimationData[i].m_nFrameCount];
					pBoneAnimationData[i].m_pd3dxvTranslate = new D3DXVECTOR3[pBoneAnimationData[i].m_nFrameCount];
					pBoneAnimationData[i].m_pd3dxvScale = new D3DXVECTOR3[pBoneAnimationData[i].m_nFrameCount];
					pBoneAnimationData[i].m_pd3dxvQuaternion = new D3DXVECTOR4[pBoneAnimationData[i].m_nFrameCount];
					for (int j = 0; j < pBoneAnimationData[i].m_nFrameCount; j++)
					{
						fin >> ignore >> pBoneAnimationData[i].m_pfAniTime[j];

						fin >> ignore >> pBoneAnimationData[i].m_pd3dxvTranslate[j].x >> pBoneAnimationData[i].m_pd3dxvTranslate[j].y
							>> pBoneAnimationData[i].m_pd3dxvTranslate[j].z;
						fin >> ignore >> pBoneAnimationData[i].m_pd3dxvScale[j].x >> pBoneAnimationData[i].m_pd3dxvScale[j].y
							>> pBoneAnimationData[i].m_pd3dxvScale[j].z;
						fin >> ignore >> pBoneAnimationData[i].m_pd3dxvQuaternion[j].x >> pBoneAnimationData[i].m_pd3dxvQuaternion[j].y
							>> pBoneAnimationData[i].m_pd3dxvQuaternion[j].z >> pBoneAnimationData[i].m_pd3dxvQuaternion[j].w;
					}

					fin >> ignore; // "}"
				}
				m_ppBoneAnimationData[k] = pBoneAnimationData;
			}
			fin >> ignore; // "}" clip 종료
		}

		fin.close();
	}





	//바운딩 큐브용
	D3DXVECTOR3 Min = { 0.f,0.f,0.f }, Max = { 0.f,0.f,0.f };
	D3DXVECTOR3	*m_pd3dxvPositionsk = new D3DXVECTOR3[m_nVertices];
	for (int i = 0; i < m_nVertices; i++)
	{
		if (filename == BANSHEE_TXT)
			m_pd3dxvPositions[i] *= 10.0f;
		else
			//		m_pd3dxvPositions[i] *= 25.0f;
			m_pd3dxvPositionsk[i] = m_pd3dxvPositions[i] * 25.f;
		if (Min.x > m_pd3dxvPositions[i].x)Min.x = m_pd3dxvPositions[i].x;
		if (Min.y > m_pd3dxvPositionsk[i].y)Min.y = m_pd3dxvPositionsk[i].y;
		if (Min.z > m_pd3dxvPositionsk[i].z)Min.z = m_pd3dxvPositionsk[i].z;
		if (Max.x < m_pd3dxvPositionsk[i].x)Max.x = m_pd3dxvPositionsk[i].x;
		if (Max.y < m_pd3dxvPositionsk[i].y)Max.y = m_pd3dxvPositionsk[i].y;
		if (Max.z < m_pd3dxvPositionsk[i].z)Max.z = m_pd3dxvPositionsk[i].z;
	}
	delete[]m_pd3dxvPositionsk;

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dNormalBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvNormals, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, m_pd3dxvTexCoords, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dBoneIndexBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR4), m_nVertices, m_pd3dxvBoneIndices, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dBoneWeightBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR4), m_nVertices, m_pd3dxvBoneWeights, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	ID3D11Buffer *pd3dBuffers[5] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_pd3dTexCoordBuffer, m_pd3dBoneIndexBuffer, m_pd3dBoneWeightBuffer };
	UINT pnBufferStrides[5] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR4), sizeof(D3DXVECTOR4) };
	UINT pnBufferOffsets[5] = { 0, 0, 0, 0, 0 };
	AssembleToVertexBuffer(5, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_pd3dIndexBuffer = CreateBuffer(pd3dDevice, sizeof(UINT), m_nIndices, m_pnIndices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	m_bcBoundingCube.m_d3dxvMinimum = Min;
	m_bcBoundingCube.m_d3dxvMaximum = Max;

}


CMarineMeshDiffused::~CMarineMeshDiffused()
{
}

void CMarineMeshDiffused::MakeBoneMatrix(int nNowframe, int nAnimationNum, int nBoneNum, D3DXMATRIX & BoneMatrix)
{
	// XMAffine 함수에서는 scale의 VECTOR3을 쓰지만
	// D3DXAffine 함수에서는 scale의 계수를 사용한다.
	if (m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_nFrameCount != 0)
	{
		float fScale = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvScale[nNowframe].z;
		D3DXVECTOR3 d3dxvTranslate = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvTranslate[nNowframe];
		D3DXQUATERNION d3dxvQuaternion = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvQuaternion[nNowframe];
		D3DXVECTOR3 d3dxvZero = { 0.0f, 0.0f, 0.0f };

		D3DXMatrixAffineTransformation(&BoneMatrix, fScale, &d3dxvZero, &d3dxvQuaternion, &d3dxvTranslate);
	}
	else // 해당 본에 애니메이션 프레임이 없을 경우 단위행렬을 리턴하자.
	{
		D3DXMatrixIdentity(&BoneMatrix);
	}
}

void CMarineMeshDiffused::UpdateBoneTransform(int nAnimationNum, int nNowFrame)
{
	for (int i = 0; i < m_nBoneCount; i++)
	{
		MakeBoneMatrix(nNowFrame, nAnimationNum, i, *(m_pd3dxmtxSQTTransform + i));
	}
	// 마지막으로 본의 기본 오프셋행렬을 곱해주어 최종 행렬을 만들어준다.
	for (int i = 0; i < m_nBoneCount; i++)
	{
		D3DXMATRIX offset = m_pd3dxmtxBoneOffsets[i];
		D3DXMATRIX toRoot = m_pd3dxmtxSQTTransform[i];
		D3DXMatrixMultiply(&m_pd3dxmtxFinalBone[i], &offset, &toRoot);
	}
}

float CMarineMeshDiffused::GetClipStartTime(int nAnimationNum)
{
	float t = FLT_MAX;
	for (UINT i = 0; i < m_nBoneCount; ++i)
	{
		int nFrameCount = m_ppBoneAnimationData[nAnimationNum][i].m_nFrameCount;
		if (0 == nFrameCount) continue;
		t = min(t, m_ppBoneAnimationData[nAnimationNum][i].m_pfAniTime[0]);
	}

	return t;
}

float CMarineMeshDiffused::GetClipEndTime(int nAnimationNum)
{
	float t = 0.0f;
	for (UINT i = 0; i < m_nBoneCount; ++i)
	{
		int nFrameCount = m_ppBoneAnimationData[nAnimationNum][i].m_nFrameCount;
		if (0 == nFrameCount) continue;
		t = max(t, m_ppBoneAnimationData[nAnimationNum][i].m_pfAniTime[nFrameCount - 1]);
	}

	return t;
}

void CMarineMeshDiffused::InterpolateBoneTransform(int nAnimationNum, float time)
{
	for (int i = 0; i < m_nBoneCount; i++)
	{
		InterpolateBone(nAnimationNum, i, time, *(m_pd3dxmtxSQTTransform + i));
	}
	// 마지막으로 본의 기본 오프셋행렬을 곱해주어 최종 행렬을 만들어준다.
	for (int i = 0; i < m_nBoneCount; i++)
	{
		D3DXMATRIX offset = m_pd3dxmtxBoneOffsets[i];
		D3DXMATRIX toRoot = m_pd3dxmtxSQTTransform[i];
		D3DXMatrixMultiply(&m_pd3dxmtxFinalBone[i], &offset, &toRoot);
	}
}

void CMarineMeshDiffused::InterpolateBone(int nAnimationNum, int nBoneNum, float time, D3DXMATRIX & BoneMatrix)
{
	if (m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_nFrameCount != 0)
	{
		int maxFrameCount = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_nFrameCount;

		if (time <= m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pfAniTime[0])
		{
			float S = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvScale[0].z;
			D3DXVECTOR3 P = (m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvTranslate[0]);
			D3DXQUATERNION Q = (m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvQuaternion[0]);

			D3DXVECTOR3 zero = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			D3DXMatrixAffineTransformation(&BoneMatrix, S, &zero, &Q, &P);
		}
		else if (time >= m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pfAniTime[maxFrameCount - 1])
		{
			float S = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvScale[maxFrameCount - 1].z;
			D3DXVECTOR3 P = (m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvTranslate[maxFrameCount - 1]);
			D3DXQUATERNION Q = (m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvQuaternion[maxFrameCount - 1]);

			D3DXVECTOR3 zero = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			D3DXMatrixAffineTransformation(&BoneMatrix, S, &zero, &Q, &P);
		}
		else
		{
			for (UINT i = 0; i < maxFrameCount - 1; ++i)
			{
				if (time >= m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pfAniTime[i] &&
					time <= m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pfAniTime[i + 1])
				{
					float lerpPercent = (time - m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pfAniTime[i]) /
						(m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pfAniTime[i + 1] -
							m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pfAniTime[i]);

					D3DXVECTOR3 s0 = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvScale[i];
					D3DXVECTOR3 s1 = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvScale[i + 1];

					D3DXVECTOR3 p0 = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvTranslate[i];
					D3DXVECTOR3 p1 = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvTranslate[i + 1];

					D3DXQUATERNION q0 = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvQuaternion[i];
					D3DXQUATERNION q1 = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvQuaternion[i + 1];

					D3DXVECTOR3 S, P;
					D3DXQUATERNION Q;

					D3DXVec3Lerp(&S, &s0, &s1, lerpPercent);
					D3DXVec3Lerp(&P, &p0, &p1, lerpPercent);
					D3DXQuaternionSlerp(&Q, &q0, &q1, lerpPercent);

					D3DXVECTOR3 zero = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
					D3DXMatrixAffineTransformation(&BoneMatrix, S.z, &zero, &Q, &P);

					break;
				}
			}
		}
	}
	else // 해당 본에 애니메이션 프레임이 없을 경우 단위행렬을 리턴하자.
	{
		D3DXMatrixIdentity(&BoneMatrix);
	}
}
