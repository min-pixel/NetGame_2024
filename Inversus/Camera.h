#pragma once
#include "Mesh.h"


class Viewport //화면 클라이언트 영역의 투영 사각형이 그려지기 위한 위치를 갖고있다.
{
public:
	Viewport(int nLeft, int nTop, int nWidth, int nHeight) { m_nLeft = nLeft; m_nTop = nTop; m_nWidth = nWidth; m_nHeight = nHeight; }
	virtual ~Viewport() { }

	int							m_nLeft;
	int							m_nTop;
	int							m_nWidth;
	int							m_nHeight;
};
class Camera
{
public:
	Camera();
	virtual ~Camera();
private:
	float						m_fxPosition = 0.0f;		//위치
	float						m_fyPosition = 0.0f;

	Viewport* m_pViewport = NULL;

public:
	Viewport* GetViewport();
	Point2D CameraTransform(Point2D& f2World);
	Point2D ScreenTransform(Point2D& f2Camera);
	void SetPosition(float x, float y) { m_fxPosition = x; m_fyPosition = y; }
	void SetViewport(int xStart, int yStart, int nWidth, int nHeight);
	Point2D GetPosition();

	void Move(float x, float y);
};

