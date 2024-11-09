#pragma once
#include "MainH.h"


class Vertex
{
public:
	Vertex() { m_f2Position = Point2D(0.0f, 0.0f); }
	Vertex(float x, float y) { m_f2Position = Point2D(x, y); }
	~Vertex() { }

	Point2D					m_f2Position;
};

class CPolygon
{
public:
	CPolygon() { }
	CPolygon(int nVertices);
	~CPolygon();

	int							m_nVertices = 0;
	Vertex* m_pVertices = NULL;

	void SetVertex(int nIndex, Vertex vertex);
};

class Mesh
{
public:
	Mesh() {}
	Mesh(int nPolygons);
	virtual ~Mesh();

private:
	int							m_nReferences = 1;

public:
	void AddRef() { m_nReferences++; }
	void Release() { m_nReferences--; if (m_nReferences <= 0) delete this; }
private:
	int		m_nPolygons = 0;
	CPolygon** m_ppPolygons = NULL;

public:
	void SetPolygon(int nIndex, CPolygon* pPolygon);

	virtual void Render(HDC hDCFrameBuffer);
};

class cubeB : public Mesh
{
public:
	cubeB(float fWidth = 4.0f, float fHeight = 4.0f);
	virtual ~cubeB();

};

class TShapeMesh : public Mesh
{
public:
	TShapeMesh(float fWidth = 4.0f, float fHeight = 4.0f);
	virtual ~TShapeMesh() {}
};

class LRShapeMesh : public Mesh
{
public:
	LRShapeMesh(float fWidth, float fHeight);
	virtual~LRShapeMesh() {}
};

class LLShapeMesh : public Mesh
{
public:
	LLShapeMesh(float fWidth, float fHeight);
	virtual~LLShapeMesh() {}
};
class Cube2x2LMesh : public Mesh
{
public:
	Cube2x2LMesh(float fWidth, float fHeight);
	virtual~Cube2x2LMesh() {}
};
class Cube2x2RMesh : public Mesh
{
public:
	Cube2x2RMesh(float fWidth, float fHeight);
	virtual~Cube2x2RMesh() {}
};
class Cube3x1Mesh : public Mesh
{
public:
	Cube3x1Mesh(float fWidth, float fHeight);
	virtual ~Cube3x1Mesh() {}
};

class Cube4x1Mesh : public Mesh
{
public:
	Cube4x1Mesh(float fWidth, float fHeight);
	virtual ~Cube4x1Mesh() {}
};
class Cube2x2Mesh : public Mesh
{
public:
	Cube2x2Mesh(float fWidth, float fHeight);
	virtual ~Cube2x2Mesh() {}
};

