//------------------------------------------------------- ----------------------
// File: Mesh.h
//-----------------------------------------------------------------------------

#pragma once

#define VERTEX_POSITION_ELEMENT			0x0001
#define VERTEX_COLOR_ELEMENT			0x0002
#define VERTEX_NORMAL_ELEMENT			0x0004
#define VERTEX_TEXTURE_ELEMENT_0		0x0008
#define VERTEX_TEXTURE_ELEMENT_1		0x0010

#define TEXTURE_ELEMENT_0_TRANSFORM		0x0100
#define TEXTURE_ELEMENT_1_TRANSFORM		0x0200

#define VERTEX_INSTANCING_ELEMENT		0x0020

#define VERTEX_BONE_ID_ELEMENT			0x0400
#define VERTEX_BONE_WEIGHT_ELEMENT		0x0800

#define VERTEX_BLENDING_SHADER			0x1000
#define INSTANCING_SHADER				0x2000

struct MESHINTERSECTINFO {
	DWORD	m_dwFaceIndex;
	float	m_fU;
	float	m_fV;
	float	m_fDistance;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
#define EPSILON				1.0e-10f

inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }
inline float InverseSqrt(float fValue) { return 1.0f / sqrtf(fValue); }
inline void Swap(float *pfS, float *pfT) { float fTemp = *pfS; *pfS = *pfT; *pfT = fTemp; }

