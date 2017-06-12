#include "stdafx.h"
#include "TitileScene.h"
#include "resource.h"


CGameObject	**m_titleObjects;
CTitileScene::CTitileScene()
{
	m_ppObjectShaders = { nullptr };
//	m_ppObjects = { nullptr };
	m_nObjects = 0;

	m_ppObjectShaders = { nullptr };
	m_nObjectShaders = 0;


	m_pPlayer = nullptr;
	m_pCamera = nullptr;
}


CTitileScene::~CTitileScene()
{
}

void CTitileScene::BuildObjects(ID3D11Device * pd3dDevice)
{
	ID3D11SamplerState *pd3dSamplerState = NULL;
	ID3D11SamplerState *pd3dSamplerState1 = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	CMaterial *pTitileMaterial[9];
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	CMaterialColors  *pTitileColor = new CMaterialColors();
	pTitileColor->m_d3dxcDiffuse = D3DXCOLOR(1.0f, .0f, 1.0f, 1.0f);
	pTitileColor->m_d3dxcAmbient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pTitileColor->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pTitileColor->m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pTitileMaterial[0] = new CMaterial(pTitileColor);
	pTitileMaterial[1] = new CMaterial(pTitileColor);
	pTitileMaterial[2] = new CMaterial(pTitileColor);
	pTitileMaterial[3] = new CMaterial(pTitileColor);
	pTitileMaterial[4] = new CMaterial(pTitileColor);
	pTitileMaterial[5] = new CMaterial(pTitileColor);
	pTitileMaterial[6] = new CMaterial(pTitileColor);
	pTitileMaterial[7] = new CMaterial(pTitileColor);
	pTitileMaterial[8] = new CMaterial(pTitileColor);
	ID3D11ShaderResourceView *pTitileView = NULL;
	CTexture *pTitileTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("titile/title.png"), NULL, NULL, &pTitileView, NULL);
	pTitileTexture->SetTexture(0, pTitileView);
	pTitileTexture->SetSampler(0, pd3dSamplerState);
	pTitileView->Release();
	pTitileMaterial[0]->SetTexture(pTitileTexture);

	CTexture *pTitileTexture1 = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("titile/create.png"), NULL, NULL, &pTitileView, NULL);
	pTitileTexture1->SetTexture(0, pTitileView);
	pTitileTexture1->SetSampler(0, pd3dSamplerState);
	pTitileView->Release();
	pTitileMaterial[1]->SetTexture(pTitileTexture1);

	CTexture *pTitileTexture2 = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("titile/Join.png"), NULL, NULL, &pTitileView, NULL);
	pTitileTexture2->SetTexture(0, pTitileView);
	pTitileTexture2->SetSampler(0, pd3dSamplerState);
	pTitileView->Release();
	pTitileMaterial[2]->SetTexture(pTitileTexture2);

	CTexture *pTitileTexture3 = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("titile/EXIT.png"), NULL, NULL, &pTitileView, NULL);
	pTitileTexture3->SetTexture(0, pTitileView);
	pTitileTexture3->SetSampler(0, pd3dSamplerState);
	pTitileView->Release();
	pTitileMaterial[3]->SetTexture(pTitileTexture3);

	CTexture *pTitileTexture4 = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("titile/joinroom.png"), NULL, NULL, &pTitileView, NULL);
	pTitileTexture4->SetTexture(0, pTitileView);
	pTitileTexture4->SetSampler(0, pd3dSamplerState);
	pTitileView->Release();
	pTitileMaterial[4]->SetTexture(pTitileTexture4);


	CTexture *pTitileTexture5 = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("titile/lobby.png"), NULL, NULL, &pTitileView, NULL);
	pTitileTexture5->SetTexture(0, pTitileView);
	pTitileTexture5->SetSampler(0, pd3dSamplerState);
	pTitileView->Release();
	pTitileMaterial[5]->SetTexture(pTitileTexture5);


	CTexture *pTitileTexture6 = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("titile/start.png"), NULL, NULL, &pTitileView, NULL);
	pTitileTexture6->SetTexture(0, pTitileView);
	pTitileTexture6->SetSampler(0, pd3dSamplerState);
	pTitileView->Release();
	pTitileMaterial[6]->SetTexture(pTitileTexture6);

	CTexture *pTitileTexture7 = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("titile/watch.png"), NULL, NULL, &pTitileView, NULL);
	pTitileTexture7->SetTexture(0, pTitileView);
	pTitileTexture7->SetSampler(0, pd3dSamplerState);
	pTitileView->Release();
	pTitileMaterial[7]->SetTexture(pTitileTexture7);

	CTexture *pTitileTexture8 = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("titile/cross.png"), NULL, NULL, &pTitileView, NULL);
	pTitileTexture8->SetTexture(0, pTitileView);
	pTitileTexture8->SetSampler(0, pd3dSamplerState);
	pTitileView->Release();
	pTitileMaterial[8]->SetTexture(pTitileTexture8);
	//pMarineView->Release();

	CMesh *pMeshes = nullptr;
	pMeshes = new CTitileMesh(pd3dDevice, -1.0f, 1.0f, -1.0f, 1.0f);

	CShader *pTitileShader = new CTitileShader();
	pTitileShader->CreateShader(pd3dDevice);


	m_nObjects = 15;
	m_titleObjects = new CGameObject*[m_nObjects];
	CGameObject		*pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[0]);
	pTitileObject->SetMesh(pMeshes);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[0] = pTitileObject;
	m_titleObjects[0]->titlevisivle = true;

	CMesh *pMeshes1 = new CTitileMesh(pd3dDevice, -0.22, 0.27, -0.15, 0.2);
	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[1]);
	pTitileObject->SetMesh(pMeshes1);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[1] = pTitileObject;

	CMesh *pMeshes2 = new CTitileMesh(pd3dDevice, -0.22, 0.27, -0.40, -0.05);
	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[2]);
	pTitileObject->SetMesh(pMeshes2);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[2] = pTitileObject;

	CMesh *pMeshes3 = new CTitileMesh(pd3dDevice, -0.22, 0.27, -0.60, -0.25);
	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[3]);
	pTitileObject->SetMesh(pMeshes3);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[3] = pTitileObject;


	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[4]);
	pTitileObject->SetMesh(pMeshes);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[4] = pTitileObject;


	CMesh *pMeshes4 = new CTitileMesh(pd3dDevice, 0.39, 0.93, -0.92, -0.7);
	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[3]);
	pTitileObject->SetMesh(pMeshes4);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[5] = pTitileObject;

	CMesh *pMeshes5 = new CTitileMesh(pd3dDevice, 0.39, 0.93, -0.78, -0.55);
	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[2]);
	pTitileObject->SetMesh(pMeshes5);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[6] = pTitileObject;



	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[5]);
	pTitileObject->SetMesh(pMeshes);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[7] = pTitileObject;


	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[3]);
	pTitileObject->SetMesh(pMeshes4);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[8] = pTitileObject;

	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[7]);
	pTitileObject->SetMesh(pMeshes5);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[9] = pTitileObject;

	CMesh *pMeshes6 = new CTitileMesh(pd3dDevice, 0.39, 0.93, -0.63, -0.40);
	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[6]);
	pTitileObject->SetMesh(pMeshes6);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[10] = pTitileObject;

	CMesh *pMeshes7 = new CTitileMesh(pd3dDevice, -0.005, 0.005, 0.03, 0.1);
	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[8]);
	pTitileObject->SetMesh(pMeshes7);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[11] = pTitileObject;

	CMesh *pMeshes8 = new CTitileMesh(pd3dDevice, -0.005, 0.005, -0.1, -0.03);
	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[8]);
	pTitileObject->SetMesh(pMeshes8);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[12] = pTitileObject;

	CMesh *pMeshes9 = new CTitileMesh(pd3dDevice, -0.08, -0.03, -0.007, 0.007);
	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[8]);
	pTitileObject->SetMesh(pMeshes9);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[13] = pTitileObject;

	CMesh *pMeshes10 = new CTitileMesh(pd3dDevice, 0.03, 0.08, -0.007, 0.007);
	pTitileObject = new CGameObject();
	pTitileObject->SetMaterial(pTitileMaterial[8]);
	pTitileObject->SetMesh(pMeshes10);
	pTitileObject->SetShader(pTitileShader);
	m_titleObjects[14] = pTitileObject;

	/*



	HRESULT T1 = D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("titile/create.png"), NULL, NULL, &pTitileView1, NULL);
	pTitileMaterial1->SetTexture(pTitileTexture1);
	pTitileTexture1->SetSampler(0, pd3dSamplerState1);
	pTitileMaterial1->m_pTexture->SetTexture(0, pTitileView1);
	pTitileObject[1].SetMaterial(pTitileMaterial1);
	pTitileObject[1].SetMesh(pMeshes1);

	pTitileObject[1].SetShader(pTitileShader);
	m_ppObjects[1] = &pTitileObject[1];*/
	/*

	pTitileMaterial->m_pTexture->SetTexture(1, pTitileView[1]);

	pTitileObject->SetMesh(pMeshes1);
	m_ppObjects[1] = pTitileObject;
	/*

	//	pTitileTexture[1] = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	//pTitileMaterial[1] = new CMaterial(pTitileColor);
	CMesh *pMeshes1 = new CTitileMesh(pd3dDevice, -0.2, 0.5, -0.2, 0.5);

	// pTitileMaterial[1]->SetTexture(pTitileTexture[1]);
	pTitileTexture[1]->SetSampler(0, pd3dSamplerState);
	//pTitileMaterial[1]->m_pTexture->SetTexture(0, pTitileView[1]);
	//pTitileObject[1].SetMaterial(pTitileMaterial[1]);

	pTitileObject[1].SetShader(pTitileShader);
	m_ppObjects[1] = &pTitileObject[1];


	//HRESULT T = D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("titile/Join.png"), NULL, NULL, &pTitileView, NULL);
	//HRESULT T = D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("titile/EXIT.png"), NULL, NULL, &pTitileView, NULL);*/
	//CreateShaderVariables(pd3dDevice);

}

void CTitileScene::Render(ID3D11DeviceContext * pd3dDeviceContext, CCamera * pCamera)
{

	//	if (m_pLights && m_pd3dcbLights) UpdateShaderVariable(pd3dDeviceContext, m_pLights);


	for (int j = m_nObjects - 1; j >-1; j--)
	{
		if (m_titleObjects[j]->titlevisivle) {
			m_titleObjects[j]->Render(pd3dDeviceContext, pCamera);
		}
		//m_ppObjects[0]->Render(pd3dDeviceContext, pCamera); //SkyBox
		//SkyBox

	}
	//	for (int i = 0; i < m_nObjectShaders; i++) 
	//m_ppObjectShaders[0]->Render(pd3dDeviceContext, pCamera);
	//	for (int i = 0; i < m_nInstancingShaders; i++) m_ppInstancingShaders[i]->Render(pd3dDeviceContext, pCamera);
}


