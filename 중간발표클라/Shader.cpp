//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Shader.h"
#include "Scene.h"

CShader::CShader()
{
	m_nReferences = 0;

	m_pd3dVertexShader = NULL;
	m_pd3dVertexLayout = NULL;
	m_pd3dPixelShader = NULL;
	m_pd3dGeometryShader = NULL;

	m_nType = 0x00;
}

CShader::~CShader()
{
	if (m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if (m_pd3dVertexLayout) m_pd3dVertexLayout->Release();
	if (m_pd3dPixelShader) m_pd3dPixelShader->Release();
	if (m_pd3dGeometryShader) m_pd3dGeometryShader->Release();
}

void CShader::CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputElements, UINT nElements, ID3D11InputLayout **ppd3dInputLayout)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dErrorBlob = NULL;
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dVertexShaderBlob, &pd3dErrorBlob, NULL)))
	{
		pd3dDevice->CreateVertexShader(pd3dVertexShaderBlob->GetBufferPointer(), pd3dVertexShaderBlob->GetBufferSize(), NULL, ppd3dVertexShader);
		pd3dDevice->CreateInputLayout(pd3dInputElements, nElements, pd3dVertexShaderBlob->GetBufferPointer(), pd3dVertexShaderBlob->GetBufferSize(), ppd3dInputLayout);
		pd3dVertexShaderBlob->Release();
	}
}

void CShader::CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dPixelShaderBlob = NULL, *pd3dErrorBlob = NULL;
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dPixelShaderBlob, &pd3dErrorBlob, NULL)))
	{
		pd3dDevice->CreatePixelShader(pd3dPixelShaderBlob->GetBufferPointer(), pd3dPixelShaderBlob->GetBufferSize(), NULL, ppd3dPixelShader);
		pd3dPixelShaderBlob->Release();
	}
}

void CShader::CreateGeometryShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11GeometryShader **ppd3dGeometryShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dPixelShaderBlob = NULL, *pd3dErrorBlob = NULL;
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dPixelShaderBlob, &pd3dErrorBlob, NULL)))
	{
		pd3dDevice->CreateGeometryShader(pd3dPixelShaderBlob->GetBufferPointer(), pd3dPixelShaderBlob->GetBufferSize(), NULL, ppd3dGeometryShader);
		pd3dPixelShaderBlob->Release();
	}
}

void CShader::CreateShader(ID3D11Device *pd3dDevice, UINT nType)
{
		m_nType |= nType;
		GetInputElementDesc(m_nType);
		LPCSTR pszVSShaderName = NULL, pszVSShaderModel = "vs_5_0", pszPSShaderName = NULL, pszPSShaderModel = "ps_5_0";
		GetShaderName(m_nType, &pszVSShaderName, &pszVSShaderModel, &pszPSShaderName, &pszPSShaderModel);
		CreateShader(pd3dDevice, NULL, 0, L"Effect.fx", pszVSShaderName, pszVSShaderModel, pszPSShaderName, pszPSShaderModel);

}

void CShader::CreateShader(ID3D11Device *pd3dDevice, D3D11_INPUT_ELEMENT_DESC *pd3dInputElementDesc, int nInputElements, WCHAR *pszFileName, LPCSTR pszVSShaderName, LPCSTR pszVSShaderModel, LPCSTR pszPSShaderName, LPCSTR pszPSShaderModel)
{
	CreateVertexShaderFromFile(pd3dDevice, pszFileName, pszVSShaderName, pszVSShaderModel, &m_pd3dVertexShader, (pd3dInputElementDesc) ? pd3dInputElementDesc : m_pd3dInputElementDescs, (pd3dInputElementDesc) ? nInputElements : m_nInputElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, pszFileName, pszPSShaderName, pszPSShaderModel, &m_pd3dPixelShader);
}

void CShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
}

void CShader::ReleaseShaderVariables()
{
}

