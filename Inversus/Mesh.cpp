#include "MainH.h"
#include "Mesh.h"
#include "GraphicsPipeline.h"

CPolygon::CPolygon(int nVertices)
{
	m_nVertices = nVertices;		//현재 폴리곤의 정점의 갯수
	m_pVertices = new Vertex[nVertices]; //현재 폴리곤의 정점 갯수 만큼 정점 배열을 동적 선언.
}

CPolygon::~CPolygon()
{
	if (m_pVertices) delete[] m_pVertices;
}

void CPolygon::SetVertex(int nIndex, Vertex vertex)
{
	if ((0 <= nIndex) && (nIndex < m_nVertices) && m_pVertices)	//폴리곤을 만들때 정점의 갯수를 입력해둬서 해당 인덱스에 각각의 정점을 부여해준다.
	{
		m_pVertices[nIndex] = vertex;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
Mesh::Mesh(int nPolygons)	//폴리곤의 갯수를 입력 받는다.
{
	m_nPolygons = nPolygons;
	m_ppPolygons = new CPolygon * [nPolygons];
}

Mesh::~Mesh()
{
	if (m_ppPolygons)
	{
		for (int i = 0; i < m_nPolygons; i++) if (m_ppPolygons[i]) delete m_ppPolygons[i];
		delete[] m_ppPolygons;
	}
}

void Mesh::SetPolygon(int nIndex, CPolygon* pPolygon)	//해당 메쉬에 폴리곤을 추가해준다. ==> 음 우선 거의 1개의 폴리곤으로만 이루어진 친구들을 만들거 같아.
{
	if ((0 <= nIndex) && (nIndex < m_nPolygons)) m_ppPolygons[nIndex] = pPolygon;
}

void Draw2DPolygon(HDC hDCFrameBuffer, CPolygon* polygon)	//메쉬를 폴리곤 형태로 그릴 예정이라 폴리곤을 썼다.
{
	Point2D* Vertices = new Point2D[polygon->m_nVertices];	//현재 변환이 안일어난 정점을 받아왔다.
	POINT* point = new POINT[polygon->m_nVertices];
	Point2D ver;
	for (int i = 0; i < polygon->m_nVertices; i++)
	{
		ver = GraphicsPipeline::Transform(polygon->m_pVertices[i].m_f2Position);
		Vertices[i] = GraphicsPipeline::ScreenTransform(ver);

		point[i].x = Vertices[i].x;
		point[i].y = Vertices[i].y;

		//printf("Point %d: x = %d, y = %d\n", i, point[i].x, point[i].y);
	}

	Polygon(hDCFrameBuffer, point, polygon->m_nVertices);

	delete[] Vertices;	//배열 해제
	delete[] point;
}
void Mesh::Render(HDC hDCFrameBuffer)
{
	Point2D f2InitialProject, f2PreviousProject;
	int nWidth = GraphicsPipeline::GetScreenViewport()->m_nWidth;	//현재 내가 그릴 뷰포트 2D 게임에서는 뷰포트가 화면이랑 같에
	int nHeight = GraphicsPipeline::GetScreenViewport()->m_nHeight;	//거기 안의 범위에 들어가면 그려지도록 할려고 만들었어

	for (int j = 0; j < m_nPolygons; j++)
	{
		int ICount = 0;		//0이면 DrawPolygon을 수행하지 않는다.
		int nVertices = m_ppPolygons[j]->m_nVertices;		//정점의 갯수
		Vertex* pVertices = m_ppPolygons[j]->m_pVertices;		//폴리곤의 Vertoces 배열들.

		f2PreviousProject = f2InitialProject = GraphicsPipeline::Transform(pVertices[0].m_f2Position);		//0번째 정점의 좌표 값을 저장.
		if ((-nWidth / 2 <= f2InitialProject.x) && (f2InitialProject.x <= nWidth / 2) && (-nHeight / 2 <= f2InitialProject.y) && (f2InitialProject.y <= nHeight / 2)) //x: -1 ~ 1, y: -1 ~ 1 벗어나면 그리지 않는다.
		{
			ICount++;
		}
		for (int i = 1; i < nVertices; i++)
		{
			Point2D f3CurrentProject = GraphicsPipeline::Transform(pVertices[i].m_f2Position);
			if ((-nWidth / 2 <= f3CurrentProject.x) && (f3CurrentProject.x <= nWidth / 2) && (-nHeight / 2 <= f3CurrentProject.y) && (f3CurrentProject.y <= nHeight / 2))
			{
				ICount++;
			}
		}
		//printf("ICount:%d", ICount);
		if (ICount != 0)	//해당 폴리곤의 정점이 하나라도 뷰포트 내부에있으면 그냥 그리도록 판정을 했어.
		{
			Draw2DPolygon(hDCFrameBuffer, m_ppPolygons[j]);
		}
	}
}

cubeB::cubeB(float fWidth, float fHeight) : Mesh(1)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;

	CPolygon* Face = new CPolygon(4);
	Face->SetVertex(0, Vertex(-fHalfWidth, fHalfHeight));
	Face->SetVertex(1, Vertex(fHalfWidth, fHalfHeight));
	Face->SetVertex(2, Vertex(fHalfWidth, -fHalfHeight));
	Face->SetVertex(3, Vertex(-fHalfWidth, -fHalfHeight));
	SetPolygon(0, Face);
}
cubeB::~cubeB()
{

}

TShapeMesh::TShapeMesh(float fWidth, float fHeight) : Mesh(4) {
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;

	CPolygon* pFace = new CPolygon(4);
	pFace->SetVertex(0, Vertex(-fHalfWidth - fWidth, fHeight));
	pFace->SetVertex(1, Vertex(-fHalfWidth, fHeight));
	pFace->SetVertex(2, Vertex(-fHalfWidth, 0.0f));
	pFace->SetVertex(3, Vertex(-fHalfWidth - fWidth, 0.0f));
	SetPolygon(0, pFace);

	CPolygon* pFace1 = new CPolygon(4);

	pFace1->SetVertex(0, Vertex(-fHalfWidth, fHeight));
	pFace1->SetVertex(1, Vertex(fHalfWidth, fHeight));
	pFace1->SetVertex(2, Vertex(-fHalfWidth, 0.0f));
	pFace1->SetVertex(3, Vertex(fHalfWidth, 0.0f));
	SetPolygon(1, pFace1);

	CPolygon* pFace2 = new CPolygon(4);

	pFace2->SetVertex(0, Vertex(fHalfWidth, fHeight));
	pFace2->SetVertex(1, Vertex(fHalfWidth + fWidth, fHeight));
	pFace2->SetVertex(2, Vertex(fHalfWidth + fWidth, 0.0f));
	pFace2->SetVertex(3, Vertex(fHalfWidth, 0.0f));
	SetPolygon(2, pFace2);

	CPolygon* pFace3 = new CPolygon(4);

	pFace3->SetVertex(0, Vertex(-fHalfWidth, 0.0f));
	pFace3->SetVertex(1, Vertex(fHalfWidth, 0.0f));
	pFace3->SetVertex(2, Vertex(fHalfWidth, -fHeight));
	pFace3->SetVertex(3, Vertex(-fHalfWidth, -fHeight));
	SetPolygon(3, pFace3);

}

LRShapeMesh::LRShapeMesh(float fWidth, float fHeight) : Mesh(4) {	//ㄱ자 폴리곤 오른쪽 방향
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;

	CPolygon* pFace = new CPolygon(4);
	pFace->SetVertex(0, Vertex(0, fHalfHeight + fHeight));
	pFace->SetVertex(1, Vertex(fWidth, fHalfHeight + fHeight));
	pFace->SetVertex(2, Vertex(fWidth, fHalfHeight));
	pFace->SetVertex(3, Vertex(0, fHalfHeight));
	SetPolygon(0, pFace);


	CPolygon* pFace1 = new CPolygon(4);

	pFace1->SetVertex(0, Vertex(-fWidth, fHalfHeight + fHeight));
	pFace1->SetVertex(1, Vertex(0, fHalfHeight + fHeight));
	pFace1->SetVertex(2, Vertex(0, fHalfHeight));
	pFace1->SetVertex(3, Vertex(-fWidth, fHalfHeight));
	SetPolygon(1, pFace1);

	CPolygon* pFace2 = new CPolygon(4);

	pFace2->SetVertex(0, Vertex(-fWidth, fHalfHeight));
	pFace2->SetVertex(1, Vertex(0, fHalfHeight));
	pFace2->SetVertex(2, Vertex(0, -fHalfHeight));
	pFace2->SetVertex(3, Vertex(-fWidth, -fHalfHeight));
	SetPolygon(2, pFace2);

	CPolygon* pFace3 = new CPolygon(4);

	pFace3->SetVertex(0, Vertex(-fWidth, -fHalfHeight));
	pFace3->SetVertex(1, Vertex(0, -fHalfHeight));
	pFace3->SetVertex(2, Vertex(0, -fHalfHeight - fHeight));
	pFace3->SetVertex(3, Vertex(-fWidth, -fHalfHeight - fHeight));
	SetPolygon(3, pFace3);
}

LLShapeMesh::LLShapeMesh(float fWidth, float fHeight) : Mesh(4) {	//ㄱ자 폴리곤 왼쪽 방향
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;

	CPolygon* pFace = new CPolygon(4);
	pFace->SetVertex(0, Vertex(-fWidth, fHalfHeight + fHeight));
	pFace->SetVertex(1, Vertex(0, fHalfHeight + fHeight));
	pFace->SetVertex(2, Vertex(0, fHalfHeight));
	pFace->SetVertex(3, Vertex(-fWidth, fHalfHeight));
	SetPolygon(0, pFace);


	CPolygon* pFace1 = new CPolygon(4);

	pFace1->SetVertex(0, Vertex(0, fHalfHeight + fHeight));
	pFace1->SetVertex(1, Vertex(fWidth, fHalfHeight + fHeight));
	pFace1->SetVertex(2, Vertex(fWidth, fHalfHeight));
	pFace1->SetVertex(3, Vertex(0, fHalfHeight));
	SetPolygon(1, pFace1);

	CPolygon* pFace2 = new CPolygon(4);

	pFace2->SetVertex(0, Vertex(0, fHalfHeight));
	pFace2->SetVertex(1, Vertex(fWidth, fHalfHeight));
	pFace2->SetVertex(2, Vertex(fWidth, -fHalfHeight));
	pFace2->SetVertex(3, Vertex(0, -fHalfHeight));
	SetPolygon(2, pFace2);

	CPolygon* pFace3 = new CPolygon(4);

	pFace3->SetVertex(0, Vertex(0, -fHalfHeight));
	pFace3->SetVertex(1, Vertex(fWidth, -fHalfHeight));
	pFace3->SetVertex(2, Vertex(fWidth, -fHalfHeight - fHeight));
	pFace3->SetVertex(3, Vertex(0, -fHalfHeight - fHeight));
	SetPolygon(3, pFace3);
}

Cube2x2LMesh::Cube2x2LMesh(float fWidth, float fHeight) : Mesh(4) //왼쪽방향 메쉬
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;

	CPolygon* pFace = new CPolygon(4);
	pFace->SetVertex(0, Vertex(-fHalfWidth - fWidth, fHeight));
	pFace->SetVertex(1, Vertex(-fHalfWidth, fHeight));
	pFace->SetVertex(2, Vertex(-fHalfWidth, 0));
	pFace->SetVertex(3, Vertex(-fHalfWidth - fWidth, 0));
	SetPolygon(0, pFace);


	CPolygon* pFace1 = new CPolygon(4);

	pFace1->SetVertex(0, Vertex(-fHalfWidth, fHeight));
	pFace1->SetVertex(1, Vertex(fHalfWidth, fHeight));
	pFace1->SetVertex(2, Vertex(fHalfWidth, 0));
	pFace1->SetVertex(3, Vertex(-fHalfWidth, 0));
	SetPolygon(1, pFace1);

	CPolygon* pFace2 = new CPolygon(4);

	pFace2->SetVertex(0, Vertex(-fHalfWidth, 0));
	pFace2->SetVertex(1, Vertex(fHalfWidth, 0));
	pFace2->SetVertex(2, Vertex(fHalfWidth, -fHeight));
	pFace2->SetVertex(3, Vertex(-fHalfWidth, -fHeight));
	SetPolygon(2, pFace2);

	CPolygon* pFace3 = new CPolygon(4);

	pFace3->SetVertex(0, Vertex(fHalfWidth, 0));
	pFace3->SetVertex(1, Vertex(fHalfWidth + fWidth, 0));
	pFace3->SetVertex(2, Vertex(fHalfWidth + fWidth, -fHeight));
	pFace3->SetVertex(3, Vertex(fHalfWidth, -fHeight));
	SetPolygon(3, pFace3);

}

