//-----------------------------------------------------------------------------
// File: Scene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"
#include "resource.h"

CInstancedObjectsShader			**m_ppInstancingShaders;
CHeightMapTerrain *pTerrain;

CScene::CScene()
{
	m_ppObjects = NULL;
	m_nObjects = 0;

	m_ppObjectShaders = NULL;
	m_nObjectShaders = 0;

	m_ppInstancingShaders = NULL;
	m_nInstancingShaders = 0;

	m_pLights = NULL;
	m_pd3dcbLights = NULL;

	m_pCamera = NULL;
	m_pSelectedObject = NULL;
	
}

CScene::~CScene()
{
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		m_pSelectedObject = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONUP:
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

void CScene::OnChangeSkyBoxTextures(ID3D11Device *pd3dDevice, CMaterial *pMaterial, int nIndex)
{
#ifdef _WITH_SKYBOX_TEXTURE_ARRAY
	_TCHAR ppstrTextureNames[6][128];
	_stprintf_s(ppstrTextureNames[0], _T("../Assets/Image/SkyBox/SkyBox_Front_%d.dds"), nIndex, 128);
	_stprintf_s(ppstrTextureNames[1], _T("../Assets/Image/SkyBox/SkyBox_Back_%d.dds"), nIndex, 128);
	_stprintf_s(ppstrTextureNames[2], _T("../Assets/Image/SkyBox/SkyBox_Left_%d.dds"), nIndex, 128);
	_stprintf_s(ppstrTextureNames[3], _T("../Assets/Image/SkyBox/SkyBox_Right_%d.dds"), nIndex, 128);
	_stprintf_s(ppstrTextureNames[4], _T("../Assets/Image/SkyBox/SkyBox_Top_%d.dds"), nIndex, 128);
	_stprintf_s(ppstrTextureNames[5], _T("../Assets/Image/SkyBox/SkyBox_Bottom_%d.dds"), nIndex, 128);
	ID3D11ShaderResourceView *pd3dsrvTexture = CTexture::CreateTexture2DArraySRV(pd3dDevice, ppstrTextureNames, 6);
	pMaterial->m_pTexture->SetTexture(0, pd3dsrvTexture); pd3dsrvTexture->Release();
#else
#ifdef _WITH_SKYBOX_TEXTURE_CUBE
	_TCHAR pstrTextureNames[128];
	_stprintf_s(pstrTextureNames, _T("Assets/Image/SkyBox/SkyBox_%d.dds"), nIndex, 128);
	ID3D11ShaderResourceView *pd3dsrvTexture = nullptr;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureNames, NULL, NULL, &pd3dsrvTexture, NULL);
	pMaterial->m_pTexture->SetTexture(0, pd3dsrvTexture); pd3dsrvTexture->Release();
#else
	_TCHAR pstrTextureName[80];
	_stprintf_s(pstrTextureName, _T("../Assets/Image/SkyBox/SkyBox_Front_%d.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	pMaterial->m_pTexture->SetTexture(0, pd3dsrvTexture); pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("../Assets/Image/SkyBox/SkyBox_Back_%d.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	pMaterial->m_pTexture->SetTexture(1, pd3dsrvTexture); pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("../Assets/Image/SkyBox/SkyBox_Left_%d.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	pMaterial->m_pTexture->SetTexture(2, pd3dsrvTexture); pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("../Assets/Image/SkyBox/SkyBox_Right_%d.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	pMaterial->m_pTexture->SetTexture(3, pd3dsrvTexture); pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("../Assets/Image/SkyBox/SkyBox_Top_%d.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	pMaterial->m_pTexture->SetTexture(4, pd3dsrvTexture); pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("../Assets/Image/SkyBox/SkyBox_Bottom_%d.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	pMaterial->m_pTexture->SetTexture(5, pd3dsrvTexture); pd3dsrvTexture->Release();
#endif
#endif
}

void CScene::CreateBullet()
{


}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;// 0.0   1.0 이하,이상을 0.0   1.0으로 맞춤
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//축소,배율 ,레벨 샘플링에 선형보간을 사용
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;//비교를 하지 않음
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);


#ifdef _WITH_SKYBOX_TEXTURE_ARRAY
	CTexture *pSkyboxTexture = new CTexture(1, 1, PS_SLOT_TEXTURE_SKYBOX, PS_SLOT_SAMPLER_SKYBOX);
#else
#ifdef _WITH_SKYBOX_TEXTURE_CUBE
	CTexture *pSkyboxTexture = new CTexture(1, 1, PS_SLOT_TEXTURE_SKYBOX, PS_SLOT_SAMPLER_SKYBOX);
#else
	CTexture *pSkyboxTexture = new CTexture(6, 1, PS_SLOT_TEXTURE_SKYBOX, PS_SLOT_SAMPLER_SKYBOX);
#endif
#endif
	pSkyboxTexture->SetSampler(0, pd3dSamplerState);
	pd3dSamplerState->Release();

	

	pd3dSamplerState = NULL;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);
	ID3D11ShaderResourceView *pMarineView = NULL;
	CTexture *pMarineTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	CMaterial *pMarineMaterial[4] = {nullptr};
	CMaterialColors  *pMarineColor= new CMaterialColors();
	pMarineColor->m_d3dxcDiffuse= D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pMarineColor->m_d3dxcAmbient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pMarineColor->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pMarineColor->m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pMarineMaterial[0] = new CMaterial(pMarineColor);

	CMaterialColors  *pRMarineColor = new CMaterialColors();
	pRMarineColor->m_d3dxcDiffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pRMarineColor->m_d3dxcAmbient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pRMarineColor->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pRMarineColor->m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pMarineMaterial[1] = new CMaterial(pRMarineColor);

	CMaterialColors  *pGMarineColor = new CMaterialColors();
	pGMarineColor->m_d3dxcDiffuse = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	pGMarineColor->m_d3dxcAmbient = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	pGMarineColor->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 3.0f);
	pGMarineColor->m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pMarineMaterial[2] = new CMaterial(pGMarineColor);


	CMaterialColors  *pBMarineColor = new CMaterialColors();
	pBMarineColor->m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.f);
	pBMarineColor->m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pBMarineColor->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 4.0f);
	pBMarineColor->m_d3dxcEmissive = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	pMarineMaterial[3] = new CMaterial(pBMarineColor);

	//CTexture *pTitileTexture5 = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	//D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("titile/lobby.png"), NULL, NULL, &pTitileView, NULL);
	//pTitileTexture5->SetTexture(0, pTitileView);
	//pTitileTexture5->SetSampler(0, pd3dSamplerState);
	
	//pTitileMaterial[5]->SetTexture(pTitileTexture5);


	//마린 marterial   0,1,2,3는  일반, 빨, 녹 ,청 
	CTexture *pMarineTexture1 = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("marine/marine_diffuse.png"), NULL, NULL, &pMarineView, NULL);
	pMarineTexture1->SetTexture(0, pMarineView);
	pMarineTexture1->SetSampler(0, pd3dSamplerState);
	pMarineView->Release();
	pMarineMaterial[0]->SetTexture(pMarineTexture1);
	CTexture *pMarineTexture2= new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("marine/marine_green.png"), NULL, NULL, &pMarineView, NULL);
	pMarineTexture2->SetTexture(0, pMarineView);
	pMarineTexture2->SetSampler(0, pd3dSamplerState);
	pMarineView->Release();
	pMarineMaterial[1]->SetTexture(pMarineTexture2);
	CTexture *pMarineTexture3 = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("marine/marine_blue.png"), NULL, NULL, &pMarineView, NULL);
	pMarineTexture3->SetTexture(0, pMarineView);
	pMarineTexture3->SetSampler(0, pd3dSamplerState);
	pMarineView->Release();
	pMarineMaterial[2]->SetTexture(pMarineTexture3);

	/*
	HRESULT T = D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("marine/marine_diffuse.png"), NULL, NULL, &pMarineView, NULL);
	for (int i = 0; i < 4; i++)
	{

		pMarineMaterial[i]->SetTexture(pMarineTexture);
		if (i == 0) {

			
		}
		else if (i == 1)
		{
			HRESULT T = D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("marine/marine_green.png"), NULL, NULL, &pMarineView, NULL);
		}
		else if (i == 2) {
			HRESULT T = D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("marine/marine_blue.png"), NULL, NULL, &pMarineView, NULL);
		}
	
		pMarineTexture->SetSampler(0, pd3dSamplerState);
	}*/

	//Set을 무엇으로 할것인지로  색을 정함(0~3)
	m_pPlayer->SetMaterial(pMarineMaterial[1]);

	ID3D11ShaderResourceView *pBulletView = NULL;
	CTexture *pBulletTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	CMaterial *pBulletMaterial=  nullptr ;
	CMaterialColors  *pBulletColor = new CMaterialColors();
	pBulletColor->m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pBulletColor->m_d3dxcAmbient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pBulletColor->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pBulletColor->m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pBulletMaterial = new CMaterial(pBulletColor);

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("bullet/bullet_diffuse.png"), NULL, NULL, &pBulletView, NULL);
	
		pBulletMaterial->SetTexture(pBulletTexture);
		pBulletMaterial->m_pTexture->SetTexture(0, pBulletView);
		pBulletTexture->SetSampler(0, pd3dSamplerState);
	
		
		m_pBullet->SetMaterial(pBulletMaterial);


	CMaterial *pSkyboxMaterial = new CMaterial(NULL);
	pSkyboxMaterial->SetTexture(pSkyboxTexture);
	OnChangeSkyBoxTextures(pd3dDevice, pSkyboxMaterial, 0);

	CSkyBoxMesh *pSkyBoxMesh = new CSkyBoxMesh(pd3dDevice, 20.0f, 20.0f, 20.0f);
	CSkyBox *pSkyBox = new CSkyBox(pd3dDevice);
	pSkyBox->SetMesh(pSkyBoxMesh, 0);
	pSkyBox->SetMaterial(pSkyboxMaterial);

	CShader *pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice);
	pSkyBox->SetShader(pSkyBoxShader);

	ID3D11SamplerState *pd3dBaseSamplerState = NULL;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dBaseSamplerState);

	ID3D11SamplerState *pd3dDetailSamplerState = NULL;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dDetailSamplerState);

	CTexture *pTerrainTexture = new CTexture(2, 2, PS_SLOT_TEXTURE_TERRAIN, PS_SLOT_SAMPLER_TERRAIN);

	ID3D11ShaderResourceView *pd3dsrvBaseTexture = NULL;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/Terrain/1.jpg"), NULL, NULL, &pd3dsrvBaseTexture, NULL);
	pTerrainTexture->SetTexture(0, pd3dsrvBaseTexture);
	pTerrainTexture->SetSampler(0, pd3dBaseSamplerState);
	pd3dsrvBaseTexture->Release();
	pd3dBaseSamplerState->Release();

	ID3D11ShaderResourceView *pd3dsrvDetailTexture = NULL;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/Terrain/stones.bmp"), NULL, NULL, &pd3dsrvDetailTexture, NULL);
	pTerrainTexture->SetTexture(1, pd3dsrvDetailTexture);
	pTerrainTexture->SetSampler(1, pd3dDetailSamplerState);
	pd3dsrvDetailTexture->Release();
	pd3dDetailSamplerState->Release();

	CMaterialColors *pTerrainColors = new CMaterialColors();
	pTerrainColors->m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pTerrainColors->m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pTerrainColors->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pTerrainColors->m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	CMaterial *pTerrainMaterial = new CMaterial(pTerrainColors);
	pTerrainMaterial->SetTexture(pTerrainTexture);

	D3DXVECTOR3 d3dxvScale(8.0f, 2.0f, 8.0f);