void CShader::GetInputElementDesc(UINT nVertexElementType)
{
	m_nInputElements = 0;
	if (nVertexElementType & VERTEX_POSITION_ELEMENT)
		m_nInputElements++;
	if (nVertexElementType & VERTEX_COLOR_ELEMENT) 
		m_nInputElements++;
	if (nVertexElementType & VERTEX_NORMAL_ELEMENT) 
		m_nInputElements++;
	if (nVertexElementType & VERTEX_TEXTURE_ELEMENT_0)
		m_nInputElements++;
	if (nVertexElementType & VERTEX_TEXTURE_ELEMENT_1)
		m_nInputElements++;
	if (nVertexElementType & VERTEX_BONE_ID_ELEMENT)
		m_nInputElements++;
	if (nVertexElementType & VERTEX_BONE_WEIGHT_ELEMENT) 
		m_nInputElements++;
	if (nVertexElementType & VERTEX_INSTANCING_ELEMENT)
		m_nInputElements += 4;
	m_pd3dInputElementDescs = new D3D11_INPUT_ELEMENT_DESC[m_nInputElements];
	UINT nIndex = 0, nSlot = 0;
	if (nVertexElementType & VERTEX_POSITION_ELEMENT)
		m_pd3dInputElementDescs[nIndex++] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, nSlot++, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	if (nVertexElementType & VERTEX_COLOR_ELEMENT) 
		m_pd3dInputElementDescs[nIndex++] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, nSlot++, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	if (nVertexElementType & VERTEX_NORMAL_ELEMENT)
		m_pd3dInputElementDescs[nIndex++] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, nSlot++, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	if (nVertexElementType & VERTEX_TEXTURE_ELEMENT_0)
		m_pd3dInputElementDescs[nIndex++] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, nSlot++, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	if (nVertexElementType & VERTEX_TEXTURE_ELEMENT_1) 
		m_pd3dInputElementDescs[nIndex++] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, nSlot++, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	if (nVertexElementType & VERTEX_BONE_ID_ELEMENT)
		m_pd3dInputElementDescs[nIndex++] = { "BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_SINT, nSlot++, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	if (nVertexElementType & VERTEX_BONE_WEIGHT_ELEMENT)
		m_pd3dInputElementDescs[nIndex++] = { "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, nSlot++, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	if (nVertexElementType & VERTEX_INSTANCING_ELEMENT)
	{
		m_pd3dInputElementDescs[nIndex++] = { "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, nSlot, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		m_pd3dInputElementDescs[nIndex++] = { "INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, nSlot, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		m_pd3dInputElementDescs[nIndex++] = { "INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, nSlot, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
		m_pd3dInputElementDescs[nIndex++] = { "INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, nSlot, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
	}
}

void CShader::GetShaderName(UINT nVertexElementType, LPCSTR *ppszVSShaderName, LPCSTR *ppszVSShaderModel, LPCSTR *ppszPSShaderName, LPCSTR *ppszPSShaderModel)
{
	int nInputElements = 0, nIndex = 0;
	if (nVertexElementType == (VERTEX_POSITION_ELEMENT | VERTEX_COLOR_ELEMENT))
	{ *ppszVSShaderName = "VSDiffusedColor", *ppszPSShaderName = "PSDiffusedColor"; }
	if (nVertexElementType == (VERTEX_POSITION_ELEMENT | VERTEX_NORMAL_ELEMENT)) 
	{ *ppszVSShaderName = "VSLightingColor", *ppszPSShaderName = "PSLightingColor"; }
	if (nVertexElementType == (VERTEX_POSITION_ELEMENT | VERTEX_TEXTURE_ELEMENT_0)) 
	{ *ppszVSShaderName = "VSTexturedColor", *ppszPSShaderName = "PSTexturedColor"; }
	if (nVertexElementType == (VERTEX_POSITION_ELEMENT | VERTEX_TEXTURE_ELEMENT_0 | VERTEX_TEXTURE_ELEMENT_1))
	{ *ppszVSShaderName = "VSDetailTexturedColor", *ppszPSShaderName = "PSDetailTexturedColor"; }
	if (nVertexElementType == (VERTEX_POSITION_ELEMENT | VERTEX_NORMAL_ELEMENT | VERTEX_TEXTURE_ELEMENT_0))
	{ *ppszVSShaderName = "VSTexturedLightingColor", *ppszPSShaderName = "PSTexturedLightingColor"; }
	if (nVertexElementType == (VERTEX_POSITION_ELEMENT | VERTEX_NORMAL_ELEMENT | VERTEX_TEXTURE_ELEMENT_0 | VERTEX_TEXTURE_ELEMENT_1))
	{ *ppszVSShaderName = "VSDetailTexturedLightingColor", *ppszPSShaderName = "PSDetailTexturedLightingColor"; }
	if (nVertexElementType == (VERTEX_POSITION_ELEMENT | VERTEX_NORMAL_ELEMENT | VERTEX_BONE_ID_ELEMENT | VERTEX_BONE_WEIGHT_ELEMENT)) 
	{ *ppszVSShaderName = "VSSkinnedLightingColor", *ppszPSShaderName = "PSLightingColor"; }
	if (nVertexElementType == (VERTEX_POSITION_ELEMENT | VERTEX_NORMAL_ELEMENT | VERTEX_TEXTURE_ELEMENT_0 | VERTEX_BONE_ID_ELEMENT | VERTEX_BONE_WEIGHT_ELEMENT)) 
	{ *ppszVSShaderName = "VSSkinnedTexturedLightingColor", *ppszPSShaderName = "PSTexturedLightingColor"; }
	if (nVertexElementType == (VERTEX_POSITION_ELEMENT | VERTEX_NORMAL_ELEMENT | VERTEX_TEXTURE_ELEMENT_0 | VERTEX_INSTANCING_ELEMENT)) 
	{ *ppszVSShaderName = "VSInstancedTexturedLightingColor", *ppszPSShaderName = "PSInstancedTexturedLightingColor"; }

}

void CShader::OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
	pd3dDeviceContext->GSSetShader(m_pd3dGeometryShader, NULL, 0);
}

void CShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
}

ID3D11Buffer *CShader::CreateBuffer(ID3D11Device *pd3dDevice, UINT nStride, int nElements, void *pBufferData, UINT nBindFlags, D3D11_USAGE d3dUsage, UINT nCPUAccessFlags)
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

	ID3D11Buffer *pd3dInstanceBuffer = NULL;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, (pBufferData) ? &d3dBufferData : NULL, &pd3dInstanceBuffer);
	return(pd3dInstanceBuffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

void CTerrainShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSTerrainDetailTexturedLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTerrainDetailTexturedLightingColor", "ps_5_0", &m_pd3dPixelShader);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkyBoxShader::CSkyBoxShader()
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}

void CSkyBoxShader::CreateShader(ID3D11Device *pd3dDevice)
{
#ifdef _WITH_SKYBOX_TEXTURE_CUBE
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSSkyBoxTexturedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSSkyBoxTexturedColor", "ps_5_0", &m_pd3dPixelShader);
#else
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSTexturedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSSkyBoxTexturedColor", "ps_5_0", &m_pd3dPixelShader);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CWaterShader::CWaterShader()
{
}

CWaterShader::~CWaterShader()
{
}

void CWaterShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSAnimatedDetailTexturedLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSDetailTexturedLightingColor", "ps_5_0", &m_pd3dPixelShader);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CObjectsShader::CObjectsShader(int nObjects)
{
	m_ppObjects = NULL;

	m_nObjects = nObjects;
	if (m_nObjects > 0)
	{
		m_ppObjects = new CGameObject*[m_nObjects];
		for (int i = 0; i < m_nObjects; i++) m_ppObjects[i] = NULL;
	}

	m_pMaterial = NULL;
	m_pContext = NULL;

	m_nIndexToAdd = 0;
}

CObjectsShader::~CObjectsShader()
{
}

void CObjectsShader::SetObject(int nIndex, CGameObject *pGameObject)
{
	if (m_ppObjects[nIndex]) m_ppObjects[nIndex]->Release();
	m_ppObjects[nIndex] = pGameObject;
	if (pGameObject) pGameObject->AddRef();
}

void CObjectsShader::AddObject(CGameObject *pGameObject)
{
	m_ppObjects[m_nIndexToAdd++] = pGameObject;
	if (pGameObject) pGameObject->AddRef();
}

void CObjectsShader::SetMaterial(CMaterial *pMaterial)
{
	m_pMaterial = pMaterial;
	if (pMaterial) pMaterial->AddRef();
}

void CObjectsShader::BuildObjects(ID3D11Device *pd3dDevice, void *pContext)
{
	m_pContext = pContext;
}

void CObjectsShader::ReleaseObjects()
{
	if (m_pMaterial) m_pMaterial->Release();

	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->Release();
		delete[] m_ppObjects;
	}
}

void CObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed, NULL);
	}
}

