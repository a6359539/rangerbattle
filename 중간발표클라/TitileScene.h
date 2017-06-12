#pragma once
#include "Scene.h"
class CTitileScene //:public CScene
{
public:
	CTitileScene();
	~CTitileScene();

	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual 	void Render(ID3D11DeviceContext	*pd3dDeviceContext, CCamera *pCamera);
	virtual		void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }
	virtual		void SetPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }

private:

	int								m_nObjects;

	CObjectsShader					**m_ppObjectShaders;
	int								m_nObjectShaders;


	CPlayer							*m_pPlayer;
	CCamera							*m_pCamera;
	//	CGameObject						*m_pSelectedObject;

	//	LIGHTS							*m_pLights;
	//	ID3D11Buffer					*m_pd3dcbLights;

};