Cube2x2RMesh::Cube2x2RMesh(float fWidth, float fHeight) : Mesh(4) //왼쪽방향 메쉬
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;

	CPolygon* pFace = new CPolygon(4);
	pFace->SetVertex(0, Vertex(fHalfWidth, fHeight));
	pFace->SetVertex(1, Vertex(fHalfWidth + fWidth, fHeight));
	pFace->SetVertex(2, Vertex(fHalfWidth + fWidth, 0));
	pFace->SetVertex(3, Vertex(fHalfWidth, 0));
	SetPolygon(0, pFace);


	CPolygon* pFace1 = new CPolygon(4);

	pFace1->SetVertex(0, Vertex(-fHalfWidth, fHeight));
	pFace1->SetVertex(1, Vertex(fHalfWidth, fHeight));
	pFace1->SetVertex(2, Vertex(fHalfWidth, 0));
	pFace1->SetVertex(3, Vertex(-fHalfWidth, 0));
	SetPolygon(1, pFace1);

	CPolygon* pFace2 = new CPolygon(4);

	pFace2->SetVertex(0, Vertex(-fHalfWidth, 0));
	pFace2->SetVertex(1, Vertex(fHalfWidth, 0));
	pFace2->SetVertex(2, Vertex(fHalfWidth, -fHeight));
	pFace2->SetVertex(3, Vertex(-fHalfWidth, -fHeight));
	SetPolygon(2, pFace2);

	CPolygon* pFace3 = new CPolygon(4);

	pFace3->SetVertex(0, Vertex(-fHalfWidth - fWidth, 0));
	pFace3->SetVertex(1, Vertex(-fHalfWidth, 0));
	pFace3->SetVertex(2, Vertex(-fHalfWidth, -fHeight));
	pFace3->SetVertex(3, Vertex(-fHalfWidth - fWidth, -fHeight));
	SetPolygon(3, pFace3);

}

