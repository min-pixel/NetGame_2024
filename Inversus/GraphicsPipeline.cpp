#include "MainH.h"
#include "GraphicsPipeline.h"

GameObject* GraphicsPipeline::m_pGameObject = NULL;
Camera* GraphicsPipeline::m_pCamera = NULL;

Point2D GraphicsPipeline::Transform(Point2D& f2Model)
{
	Point2D f2World = m_pGameObject->WorldTransform(f2Model);
	Point2D f2Camera = m_pCamera->CameraTransform(f2World);	//2D�� ���庯ȯ�� ī�޶� ��ȯ�� ������ �Ǵµ� �̶� ī�޶� ��ȯ�� 3D �����������̶� �ٸ���.
	return(f2Camera);
}

Point2D GraphicsPipeline::ScreenTransform(Point2D& f2Camera)
{
	Point2D f2Screen = m_pCamera->ScreenTransform(f2Camera);
	return(f2Screen);
}

Viewport* GraphicsPipeline::GetScreenViewport()
{
	return(m_pCamera->GetViewport());
}






