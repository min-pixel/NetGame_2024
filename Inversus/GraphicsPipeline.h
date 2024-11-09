#pragma once
#include "GameObject.h"
#include "Camera.h"

class GraphicsPipeline
{
private:
	static GameObject* m_pGameObject;
	static Camera* m_pCamera;

public:
	static void SetCamera(Camera* pCamera) { m_pCamera = pCamera; }
	static void SetGameObject(GameObject* pGameObject) { m_pGameObject = pGameObject; }
	static Viewport* GetScreenViewport();
	static Point2D ScreenTransform(Point2D& f2Projection);
	static Point2D Transform(Point2D& f3Model);
};