Cube3x1Mesh::Cube3x1Mesh(float fWidth, float fHeight) :Mesh(3)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;

	CPolygon* pFace = new CPolygon(4);
	pFace->SetVertex(0, Vertex(-fHalfWidth - fWidth, fHalfHeight));
	pFace->SetVertex(1, Vertex(-fHalfWidth, fHalfHeight));
	pFace->SetVertex(2, Vertex(-fHalfWidth, -fHalfHeight));
	pFace->SetVertex(3, Vertex(-fHalfWidth - fWidth, -fHalfHeight));
	SetPolygon(0, pFace);


	CPolygon* pFace1 = new CPolygon(4);

	pFace1->SetVertex(0, Vertex(-fHalfWidth, fHalfHeight));
	pFace1->SetVertex(1, Vertex(fHalfWidth, fHalfHeight));
	pFace1->SetVertex(2, Vertex(fHalfWidth, -fHalfHeight));
	pFace1->SetVertex(3, Vertex(-fHalfWidth, -fHalfHeight));
	SetPolygon(1, pFace1);

	CPolygon* pFace2 = new CPolygon(4);

	pFace2->SetVertex(0, Vertex(fHalfWidth, fHalfHeight));
	pFace2->SetVertex(1, Vertex(fHalfWidth + fWidth, fHalfHeight));
	pFace2->SetVertex(2, Vertex(fHalfWidth + fWidth, -fHalfHeight));
	pFace2->SetVertex(3, Vertex(fHalfWidth, -fHalfHeight));
	SetPolygon(2, pFace2);
}