#ifdef _WITH_TERRAIN_PARTITION
	CHeightMapTerrain *pTerrain = new CHeightMapTerrain(pd3dDevice, _T("../Assets/Image/Terrain/HeightMap.raw"), 257, 257, 17, 17, d3dxvScale);
#else
	CHeightMapTerrain *pTerrain = new CHeightMapTerrain(pd3dDevice, _T("../Assets/Image/Terrain/HeightMap.raw"), 257, 257, 257, 257, d3dxvScale);
#endif
	pTerrain->SetMaterial(pTerrainMaterial);

	CShader *pTerrainShader = new CTerrainShader();
	pTerrainShader->CreateShader(pd3dDevice);
	pTerrain->SetShader(pTerrainShader);

	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dBaseSamplerState);

	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dDetailSamplerState);

	CTexture *pTerrainWaterTexture = new CTexture(2, 2, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("../Assets/Image/Terrain/Water_Base_Texture_0.dds"), NULL, NULL, &pd3dsrvBaseTexture, NULL);
	pTerrainWaterTexture->SetTexture(0, pd3dsrvBaseTexture);
	pTerrainWaterTexture->SetSampler(0, pd3dBaseSamplerState);
	pd3dsrvBaseTexture->Release();
	pd3dBaseSamplerState->Release();

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("../Assets/Image/Terrain/Detail_Texture_1.jpg"), NULL, NULL, &pd3dsrvDetailTexture, NULL);
	//	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("../Assets/Image/Terrain/Water_Detail_Texture_0.dds"), NULL, NULL, &pd3dsrvDetailTexture, NULL);
	pTerrainWaterTexture->SetTexture(1, pd3dsrvDetailTexture);
	pTerrainWaterTexture->SetSampler(1, pd3dDetailSamplerState);
	pd3dsrvDetailTexture->Release();
	pd3dDetailSamplerState->Release();

	CMaterialColors *pWaterColors = new CMaterialColors();
	pWaterColors->m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pWaterColors->m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pWaterColors->m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.3f, 30.0f);
	pWaterColors->m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.2f, 1.0f);

	CMaterial *pTerrainWaterMaterial = new CMaterial(pWaterColors);
	pTerrainWaterMaterial->SetTexture(pTerrainWaterTexture);

	CTerrainWater *pTerrainWater = new CTerrainWater(pd3dDevice, 257, 257, 17, 17, d3dxvScale);
	pTerrainWater->SetMaterial(pTerrainWaterMaterial);
	pTerrainWater->SetPosition(0.0f, 80.0f, 0.0f);

	CShader *pTerrainWaterShader = new CWaterShader();
	pTerrainWaterShader->CreateShader(pd3dDevice);
	pTerrainWater->SetShader(pTerrainWaterShader);

	m_nObjects = 2;
	m_ppObjects = new CGameObject*[m_nObjects];

	m_ppObjects[0] = pSkyBox;
	m_ppObjects[1] = pTerrain;
	//m_ppObjects[2] = pTerrainWater;

	//Instancing
	CMaterial *pInstancingMaterials[6] = {nullptr};

	
	CMaterialColors *pColor = new CMaterialColors();
	pColor->m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pColor->m_d3dxcAmbient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pColor->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pColor->m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pInstancingMaterials[0] = new CMaterial(pColor);
	
	
	CMaterialColors *pRedColor = new CMaterialColors();
	pRedColor->m_d3dxcDiffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pRedColor->m_d3dxcAmbient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pRedColor->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pRedColor->m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pInstancingMaterials[1] = new CMaterial(pRedColor);

	CMaterialColors *pGreenColor = new CMaterialColors();
	pGreenColor->m_d3dxcDiffuse = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	pGreenColor->m_d3dxcAmbient = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	pGreenColor->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 3.0f);
	pGreenColor->m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pInstancingMaterials[2] = new CMaterial(pGreenColor);

	CMaterialColors *pBlueColor = new CMaterialColors();
	pBlueColor->m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pBlueColor->m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pBlueColor->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 4.0f);
	pBlueColor->m_d3dxcEmissive = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);


	//벤쉬 일반색상
	pInstancingMaterials[3] = new CMaterial(pColor);



	CMaterialColors *pBanSheeColor = new CMaterialColors();
	pBanSheeColor->m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pBanSheeColor->m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pBanSheeColor->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 4.0f);
	pBanSheeColor->m_d3dxcEmissive = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	pInstancingMaterials[4] = new CMaterial(pColor);

	CMaterialColors *pTitileColor = new CMaterialColors();
	pTitileColor->m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pTitileColor->m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pTitileColor->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 4.0f);
	pTitileColor->m_d3dxcEmissive = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	pInstancingMaterials[5] = new CMaterial(pTitileColor);


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
	CTexture *pStoneTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/Miscellaneous/Stone01.jpg"), NULL, NULL, &pd3dsrvTexture, NULL);
	pStoneTexture->SetTexture(0, pd3dsrvTexture);
	pStoneTexture->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();
	pInstancingMaterials[0]->SetTexture(pStoneTexture);

	CTexture *pBrickTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/Miscellaneous/Brick01.jpg"), NULL, NULL, &pd3dsrvTexture, NULL);
	pBrickTexture->SetTexture(0, pd3dsrvTexture);
	pBrickTexture->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();
	pInstancingMaterials[1]->SetTexture(pBrickTexture);

	CTexture *pWoodTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/Miscellaneous/Wood01.jpg"), NULL, NULL, &pd3dsrvTexture, NULL);
	pWoodTexture->SetTexture(0, pd3dsrvTexture);
	pWoodTexture->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();
	pInstancingMaterials[2]->SetTexture(pWoodTexture);

	CTexture *pBansheeTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("banshee/banshee_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pBansheeTexture->SetTexture(0, pd3dsrvTexture);
	pBansheeTexture->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();
	pInstancingMaterials[3]->SetTexture(pBansheeTexture);

	CTexture *pTitileTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("banshee/banshee_blue.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTitileTexture->SetTexture(0, pd3dsrvTexture);
	pTitileTexture->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();
	pInstancingMaterials[4]->SetTexture(pTitileTexture);


	/*CTexture *pBulletTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	T=D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("bullet/bullet_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	cout << T << endl;
	pBulletTexture->SetTexture(0, pd3dsrvTexture);
	pBulletTexture->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();
	pInstancingMaterials[6] -> SetTexture(pBulletTexture);*/


	pd3dSamplerState->Release();

	float fSize = 24.0f;
	float fObstacleSize = 180.f;

	CMesh *pMeshes[10];
	//	pMeshes[0] = new CCubeMeshTexturedIlluminated(pd3dDevice, fSize, fSize, fSize);
	pMeshes[0] = new CSkinMeshDiffused(pd3dDevice, fSize, fSize, fSize, BANSHEE_TXT);
	
	// 돌 장애물
	pMeshes[1] = new CCubeMeshTexturedIlluminated(pd3dDevice, fObstacleSize, fObstacleSize, fObstacleSize);
	// 벽돌 장애물
	pMeshes[2] = new CCubeMeshTexturedIlluminated(pd3dDevice, fObstacleSize/2, fObstacleSize, fObstacleSize);
	// 맵 좌우
	pMeshes[3] = new CCubeMeshTexturedIlluminated(pd3dDevice, 1.0f, fObstacleSize, 2046.f);
	// 맵 위아래
	pMeshes[4] = new  CCubeMeshTexturedIlluminated(pd3dDevice, 2046.0f, fObstacleSize, 1.f);
	// 레드 본진
	pMeshes[5] = new CCubeMeshTexturedIlluminated(pd3dDevice, fObstacleSize+30, fObstacleSize-30, fObstacleSize+30);
	// 블루 본진
	pMeshes[6] = new  CCubeMeshTexturedIlluminated(pd3dDevice, fObstacleSize + 30, fObstacleSize - 30, fObstacleSize + 30);
	pMeshes[7] = new CSkinMeshDiffused(pd3dDevice, fSize, fSize, fSize, MARINE_TXT);
	pMeshes[8] = new CSkinMeshDiffused(pd3dDevice, fSize, fSize, fSize, BANSHEE_TXT);
	pMeshes[9] = new CSkinMeshDiffused(pd3dDevice, fSize, fSize, fSize, MARINE_TXT);
//	pMeshes[6] = new CSphereMeshTexturedIlluminated(pd3dDevice, fSize, 20, 20);

//	pMeshes[7] = new CBulletMeshDiffused(pd3dDevice, "bullet/BulletMesh.txt");

	//ID3D11ShaderResourceView *pBulletView = NULL;
	//CTexture *pBulletTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	//CMaterial *pBulletMaterial = nullptr;
	//HRESULT T = D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("bullet/bullet_diffuse.png"), NULL, NULL, &pBulletView, NULL);

	//pBulletMaterial->SetTexture(pBulletTexture);
	//pBulletMaterial->m_pTexture->SetTexture(0, pBulletView);
	//pBulletTexture->SetSampler(0, pd3dSamplerState);

	



	float fxPitch = fSize * 3.5f;
	float fyPitch = fSize * 3.5f;
	float fzPitch = fSize * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = int(fTerrainWidth / (fxPitch * 3.0f)), zObjects = int(fTerrainLength / (fzPitch * 3.0f));

	m_nInstancingShaders = 10;
	m_ppInstancingShaders = new CInstancedObjectsShader*[m_nInstancingShaders];



	
	for (int i = 0; i < 10; i++)
	{
		if (i == 0)/// 레드팀  미니언
		{
			m_ppInstancingShaders[0] = new CInstancedObjectsShader(100);
			m_ppInstancingShaders[0]->SetMesh(pMeshes[0]);
			m_ppInstancingShaders[0]->SetMaterial(pInstancingMaterials[3]);
			m_ppInstancingShaders[0]->BuildObjects(pd3dDevice, NULL);
			m_ppInstancingShaders[0]->CreateShader(pd3dDevice);
		}
		else if (i==1)
		{
			m_ppInstancingShaders[1] = new CInstancedObjectsShader(5);
			m_ppInstancingShaders[1]->SetMesh(pMeshes[1]);
			m_ppInstancingShaders[1]->SetMaterial(pInstancingMaterials[0]);
			m_ppInstancingShaders[1]->BuildObjects(pd3dDevice, NULL);
			m_ppInstancingShaders[1]->CreateShader(pd3dDevice);

		}
		else if (i==2)
		{
			m_ppInstancingShaders[2] = new CInstancedObjectsShader(4);
			m_ppInstancingShaders[2]->SetMesh(pMeshes[2]);
			m_ppInstancingShaders[2]->SetMaterial(pInstancingMaterials[1]);
			m_ppInstancingShaders[2]->BuildObjects(pd3dDevice, NULL);
			m_ppInstancingShaders[2]->CreateShader(pd3dDevice);

		}
		else if (i == 3)
		{
			m_ppInstancingShaders[3] = new CInstancedObjectsShader(2);
			m_ppInstancingShaders[3]->SetMesh(pMeshes[3]);
			m_ppInstancingShaders[3]->SetMaterial(pInstancingMaterials[2]);
			m_ppInstancingShaders[3]->BuildObjects(pd3dDevice, NULL);
			m_ppInstancingShaders[3]->CreateShader(pd3dDevice);

		}
		else if (i == 4)
		{
			m_ppInstancingShaders[4] = new CInstancedObjectsShader(2);
			m_ppInstancingShaders[4]->SetMesh(pMeshes[4]);
			m_ppInstancingShaders[4]->SetMaterial(pInstancingMaterials[2]);
			m_ppInstancingShaders[4]->BuildObjects(pd3dDevice, NULL);
			m_ppInstancingShaders[4]->CreateShader(pd3dDevice);

		}
		else if (i == 5)
		{
			m_ppInstancingShaders[5] = new CInstancedObjectsShader(2);
			m_ppInstancingShaders[5]->SetMesh(pMeshes[5]);
			m_ppInstancingShaders[5]->SetMaterial(pInstancingMaterials[0]);
			m_ppInstancingShaders[5]->BuildObjects(pd3dDevice, NULL);
			m_ppInstancingShaders[5]->CreateShader(pd3dDevice);
		}

		else if (i == 6)
		{
			m_ppInstancingShaders[6] = new CInstancedObjectsShader(1);
			m_ppInstancingShaders[6]->SetMesh(pMeshes[6]);
			m_ppInstancingShaders[6]->SetMaterial(pInstancingMaterials[5]);
			m_ppInstancingShaders[6]->BuildObjects(pd3dDevice, NULL);
			m_ppInstancingShaders[6]->CreateShader(pd3dDevice);
		}
		else if (i == 7)//레드팀 마린
		{
			m_ppInstancingShaders[7] = new CInstancedObjectsShader(5);
			m_ppInstancingShaders[7]->SetMesh(pMeshes[7]);
			m_ppInstancingShaders[7]->SetMaterial(pMarineMaterial[0]);
			m_ppInstancingShaders[7]->BuildObjects(pd3dDevice, NULL);
			m_ppInstancingShaders[7]->CreateShader(pd3dDevice);

		}
		else if (i == 8)//블루팀 마린
		{
			m_ppInstancingShaders[8] = new CInstancedObjectsShader(5);
			m_ppInstancingShaders[8]->SetMesh(pMeshes[9]);
			m_ppInstancingShaders[8]->SetMaterial(pMarineMaterial[2]);
			m_ppInstancingShaders[8]->BuildObjects(pd3dDevice, NULL);
			m_ppInstancingShaders[8]->CreateShader(pd3dDevice);
		}
		else if (i == 9) {// 블루팀미니언
			m_ppInstancingShaders[9] = new CInstancedObjectsShader(100);
			m_ppInstancingShaders[9]->SetMesh(pMeshes[8]);
			m_ppInstancingShaders[9]->SetMaterial(pInstancingMaterials[4]);
			m_ppInstancingShaders[9]->BuildObjects(pd3dDevice, NULL);
			m_ppInstancingShaders[9]->CreateShader(pd3dDevice);

		}

	}


	CRotatingObject *pRotatingObject = nullptr;
	CGameObject		*pGameObject = nullptr;
	for (int i = 0; i < 5; i++)
	{
		pGameObject = new CGameObject();
		pGameObject->SetMesh(pMeshes[7]);
		pGameObject->Rotate(0, 180, 0);
		pGameObject->SetPosition(100, pTerrain->GetHeight(100, 500) + 10,100);


		m_ppInstancingShaders[7]->AddObject(pGameObject);
	}
	for (int i = 0; i < 5; i++)
	{
		pGameObject = new CGameObject();
		pGameObject->SetMesh(pMeshes[7]);
		//pGameObject->Rotate(0, 180, 0);
		pGameObject->SetPosition(rand() % 2000 - 1000, pTerrain->GetHeight(100, 500) + 10, rand() % 2000 - 1000);


		m_ppInstancingShaders[8]->AddObject(pGameObject);
	}


	D3DXVECTOR3 d3dxvRotateAxis;
	

	

		
	
	for (int i = 0; i < 100; i++)
	{
		pGameObject = new CGameObject();
		pGameObject->SetMesh(pMeshes[0]);
		pGameObject->SetPosition(50000, 50000,50000);
		pGameObject->Rotate(0, 180, 0);
		m_ppInstancingShaders[0]->AddObject(pGameObject);
	}
	for (int i = 0; i < 100; i++)
	{
		pGameObject = new CGameObject();
		pGameObject->SetMesh(pMeshes[0]);
		pGameObject->SetPosition(50000, 50000, 50000);
		m_ppInstancingShaders[9]->AddObject(pGameObject);
	}
	pGameObject = new CGameObject();
	pGameObject -> SetMesh(pMeshes[1]);
	pGameObject -> SetPosition(600.f, 416.f, 400.f);
	m_ppInstancingShaders[1]->AddObject(pGameObject);
	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[1]);
	pGameObject->SetPosition(1446.f, 416.f, 400.f);
	m_ppInstancingShaders[1]->AddObject(pGameObject);
	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[1]);
	pGameObject->SetPosition(1446.f, 416.f, 1646.f);
	m_ppInstancingShaders[1]->AddObject(pGameObject);
	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[1]);
	pGameObject->SetPosition(600.f, 416.f, 1646.f);
	m_ppInstancingShaders[1]->AddObject(pGameObject);
	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[1]);
	pGameObject->SetPosition(1023.f, 416.f, 1023.f);
	m_ppInstancingShaders[1]->AddObject(pGameObject);


	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[2]);
	pGameObject->SetPosition(901.f, 416.f, 700.f);
	m_ppInstancingShaders[2]->AddObject(pGameObject);
	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[2]);
	pGameObject->SetPosition(1145.f, 416.f, 700.f);
	m_ppInstancingShaders[2]->AddObject(pGameObject);
	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[2]);
	pGameObject->SetPosition(1145.f, 416.f, 1346.f);
	m_ppInstancingShaders[2]->AddObject(pGameObject);
	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[2]);
	pGameObject->SetPosition(901.f, 416.f, 1346.f);
	m_ppInstancingShaders[2]->AddObject(pGameObject);
	
	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[3]);
	pGameObject->SetPosition(0.f, 416.f, 1023.f);
	m_ppInstancingShaders[3]->AddObject(pGameObject);
	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[3]);
	pGameObject->SetPosition(2046.f, 416.f, 1023.f);
	m_ppInstancingShaders[3]->AddObject(pGameObject);

	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[4]);
	pGameObject->SetPosition(1023.f, 416.f, 0.f);
	m_ppInstancingShaders[4]->AddObject(pGameObject);
	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[4]);
	pGameObject->SetPosition(1023.f, 416.f, 2046.f);
	m_ppInstancingShaders[4]->AddObject(pGameObject);

	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[5]);
	pGameObject->SetPosition(1023.f, 416.f,105.f);
	m_ppInstancingShaders[5]->AddObject(pGameObject);
	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[5]);
	pGameObject->SetPosition(1023.f, 416.f, 1943.f);
	m_ppInstancingShaders[5]->AddObject(pGameObject);


	pGameObject = new CGameObject();
	pGameObject->SetMesh(pMeshes[6]);
	pGameObject->SetPosition(1800.f, 416.f, 1800.f);
	m_ppInstancingShaders[6]->AddObject(pGameObject);



	//pGameObject = new CGameObject();
	//pGameObject->SetMesh(pMeshes[7]);
	//pGameObject->SetPosition(1300.f, 416.f, 1200.f);
	//m_ppInstancingShaders[7]->AddObject(pGameObject);

	//m_ppInstancingShaderss = m_ppInstancingShaders;

	CreateShaderVariables(pd3dDevice);
}

