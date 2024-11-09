#include "MainH.h"
#include "Mesh.h"
#include "GraphicsPipeline.h"

CPolygon::CPolygon(int nVertices)
{
	m_nVertices = nVertices;		//���� �������� ������ ����
	m_pVertices = new Vertex[nVertices]; //���� �������� ���� ���� ��ŭ ���� �迭�� ���� ����.
}

CPolygon::~CPolygon()
{
	if (m_pVertices) delete[] m_pVertices;
}

void CPolygon::SetVertex(int nIndex, Vertex vertex)
{
	if ((0 <= nIndex) && (nIndex < m_nVertices) && m_pVertices)	//�������� ���鶧 ������ ������ �Է��صּ� �ش� �ε����� ������ ������ �ο����ش�.
	{
		m_pVertices[nIndex] = vertex;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
Mesh::Mesh(int nPolygons)	//�������� ������ �Է� �޴´�.
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

void Mesh::SetPolygon(int nIndex, CPolygon* pPolygon)	//�ش� �޽��� �������� �߰����ش�. ==> �� �켱 ���� 1���� ���������θ� �̷���� ģ������ ����� ����.
{
	if ((0 <= nIndex) && (nIndex < m_nPolygons)) m_ppPolygons[nIndex] = pPolygon;
}

void Draw2DPolygon(HDC hDCFrameBuffer, CPolygon* polygon)	//�޽��� ������ ���·� �׸� �����̶� �������� ���.
{
	Point2D* Vertices = new Point2D[polygon->m_nVertices];	//���� ��ȯ�� ���Ͼ ������ �޾ƿԴ�.
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

	delete[] Vertices;	//�迭 ����
	delete[] point;
}
void Mesh::Render(HDC hDCFrameBuffer)
{
	Point2D f2InitialProject, f2PreviousProject;
	int nWidth = GraphicsPipeline::GetScreenViewport()->m_nWidth;	//���� ���� �׸� ����Ʈ 2D ���ӿ����� ����Ʈ�� ȭ���̶� ����
	int nHeight = GraphicsPipeline::GetScreenViewport()->m_nHeight;	//�ű� ���� ������ ���� �׷������� �ҷ��� �������

	for (int j = 0; j < m_nPolygons; j++)
	{
		int ICount = 0;		//0�̸� DrawPolygon�� �������� �ʴ´�.
		int nVertices = m_ppPolygons[j]->m_nVertices;		//������ ����
		Vertex* pVertices = m_ppPolygons[j]->m_pVertices;		//�������� Vertoces �迭��.

		f2PreviousProject = f2InitialProject = GraphicsPipeline::Transform(pVertices[0].m_f2Position);		//0��° ������ ��ǥ ���� ����.
		if ((-nWidth / 2 <= f2InitialProject.x) && (f2InitialProject.x <= nWidth / 2) && (-nHeight / 2 <= f2InitialProject.y) && (f2InitialProject.y <= nHeight / 2)) //x: -1 ~ 1, y: -1 ~ 1 ����� �׸��� �ʴ´�.
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
		if (ICount != 0)	//�ش� �������� ������ �ϳ��� ����Ʈ ���ο������� �׳� �׸����� ������ �߾�.
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

LRShapeMesh::LRShapeMesh(float fWidth, float fHeight) : Mesh(4) {	//���� ������ ������ ����
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

LLShapeMesh::LLShapeMesh(float fWidth, float fHeight) : Mesh(4) {	//���� ������ ���� ����
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

Cube2x2LMesh::Cube2x2LMesh(float fWidth, float fHeight) : Mesh(4) //���ʹ��� �޽�
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

Cube2x2RMesh::Cube2x2RMesh(float fWidth, float fHeight) : Mesh(4) //���ʹ��� �޽�
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