Cube4x1Mesh::Cube4x1Mesh(float fWidth, float fHeight) : Mesh(4)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;

	CPolygon* pFace = new CPolygon(4);
	pFace->SetVertex(0, Vertex(-fWidth * 2.0f, fHalfHeight));
	pFace->SetVertex(1, Vertex(-fWidth, fHalfHeight));
	pFace->SetVertex(2, Vertex(-fWidth, -fHalfHeight));
	pFace->SetVertex(3, Vertex(-fWidth * 2.0f, -fHalfHeight));
	SetPolygon(0, pFace);


	CPolygon* pFace1 = new CPolygon(4);

	pFace1->SetVertex(0, Vertex(-fWidth, fHalfHeight));
	pFace1->SetVertex(1, Vertex(0, fHalfHeight));
	pFace1->SetVertex(2, Vertex(0, -fHalfHeight));
	pFace1->SetVertex(3, Vertex(-fWidth, -fHalfHeight));
	SetPolygon(1, pFace1);

	CPolygon* pFace2 = new CPolygon(4);

	pFace2->SetVertex(0, Vertex(0, fHalfHeight));
	pFace2->SetVertex(1, Vertex(fWidth, fHalfHeight));
	pFace2->SetVertex(2, Vertex(fWidth, -fHalfHeight));
	pFace2->SetVertex(3, Vertex(0, -fHalfHeight));
	SetPolygon(2, pFace2);

	CPolygon* pFace3 = new CPolygon(4);

	pFace3->SetVertex(0, Vertex(fWidth, fHalfHeight));
	pFace3->SetVertex(1, Vertex(fWidth * 2.0f, fHalfHeight));
	pFace3->SetVertex(2, Vertex(fWidth * 2.0f, -fHalfHeight));
	pFace3->SetVertex(3, Vertex(fWidth, -fHalfHeight));
	SetPolygon(3, pFace3);

}

