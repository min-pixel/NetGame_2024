#include "MainH.h"
#include "Camera.h"
#include "Mesh.h"
Camera::Camera()
{

}

Camera::~Camera()
{

}

void Camera::SetViewport(int nLeft, int nTop, int nWidth, int nHeight) //2D 에서는 이게 투영사각형이자 뷰포트이며 보이는 범위를 지정해줄 것이다.
{
	m_pViewport = new Viewport(nLeft, nTop, nWidth, nHeight);
}


Point2D Camera::GetPosition()
{
	Point2D position(m_fxPosition, m_fyPosition);
	return position;
}
void Camera::Move(float x, float y)	//카메라 움직임
{
	m_fxPosition += x;
	m_fyPosition += y;
}

Point2D Camera::CameraTransform(Point2D& f2World)
{
	Point2D f2Camera = f2World;
	f2Camera.x -= m_fxPosition;	//카메라 좌표계 기준 원점으로 옮기기 위해서. 화면에 대한 회전은 생각을 안할꺼니 Rotation은 뺄 예정.
	f2Camera.y -= m_fyPosition;

	return(f2Camera);
}

Point2D Camera::ScreenTransform(Point2D& f2Camera)
{
	Point2D f2Screen;
	f2Screen.x = f2Camera.x *1;			//transform으로 
	f2Screen.y = f2Camera.y *1;

	float fHalfWidth = m_pViewport->m_nWidth * 0.5f;
	float fHalfHeight = m_pViewport->m_nHeight * 0.5f;
	f2Screen.x = m_pViewport->m_nLeft + fHalfWidth + f2Screen.x;
	f2Screen.y = m_pViewport->m_nTop + fHalfHeight - f2Screen.y;

	return(f2Screen);
}

Viewport* Camera::GetViewport()
{
	return m_pViewport;
}