extern bool RayIntersectTriangle(D3DXVECTOR3 *pd3dxvOrigin, D3DXVECTOR3 *pd3dxvDirection, D3DXVECTOR3 *pd3dxvP0, D3DXVECTOR3 *pd3dxvP1, D3DXVECTOR3 *pd3dxvP2, float *pfU, float *pfV, float *pfRayToTriangle);

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class AABB
{
public:
	D3DXVECTOR3						m_d3dxvMinimum;
	D3DXVECTOR3						m_d3dxvMaximum;

public:
	AABB() { m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX); m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX); };
	AABB(D3DXVECTOR3 d3dxvMinimum, D3DXVECTOR3 d3dxvMaximum) { m_d3dxvMinimum = d3dxvMinimum; m_d3dxvMaximum = d3dxvMaximum; }

	void Merge(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	void Merge(AABB *pAABB);
	void Update(D3DXMATRIX *pd3dxmtxTransform);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMesh
{
public:
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();

private:
	int								m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected:
	AABB							m_bcBoundingCube;
	AABB							m_bcBoundingCubeHead;
	AABB							m_bcBoundingCubeBody;
	AABB							m_bcBoundingCubeRhand;
	AABB							m_bcBoundingCubeLhand;
	AABB							m_bcBoundingCubeRleg;
	AABB							m_bcBoundingCubeLleg;

protected:
	D3D11_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology;

	UINT							m_nType;

	ID3D11Buffer					*m_pd3dPositionBuffer;
	ID3D11Buffer					**m_ppd3dVertexBuffers;
//	ID3D11Buffer					*m_pd3dTexCoordBuffer;
//	ID3D11Buffer					*m_pd3dNormalBuffer;

	int								m_nVertices;
	int								m_nBuffers;
	UINT							m_nSlot;
	UINT							m_nStartVertex;
	UINT							*m_pnVertexStrides;
	UINT							*m_pnVertexOffsets;

	ID3D11Buffer					*m_pd3dIndexBuffer;

	UINT							m_nIndices;
	UINT							m_nStartIndex;
	int								m_nBaseVertex;
	DXGI_FORMAT						m_dxgiIndexFormat;
	UINT							m_nIndexOffset;

	D3DXVECTOR3						*m_pd3dxvPositions;
	D3DXVECTOR3						*m_pd3dxvNormals;
	D3DXVECTOR2						*m_pd3dxvTexCoords;
	D3DXVECTOR4						*m_pd3dxvBoneIndices;
	D3DXVECTOR4						*m_pd3dxvBoneWeights;
	UINT							*m_pnIndices;

	int								m_nBoneCount;
	int								m_nAnimationClips;

public:
	UINT GetType() { return(m_nType); }
	AABB GetBoundingCube() { return(m_bcBoundingCube); }
	AABB GetBoundingCubeHead() { return m_bcBoundingCubeHead; }
	AABB GetBoundingCubeBody() { return m_bcBoundingCubeBody; }
	AABB GetBoundingCubeRhand() { return m_bcBoundingCubeRhand; }
	AABB GetBoundingCubeLhand() { return m_bcBoundingCubeLhand; }
	AABB GetBoundingCubeRleg() { return m_bcBoundingCubeRleg; }
	AABB GetBoundingCubeLleg() { return m_bcBoundingCubeLleg; }
	void CalculateBoundingCube();

	ID3D11Buffer *CreateBuffer(ID3D11Device *pd3dDevice, UINT nStride, int nElements, void *pBufferData, UINT nBindFlags, D3D11_USAGE d3dUsage, UINT nCPUAccessFlags);
	//바운딩박스를 렌더링하기위한 버퍼
	ID3D11Buffer *CreateBoundingBuffer(ID3D11Device *pd3dDevice, UINT nStride, int nElements, void *pBufferData, UINT nBindFlags, D3D11_USAGE d3dUsage, UINT nCPUAccessFlags);
	void AssembleToVertexBuffer(int nBuffers = 0, ID3D11Buffer **m_pd3dBuffers = NULL, UINT *pnBufferStrides = NULL, UINT *pnBufferOffsets = NULL);

	virtual void OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances = 0, int nStartInstance = 0);

	int CheckRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshDiffused : public CMesh
{
public:
	CMeshDiffused(ID3D11Device *pd3dDevice);
	virtual ~CMeshDiffused();

protected:
	ID3D11Buffer					*m_pd3dColorBuffer;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshIlluminated : public CMesh
{
public:
	CMeshIlluminated(ID3D11Device *pd3dDevice);
	virtual ~CMeshIlluminated();

protected:
	ID3D11Buffer					*m_pd3dNormalBuffer;

public:
	D3DXVECTOR3 CalculateTriAngleNormal(UINT nIndex0, UINT nIndex1, UINT nIndex2);
	void SetTriAngleListVertexNormal(D3DXVECTOR3 *pd3dxvNormals);
	void SetAverageVertexNormal(D3DXVECTOR3 *pd3dxvNormals, int nPrimitives, int nOffset, bool bStrip);
	void CalculateVertexNormal(D3DXVECTOR3 *pd3dxvNormals);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshTextured : public CMesh
{
public:
	CMeshTextured(ID3D11Device *pd3dDevice);
	virtual ~CMeshTextured();

protected:
	ID3D11Buffer					*m_pd3dTexCoordBuffer;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshDetailTextured : public CMeshTextured
{
public:
	CMeshDetailTextured(ID3D11Device *pd3dDevice);
	virtual ~CMeshDetailTextured();

protected:
	ID3D11Buffer					*m_pd3dDetailTexCoordBuffer;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshTexturedIlluminated : public CMeshIlluminated
{
public:
	CMeshTexturedIlluminated(ID3D11Device *pd3dDevice);
	virtual ~CMeshTexturedIlluminated();

protected:
	ID3D11Buffer					*m_pd3dTexCoordBuffer;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshDetailTexturedIlluminated : public CMeshIlluminated
{
public:
	CMeshDetailTexturedIlluminated(ID3D11Device *pd3dDevice);
	virtual ~CMeshDetailTexturedIlluminated();

protected:
	ID3D11Buffer					*m_pd3dTexCoordBuffer;
	ID3D11Buffer					*m_pd3dDetailTexCoordBuffer;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CAirplaneMeshDiffused : public CMeshDiffused
{
public:
	CAirplaneMeshDiffused(ID3D11Device *pd3dDevice, float fWidth = 20.0f, float fHeight = 4.0f, float fDepth = 20.0f, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMeshDiffused();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeMeshDiffused : public CMeshDiffused
{
public:
	CCubeMeshDiffused(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CCubeMeshDiffused();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSphereMeshDiffused : public CMeshDiffused
{
public:
	CSphereMeshDiffused(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CSphereMeshDiffused();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminated();
};

class CSphereMeshIlluminated : public CMeshIlluminated
{
public:
	CSphereMeshIlluminated(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20);
	virtual ~CSphereMeshIlluminated();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeMeshTextured : public CMeshTextured
{
public:
	CCubeMeshTextured(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshTextured();
};

class CSphereMeshTextured : public CMeshTextured
{
public:
	CSphereMeshTextured(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20);
	virtual ~CSphereMeshTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeMeshTexturedIlluminated : public CMeshTexturedIlluminated
{
public:
	CCubeMeshTexturedIlluminated(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshTexturedIlluminated();
};

class CSphereMeshTexturedIlluminated : public CMeshTexturedIlluminated
{
public:
	CSphereMeshTexturedIlluminated(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20);
	virtual ~CSphereMeshTexturedIlluminated();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CHeightMapGridMesh : public CMeshDetailTexturedIlluminated
{
protected:
	int							m_nWidth;
	int							m_nLength;
	D3DXVECTOR3					m_d3dxvScale;

public:
	CHeightMapGridMesh(ID3D11Device *pd3dDevice, int xStart, int zStart, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f), void *pContext = NULL, D3D11_USAGE d3dUsage = D3D11_USAGE_DEFAULT);
	virtual ~CHeightMapGridMesh();

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	virtual float OnGetHeight(int x, int z, void *pContext);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBoxMesh : public CMeshTextured
{
public:
	CSkyBoxMesh(ID3D11Device *pd3dDevice, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f);
	virtual ~CSkyBoxMesh();
};

class CWaterGridMesh : public CHeightMapGridMesh
{
private:
	int							m_xStart;
	int							m_zStart;

	float						m_fK1;
	float						m_fK2;
	float						m_fK3;

	float						m_fTimeDelta;
	float						m_fSpatialDelta;
	float						m_fAccumulatedTime;

	D3DXVECTOR3					*m_pd3dxvPreviousPositions;
	D3DXVECTOR3					*m_pd3dxvCurrentPositions;
	D3DXVECTOR3					*m_pd3dxvTempPositions;
	D3DXVECTOR3					*m_pd3dxvNormalVectors;

public:
	CWaterGridMesh(ID3D11Device *pd3dDevice, int xStart, int zStart, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f), void *pContext = NULL, D3D11_USAGE d3dUsage = D3D11_USAGE_DEFAULT);
	virtual ~CWaterGridMesh();

	virtual float OnGetHeight(int x, int z, void *pContext);

	void SetDisturbingForce(int i, int j, float fMagnitude);
	void OnPrepareAnimation(float fTimeDelta, float fSpatialDelta, float fSpeed, float fDamping);

	virtual void Animate(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTextureToScreenRectMesh : public CMeshTextured
{
public:
	CTextureToScreenRectMesh(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f);
	virtual ~CTextureToScreenRectMesh();
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTexturedRectMesh : public CMeshTextured
{
public:
	CTexturedRectMesh(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fLength = 2.0f);
	virtual ~CTexturedRectMesh();
};





class CSkinMeshDiffused : public CMeshTextured 

{
protected:
	//	ID3D11Buffer					*m_pd3dTexCoordBuffer;
		ID3D11Buffer					*m_pd3dNormalBuffer;

public:
	CSkinMeshDiffused(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, string filename = nullptr);
	virtual ~CSkinMeshDiffused();
};





class CTitileMesh : public CMeshTexturedIlluminated
{
public:
	CTitileMesh(ID3D11Device *pd3dDevice, float ux, float uy, float vx, float vy);
	virtual ~CTitileMesh();
};

class CBoundingCubeMesh : public CMeshDiffused
{
public:
	CBoundingCubeMesh(ID3D11Device *pd3dDevice, D3DXVECTOR3 Max,D3DXVECTOR3 Min, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f,1.0f));
	virtual ~CBoundingCubeMesh();
};




class CBulletMeshDiffused : public CMeshTextured

{
protected:
	//	ID3D11Buffer					*m_pd3dTexCoordBuffer;
	ID3D11Buffer					*m_pd3dNormalBuffer;

public:
	CBulletMeshDiffused(ID3D11Device *pd3dDevice, string filename = nullptr);
	virtual ~CBulletMeshDiffused();
};
//-------------------------------------애니메이션--------------------------------
struct VS_CB_SKINNED
{
	D3DXMATRIX   m_d3dxmtxBone[MAX_BONE_COUNT];
};

struct BoneAnimationData
{
	int         m_nFrameCount;

	float      *m_pfAniTime;
	D3DXVECTOR3 *m_pd3dxvScale;
	D3DXVECTOR3 *m_pd3dxvTranslate;
	D3DXVECTOR4 *m_pd3dxvQuaternion;
};


class CMarineMeshDiffused : public CMeshTextured

{
protected:
	//	ID3D11Buffer					*m_pd3dTexCoordBuffer;
	ID3D11Buffer					*m_pd3dNormalBuffer;
	ID3D11Buffer		*m_pd3dBoneIndexBuffer{ nullptr };
	ID3D11Buffer		*m_pd3dBoneWeightBuffer{ nullptr };

public:
	CMarineMeshDiffused(ID3D11Device *pd3dDevice, string filename = nullptr);
	virtual ~CMarineMeshDiffused();


	BoneAnimationData **m_ppBoneAnimationData{ nullptr };
	int m_nBoneCount{ 0 };
	int m_nAnimationClip{ 0 };

	D3DXVECTOR3 *m_pd3dxvPositions;//필요
	D3DXVECTOR3	*m_pd3dxvNormals;//필요
	D3DXVECTOR2 *m_pd3dxvTexCoords;//필요
	D3DXVECTOR4 *m_pd3dxvBoneWeights;//개별적 필요한 데이터
	D3DXVECTOR4 *m_pd3dxvBoneIndices;//개별적 필요한 데이터

	D3DXMATRIX *m_pd3dxmtxBoneOffsets;
	D3DXMATRIX *m_pd3dxmtxSQTTransform;//이것도 필요하다.
	D3DXMATRIX *m_pd3dxmtxFinalBone;//최종행렬 배열(이건 필요하다.)

	D3DXMATRIX m_d3dxmtxLocalTransform;//개별적으로 필요한 로컬 변환 행렬

									   // i번 뼈대의 부모 색인(parentIndex)를 담는다.
									   // i번 뼈대는 애니메이션 클립의 i번째 BoneAnimation 인스턴스에 대응.
	UINT *m_pBoneHierarchy;

	float m_fFBXModelSize;		// 모델의 사이즈 (보통 Animate에서 조절해주기 위함)
	float m_fFBXAnimationTime;	// 모델의 AnimationTime (공통적으로 0.0333333f 씩 증가)	
	int m_nFBXAnimationNum;		// 모델이 실행할 애니메이션의 Set넘버 0 혹은 1이다.

	int m_nFBXStartFrameNum;	// 모델의 애니메이션이 시작하는 프레임 넘버
	int m_nFBXMaxFrameNum;		// 모델이 실행할 애니메이션의 최대 프레임 수.

	int m_nFBXNowFrameNum;		// 모델이 진행중인 애니메이션의 현재 프레임 값.

	unsigned int m_uiAnimationState;

	//	ID3D11Buffer *CreateBuffer(ID3D11Device *pd3dDevice, UINT nStride, int nElements, void *pBufferData, UINT nBindFlags, D3D11_USAGE d3dUsage, UINT nCPUAccessFlags);
	D3DXMATRIX& GetLocalMatrix() { return m_d3dxmtxLocalTransform; }
	int GetMaxFrame() { return m_nFBXMaxFrameNum; }
	int GetNumBones() { return m_nBoneCount; }
	// 해당 프레임의 SR(Q)T 회전이 반영된 행렬을 반환
	void MakeBoneMatrix(int nNowframe, int nAnimationNum, int nBoneNum, D3DXMATRIX& BoneMatrix);

	//상수 버퍼로 전달할 최종 본 행렬을 구한다.
	void UpdateBoneTransform(int nAnimationNum, int nNowFrame);
	D3DXMATRIX* GetBoneTransform() { return m_pd3dxmtxFinalBone; }


	//toRoot 행렬을 시간에 따라 보간해보기 위해
	float GetClipStartTime(int nAnimationNum);
	float GetClipEndTime(int nAnimationNum);

	float GetKeyFrameTime(int nAnimationNum, int nKeyFrame)
	{
		int index = 0;
		for (index; index < m_nBoneCount; ++index)
		{
			if (nKeyFrame <= m_ppBoneAnimationData[nAnimationNum][index].m_nFrameCount)
				break;
		}

		return m_ppBoneAnimationData[nAnimationNum][index].m_pfAniTime[nKeyFrame];
	}


	void InterpolateBoneTransform(int nAnimationNum, float time);
	void InterpolateBone(int nAnimationNum, int nBoneNum, float time, D3DXMATRIX& BoneMatrix);





};