CHeightMapTerrain *CScene::GetTerrain()
{
	return((CHeightMapTerrain *)m_ppObjects[1]);
}

void CScene::ReleaseObjects()
{
	ReleaseShaderVariables();

	for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->Release();
	if (m_ppObjects) delete[] m_ppObjects;

	for (int j = 0; j < m_nObjectShaders; j++)
	{
		if (m_ppObjectShaders[j]) m_ppObjectShaders[j]->ReleaseObjects();
		if (m_ppObjectShaders[j]) m_ppObjectShaders[j]->Release();
	}
	if (m_ppObjectShaders) delete[] m_ppObjectShaders;

	for (int j = 0; j < m_nInstancingShaders; j++)
	{
		if (m_ppInstancingShaders[j]) m_ppInstancingShaders[j]->ReleaseObjects();
		if (m_ppInstancingShaders[j]) m_ppInstancingShaders[j]->Release();
	}
	if (m_ppInstancingShaders) delete[] m_ppInstancingShaders;
}

void CScene::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = 1.0f;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 300.0f;
	m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvPosition = D3DXVECTOR3(300.0f, 300.0f, 300.0f);
	m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = 1.0f;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 100.0f;
	m_pLights->m_pLights[1].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcDiffuse = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_d3dxvPosition = D3DXVECTOR3(500.0f, 300.0f, 500.0f);
	m_pLights->m_pLights[1].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(D3DXToRadian(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(D3DXToRadian(20.0f));
	m_pLights->m_pLights[2].m_bEnable = 1.0f;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcDiffuse = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[2].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[3].m_bEnable = 1.0f;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 60.0f;
	m_pLights->m_pLights[3].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[3].m_d3dxcDiffuse = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[3].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_d3dxvPosition = D3DXVECTOR3(500.0f, 300.0f, 500.0f);
	m_pLights->m_pLights[3].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[3].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.001f);
	m_pLights->m_pLights[3].m_fFalloff = 20.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(D3DXToRadian(40.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(D3DXToRadian(15.0f));

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(LIGHTS);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pLights;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dcbLights);
}

void CScene::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	LIGHTS *pcbLight = (LIGHTS *)d3dMappedResource.pData;
	memcpy(pcbLight, pLights, sizeof(LIGHTS));
	pd3dDeviceContext->Unmap(m_pd3dcbLights, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_CB_SLOT_LIGHT, 1, &m_pd3dcbLights);
}