void CObjectsShader::OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext)
{
	CShader::OnPrepareRender(pd3dDeviceContext);
}

void CObjectsShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::Render(pd3dDeviceContext, pCamera);

	if (m_pMaterial) m_pMaterial->UpdateShaderVariable(pd3dDeviceContext);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				m_ppObjects[j]->Render(pd3dDeviceContext, pCamera);
			}
		}
	}
}

CGameObject *CObjectsShader::PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	int nIntersected = 0;
	float fNearHitDistance = FLT_MAX;
	CGameObject *pSelectedObject = NULL;
	MESHINTERSECTINFO d3dxIntersectInfo;
	for (int i = 0; i < m_nObjects; i++)
	{
		nIntersected = m_ppObjects[i]->PickObjectByRayIntersection(pd3dxvPickPosition, pd3dxmtxView, &d3dxIntersectInfo);
		if ((nIntersected > 0) && (d3dxIntersectInfo.m_fDistance < fNearHitDistance))
		{
			fNearHitDistance = d3dxIntersectInfo.m_fDistance;
			pSelectedObject = m_ppObjects[i];
			if (pd3dxIntersectInfo) *pd3dxIntersectInfo = d3dxIntersectInfo;
		}
	}
	return(pSelectedObject);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CInstancedObjectsShader::CInstancedObjectsShader(int nObjects) : CObjectsShader(nObjects)
{
	m_nInstanceBufferStride = sizeof(D3DXMATRIX);
	m_nInstanceBufferOffset = 0;
	m_pd3dInstanceBuffer = NULL;

	m_pMesh = NULL;

	m_nType = VERTEX_INSTANCING_ELEMENT;
}

