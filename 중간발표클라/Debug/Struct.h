#ifndef Struct_h__
#define Struct_h__

typedef struct tagVertexTexture
{
	XMFLOAT3 vPos;
	XMFLOAT2 vTexUV;
	XMFLOAT3 vNormal;
}VTXTEX;

typedef struct tagVertexCubeColor
{
	XMFLOAT3 vPos;
	XMCOLOR  dwColor;
}VTXCOL;

typedef struct tagVertexBone
{
	XMFLOAT3 vPos;
	XMFLOAT2 vTexUV;
	XMFLOAT3 vNormal;
	UINT uiBones[4];
	FLOAT fWeights[4];

}VTXBONE;


typedef struct tagFrameCnt
{
	_ushort  wFrameCnt;
	_ushort  wFrameSpeed;

}FRAMEINFO;

typedef struct tagBaseShader_CB
{
	XMMATRIX matWorld;
	XMMATRIX matView;
	XMMATRIX matProj;
	XMVECTOR vLightPos;
	XMMATRIX matLightView[2];

}BASESHADER_CB;

typedef struct tagInstShader_CB
{
	XMMATRIX matWorld[INSTCNT];

}INSTSHADER_CB;

typedef struct tagDynamicShader_CB
{
	XMMATRIX matBoneWorld[MAX_BONE_MATRICES];

}DYNAMICSHADER_CB;

typedef struct tagTerEffect_CB
{
	FLOAT		fRange1;
	FLOAT		fRange2;
	XMVECTOR	vEffectPos1;
	XMVECTOR	vEffectPos2;

}TEREFFSHADER_CB;

typedef struct tagObjData
{
	UINT		uiImgNum;
	XMFLOAT3	vScale;
	XMFLOAT3	vAngle;
	XMFLOAT3	vPos;

}OBJDATA;


typedef struct tagObjectInfo
{
	TCHAR			m_szName[MAX_PATH]; //이름
	D3DXVECTOR3		m_vAngle;			//회전값
	D3DXVECTOR3		m_vScale;			//크기
	D3DXVECTOR3		m_vPos;				//위치

}OBJ_INFO;

typedef struct tagClusData {
	XMFLOAT4 R;
	XMFLOAT3 T;
	XMFLOAT3 S;
}CLUSDATA;


class CMesh;

typedef struct tagMeshData
{
	CMesh* pMesh;
	BOOL bAlpha;
	BOOL bBillboard;
}MESHDATA;

typedef struct tagMatNode
{
	XMFLOAT4X4					matBone[MAX_BONE_MATRICES];
	std::vector<tagMatNode*>	vecNode;

	void Release() {
		for (size_t iIndex = 0; iIndex < vecNode.size(); ++iIndex)
		{
			if (NULL != vecNode[iIndex])
			{
				delete vecNode[iIndex];
				vecNode[iIndex] = NULL;
			}
		}

		vecNode.clear();
		delete this;
	}
}MATNODE;

#endif // Struct_h__