Cube2x2Mesh::Cube2x2Mesh(float fWidth, float fHeight) : Mesh(4)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;

	CPolygon* pFace = new CPolygon(4);
	pFace->SetVertex(0, Vertex(-fWidth, fHeight));
	pFace->SetVertex(1, Vertex(0, fHeight));
	pFace->SetVertex(2, Vertex(0, 0));
	pFace->SetVertex(3, Vertex(-fWidth, 0));
	SetPolygon(0, pFace);


	CPolygon* pFace1 = new CPolygon(4);

	pFace1->SetVertex(0, Vertex(0, fHeight));
	pFace1->SetVertex(1, Vertex(fWidth, fHeight));
	pFace1->SetVertex(2, Vertex(fWidth, 0));
	pFace1->SetVertex(3, Vertex(0, 0));
	SetPolygon(1, pFace1);

	CPolygon* pFace2 = new CPolygon(4);

	pFace2->SetVertex(0, Vertex(-fWidth, 0));
	pFace2->SetVertex(1, Vertex(0, 0));
	pFace2->SetVertex(2, Vertex(0, -fHeight));
	pFace2->SetVertex(3, Vertex(-fWidth, -fHeight));
	SetPolygon(2, pFace2);

	CPolygon* pFace3 = new CPolygon(4);

	pFace3->SetVertex(0, Vertex(0, 0));
	pFace3->SetVertex(1, Vertex(fWidth, 0));
	pFace3->SetVertex(2, Vertex(fWidth, -fHeight));
	pFace3->SetVertex(3, Vertex(0, -fHeight));
	SetPolygon(3, pFace3);

}