CInstancedObjectsShader::~CInstancedObjectsShader()
{
}

void CInstancedObjectsShader::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CInstancedObjectsShader::CreateShader(ID3D11Device *pd3dDevice)
{
	if (m_pMesh) CreateShader(pd3dDevice, m_pMesh->GetType());
}

void CInstancedObjectsShader::CreateShader(ID3D11Device *pd3dDevice, UINT nType)
{
	CObjectsShader::CreateShader(pd3dDevice, nType);
}

void CInstancedObjectsShader::BuildObjects(ID3D11Device *pd3dDevice, void *pContext)
{
	CObjectsShader::BuildObjects(pd3dDevice, pContext);

	m_pd3dInstanceBuffer = CreateBuffer(pd3dDevice, m_nInstanceBufferStride, m_nObjects, NULL, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	m_pMesh->AssembleToVertexBuffer(1, &m_pd3dInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);
}

void CInstancedObjectsShader::ReleaseObjects()
{
	CObjectsShader::ReleaseObjects();

	if (m_pMesh) m_pMesh->Release();

	if (m_pd3dInstanceBuffer) m_pd3dInstanceBuffer->Release();
}

void CInstancedObjectsShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);

	if (m_pMaterial) m_pMaterial->UpdateShaderVariable(pd3dDeviceContext);

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;

	int nInstances = 0;
	pd3dDeviceContext->Map(m_pd3dInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	D3DXMATRIX *pd3dxmtxInstances = (D3DXMATRIX *)d3dMappedResource.pData;
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				D3DXMatrixTranspose(&pd3dxmtxInstances[nInstances++], &m_ppObjects[j]->m_d3dxmtxWorld);
			}
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dInstanceBuffer, 0);

	m_pMesh->RenderInstanced(pd3dDeviceContext, nInstances, 0);
}

CTitileShader::CTitileShader()
{
}

CTitileShader::~CTitileShader()
{
}

void CTitileShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "defferdVS", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	//CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTexturedLightingColor", "ps_5_0", &m_pd3dPixelShader);
	//CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "deferredshadingShaderVS", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTitileColor", "ps_5_0", &m_pd3dPixelShader);
	cout << "이건오냐";
}


CBoundingCubeShader::CBoundingCubeShader()
{
}

CBoundingCubeShader::~CBoundingCubeShader()
{
}

void CBoundingCubeShader::CreateShader(ID3D11Device * pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSDiffusedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSDiffusedColor", "ps_5_0", &m_pd3dPixelShader);
}


CAnimationShader::CAnimationShader() : CShader()
{
}

CAnimationShader::~CAnimationShader()
{
}

void CAnimationShader::CreateAnimationBuffer(ID3D11Device * pd3dDevice)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_SKINNED);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbAnimation);
}

void CAnimationShader::CreateShader(ID3D11Device * pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSINSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCESLOT", 0, DXGI_FORMAT_R32_UINT, 5, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSAnimation", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSAnimation", "ps_5_0", &m_pd3dPixelShader);
}