void CScene::ReleaseShaderVariables()
{
	if (m_pLights) delete m_pLights;
	if (m_pd3dcbLights) m_pd3dcbLights->Release();
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

CGameObject *CScene::PickObjectPointedByCursor(int xClient, int yClient)
{
	if (!m_pCamera) return(NULL);

	D3DXMATRIX d3dxmtxView = m_pCamera->GetViewMatrix();
	D3DXMATRIX d3dxmtxProjection = m_pCamera->GetProjectionMatrix();
	D3D11_VIEWPORT d3dViewport = m_pCamera->GetViewport();

	D3DXVECTOR3 d3dxvPickPosition;
	d3dxvPickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / d3dxmtxProjection._11;
	d3dxvPickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / d3dxmtxProjection._22;
	d3dxvPickPosition.z = 1.0f;

	int nIntersected = 0;
	float fNearHitDistance = FLT_MAX;
	MESHINTERSECTINFO d3dxIntersectInfo;
	CGameObject *pIntersectedObject = NULL, *pNearestObject = NULL;
	for (int i = 0; i < m_nObjectShaders; i++)
	{
		pIntersectedObject = m_ppObjectShaders[i]->PickObjectByRayIntersection(&d3dxvPickPosition, &d3dxmtxView, &d3dxIntersectInfo);
		if (pIntersectedObject && (d3dxIntersectInfo.m_fDistance < fNearHitDistance))
		{
			fNearHitDistance = d3dxIntersectInfo.m_fDistance;
			pNearestObject = pIntersectedObject;
		}
	}
	for (int i = 1; i < m_nObjects; i++)
	{
		nIntersected = m_ppObjects[i]->PickObjectByRayIntersection(&d3dxvPickPosition, &d3dxmtxView, &d3dxIntersectInfo);
		if ((nIntersected > 0) && (d3dxIntersectInfo.m_fDistance < fNearHitDistance))
		{
			fNearHitDistance = d3dxIntersectInfo.m_fDistance;
			pNearestObject = m_ppObjects[i];
		}
	}

	return(pNearestObject);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	D3DXVECTOR3 d3dxvCameraPosition = m_pCamera->GetPosition();
	if (m_pLights && m_pd3dcbLights)
	{
		m_pLights->m_d3dxvCameraPosition = D3DXVECTOR4(d3dxvCameraPosition, 1.0f);

		CHeightMapTerrain *pTerrain = GetTerrain();
		static D3DXVECTOR3 d3dxvRotated = D3DXVECTOR3(pTerrain->GetWidth()*0.3f, 0.0f, 0.0f);
		D3DXMATRIX d3dxmtxRotate;
		D3DXMatrixRotationYawPitchRoll(&d3dxmtxRotate, (float)D3DXToRadian(30.0f*fTimeElapsed), 0.0f, 0.0f);
		D3DXVec3TransformCoord(&d3dxvRotated, &d3dxvRotated, &d3dxmtxRotate);
		D3DXVECTOR3 d3dxvTerrainCenter = D3DXVECTOR3(pTerrain->GetWidth()*0.5f, pTerrain->GetPeakHeight() + 10.0f, pTerrain->GetLength()*0.5f);
		m_pLights->m_pLights[0].m_d3dxvPosition = d3dxvTerrainCenter + d3dxvRotated;
		m_pLights->m_pLights[0].m_fRange = pTerrain->GetPeakHeight();

		CPlayer *pPlayer = m_pCamera->GetPlayer();
		m_pLights->m_pLights[1].m_d3dxvPosition = pPlayer->GetPosition();
		m_pLights->m_pLights[1].m_d3dxvDirection = pPlayer->GetLookVector();

		m_pLights->m_pLights[3].m_d3dxvPosition = pPlayer->GetPosition() + D3DXVECTOR3(0.0f, 80.0f, 0.0f);
	}

	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->Animate(fTimeElapsed, NULL);
	for (int i = 0; i < m_nObjectShaders; i++) m_ppObjectShaders[i]->AnimateObjects(fTimeElapsed);
	for (int i = 0; i < m_nInstancingShaders; i++) m_ppInstancingShaders[i]->AnimateObjects(fTimeElapsed);
}

void CScene::OnPreRender(ID3D11DeviceContext *pd3dDeviceContext)
{
}

void CScene::Render(ID3D11DeviceContext	*pd3dDeviceContext, CCamera *pCamera)
{
	if (m_pLights && m_pd3dcbLights) UpdateShaderVariable(pd3dDeviceContext, m_pLights);

	if (m_ppObjects && m_ppObjects[0]) m_ppObjects[0]->Render(pd3dDeviceContext, pCamera); //SkyBox
	for (int i = 1; i < m_nObjects; i++)
	{
		//if (m_ppObjects[i]->IsVisible(pCamera)) 
		m_ppObjects[i]->Render(pd3dDeviceContext, pCamera);
	}
	for (int i = 0; i < m_nObjectShaders; i++) m_ppObjectShaders[i]->Render(pd3dDeviceContext, pCamera);
	for (int i = 0; i < m_nInstancingShaders; i++) m_ppInstancingShaders[i]->Render(pd3dDeviceContext, pCamera);
}

TitileScene::TitileScene()
{
}

TitileScene::~TitileScene()
{
}

void TitileScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	

}