void CAnimationShader::BuildObjects(ID3D11Device * pd3dDevice, CMarineMeshDiffused *pMesh)
{
	//	CSkinnedMesh* pMesh = new CSkinnedMesh(pd3dDevice, MODEL_DATA, MODEL_SCALING);

	m_nObjects = 1;
	m_ppObjects = new CAnimationObject*[m_nObjects];
	//	m_ppObjects = new CTerrainPlayer*[m_nObjects];
	pMesh->AddRef();
	for (int i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i] = new CAnimationObject();
		m_ppObjects[i]->AddRef();
		m_ppObjects[i]->SetMesh(pMesh);
		m_ppObjects[i]->m_fAnimationElapsedTime = 1.0f * i;
		m_ppObjects[i]->m_d3dxmtxWorld._41 = 1000.0f;
		m_ppObjects[i]->m_d3dxmtxWorld._42 = 416.0f;
		m_ppObjects[i]->m_d3dxmtxWorld._43 = 1000.0f;

		//애니메이션 Set
		m_ppObjects[i]->m_mapAnimationSet.insert(make_pair("Idle", make_pair(0, 50)));
		m_ppObjects[i]->m_mapAnimationSet.insert(make_pair("Attack", make_pair(60, 110)));
		m_ppObjects[i]->m_mapAnimationSet.insert(make_pair("Move", make_pair(120, 170)));
		m_ppObjects[i]->m_mapAnimationSet.insert(make_pair("Die", make_pair(180, 230)));
		//	m_ppObjects[i]->m_mapAnimationSet.insert(make_pair("Extra", make_pair(231, 350)));


		m_ppObjects[i]->m_strCurrentAnimation = "Move";
	}

	//**************************************
	//인스턴스 버퍼 생성
	ID3D11Buffer* pd3dInstanceBuffer = nullptr;
	UINT m_nInstanceBufferStride = sizeof(VS_INSTANCE_WITH_SLOT);
	//UINT m_nInstanceBufferStride = sizeof(D3DXMATRIX);
	UINT m_nInstanceBufferOffset = 0;

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = m_nInstanceBufferStride * m_nObjects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = nullptr;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &pd3dInstanceBuffer);

	m_pd3dInstanceBuffer = pd3dInstanceBuffer;
	pMesh->AssembleToVertexBuffer(1, &pd3dInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

	//******************************************
	//애니메이션을 위한 StructuredBuffer 생성
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_SRV_BUFFER_ANIMATION) * MAX_KINDS_OBJECTS;	// 64 * 32 * 64 = 128KB

	bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.StructureByteStride = sizeof(VS_SRV_BUFFER_ANIMATION);
	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&subData, sizeof(subData));
	subData.pSysMem = nullptr;
	HRESULT h = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pd3dSRVAnimationBuffer);
	cout << h << endl;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_UNKNOWN;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvd.BufferEx.NumElements = MAX_KINDS_OBJECTS;
	pd3dDevice->CreateShaderResourceView(m_pd3dSRVAnimationBuffer, &srvd, &m_pd3dSRVAnimation);

	//**************************************
	//셰이더 리소스 생성
	ID3D11SamplerState *pd3dSamplerState = NULL;

	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
	h = D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("marine/marine_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	cout << h << endl;
	m_pd3dModelSamplerState = pd3dSamplerState;
	m_pd3dModelTexture = pd3dsrvTexture;
}

void CAnimationShader::UpdateAnimationBuffer(ID3D11DeviceContext * pd3dDeviceContext, D3DXMATRIX * pd3dxmtxAnimation, D3DXMATRIX * pd3dxmtxLocal, int nBone)
{
	// 상수버퍼로 최종 행렬값을 넘겨주자.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbAnimation, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_SKINNED *pcbBones = (VS_CB_SKINNED*)d3dMappedResource.pData;

	D3DXMATRIX mtxAnimation;
	for (int i = 0; i < nBone; i++)
	{
		mtxAnimation = pd3dxmtxAnimation[i];
		D3DXMatrixTranspose(&(pcbBones->m_d3dxmtxBone[i]), &mtxAnimation);
		//D3DXMatrixIdentity(&pcbBones->m_d3dxmtxBone[i]);
	}
	//World행렬과 본행렬사이의 <캐릭터 위치세팅용 행렬> 전달, 나중에 좀 더 최적화 방법을 찾아보겠다.
	D3DXMatrixTranspose(&pcbBones->m_d3dxmtxBone[MAX_BONE_COUNT - 1], pd3dxmtxLocal);
	pd3dDeviceContext->Unmap(m_pd3dcbAnimation, 0);

	//상수 버퍼를 슬롯(VS_SLOT_SKINNEDBONE)에 설정한다.
	pd3dDeviceContext->VSSetConstantBuffers(0x05, 1, &m_pd3dcbAnimation);
}

void CAnimationShader::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->Animate(fTimeElapsed);
	}
}

void CAnimationShader::Render(ID3D11DeviceContext * pd3dDeviceContext, D3DXMATRIX *pPlayermtx)
{
	OnPrepareRender(pd3dDeviceContext);

	if (m_pd3dModelSamplerState)
		pd3dDeviceContext->PSSetSamplers(0x000, 1, &m_pd3dModelSamplerState);
	if (m_pd3dModelTexture)
		pd3dDeviceContext->PSSetShaderResources(0x000, 1, &m_pd3dModelTexture);

	D3DXMATRIX *pd3dxmtxLocal = nullptr;
	D3DXMATRIX *pd3dxmtxAnimation = nullptr;
	int	nBoneCount;
	/*
	for (int i = 0; i < m_nObjects; ++i)
	{
	int nBone = m_ppObjects[i]->m_pMesh->GetNumBones();

	UpdateShaderVariable(pd3dDeviceContext, &(m_ppObjects[i]->m_d3dxmtxWorld));
	pd3dxmtxLocal = &(m_ppObjects[i]->m_pMesh->GetLocalMatrix());
	pd3dxmtxAnimation = m_ppObjects[i]->m_pMesh->GetBoneTransform();
	UpdateAnimationBuffer(pd3dDeviceContext, pd3dxmtxAnimation, pd3dxmtxLocal, nBone);

	m_ppObjects[i]->Render(pd3dDeviceContext, nullptr);
	}
	*/

	/*
	pd3dxmtxLocal = &(m_ppObjects[0]->m_pMesh->GetLocalMatrix());
	pd3dxmtxAnimation = m_ppObjects[0]->m_pMesh->GetBoneTransform();
	UpdateAnimationBuffer(pd3dDeviceContext, pd3dxmtxAnimation, pd3dxmtxLocal, m_ppObjects[0]->m_pMesh->GetNumBones());
	*/

	//**************************************
	//월드변환행렬 인스턴스 버퍼 Map
	D3D11_MAPPED_SUBRESOURCE d3dMappsedResource;
	pd3dDeviceContext->Map(m_pd3dInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappsedResource);
	//D3DXMATRIX *pd3dxmtx = (D3DXMATRIX*)d3dMappsedResource.pData;
	VS_INSTANCE_WITH_SLOT *pd3dxmtx = (VS_INSTANCE_WITH_SLOT*)d3dMappsedResource.pData;
	for (UINT i = 0; i < m_nObjects; ++i)
	{
		//D3DXMatrixTranspose(&(pd3dxmtx[i]), &(m_ppObjects[i]->m_d3dxmtxWorld));	
		m_ppObjects[i]->m_d3dxmtxWorld = *pPlayermtx;
		D3DXMatrixTranspose(&(pd3dxmtx[i].m_d3dxmtxWorld), &(m_ppObjects[i]->m_d3dxmtxWorld));
		pd3dxmtx[i].nSlot = i;
	}
	pd3dDeviceContext->Unmap(m_pd3dInstanceBuffer, 0);

	//**************************************
	//애니메이션행렬 구조화버퍼 Map
	D3D11_MAPPED_SUBRESOURCE d3dSBMappsedResource;
	pd3dDeviceContext->Map(m_pd3dSRVAnimationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dSBMappsedResource);
	VS_SRV_BUFFER_ANIMATION *pStructuredData = (VS_SRV_BUFFER_ANIMATION*)d3dSBMappsedResource.pData;
	for (UINT i = 0; i < m_nObjects; ++i)
	{
		nBoneCount = m_ppObjects[i]->m_pMesh->GetNumBones();
		pd3dxmtxLocal = &(m_ppObjects[i]->m_pMesh->GetLocalMatrix());
		pd3dxmtxAnimation = m_ppObjects[i]->m_d3dxmtxFinalTransform;

		for (int j = 0; j < nBoneCount; ++j)
		{
			D3DXMatrixTranspose(&(pStructuredData[i].m_d3dxmtxAnimation[j]), &(pd3dxmtxAnimation[j]));
		}
		D3DXMatrixTranspose(&(pStructuredData[i].m_d3dxmtxAnimation[MAX_BONE_COUNT - 1]), pd3dxmtxLocal);
	}

	pd3dDeviceContext->Unmap(m_pd3dSRVAnimationBuffer, 0);
	pd3dDeviceContext->VSSetShaderResources(0x006, 1, &m_pd3dSRVAnimation);

	m_ppObjects[0]->m_pMesh->RenderInstanced(pd3dDeviceContext, m_nObjects, 0);
}

