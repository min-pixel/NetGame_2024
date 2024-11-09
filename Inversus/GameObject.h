#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "MainH.h"
class GameObject
{
public:
	GameObject() { }
	~GameObject();
	b2Body* m_pBody = NULL;
	b2World* m_pWorld = NULL;

public:
	float					m_fxPosition = 0.0f;		//위치 ====> 월드변환에 필요한 정보들.
	float					m_fyPosition = 0.0f;		//

	float					m_fRotation = 0.0f;		//회전 정보

	Point2D				m_f2MovingDirection = Point2D(0.0f, 0.0f);
	float					m_fMovingSpeed = 0.0f;		//움직이는 속도
	Mesh* m_pMesh = NULL;        //메쉬에 대한 포인터
	DWORD					m_dwColor = RGB(255, 0, 0);		//메쉬를 그리기 위한 색깔.

	float					m_fRotationSpeed = 0.0f;	//z축 방향 회전 속도인데 2D니까 앞에 가상의 축이 있다 생각하는거.
	bool m_bgravity = false;		//스케일링 값 변경하기 위해서

	int	m_nWidth = 0;	//이건 블록이 할당되고나서 해당 블록의 가로 세로 최대 길이 값을 저장.
	int m_nHeihgt = 0;

	bool m_bDestoryActive = false; //이게 true상태면 부딪치는 충돌하는 오브젝트를 제거한다.


public:
	void SetMesh(Mesh* pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }

	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }

	void SetPosition(float x, float y) { m_fxPosition = x; m_fyPosition = y; }
	void SetRotation(float r) { m_fRotation = r; }
	void SetRotationSpeed(float f) { m_fRotationSpeed = f; }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }	//움직이는 속도
	void SetMovingDirection(Point2D& f2MovingDirection) { m_f2MovingDirection = Vector2::normalize(f2MovingDirection); }	// 방향지정
	void Move(float x, float y);//평행이동
	void Rotate(float r) { m_fRotation += r; }	//회전

	void CreatePhysicsBody(b2World* world, float x, float y, float width, float height);	//단순 사각형
	void CreateTShapeBody(b2World* world, float x, float y, float width, float height, float thickness); //T
	void Create2x2ShapeBody(b2World* world, float x, float y, float width, float height, float thickness, int change);
	void CreateLShapeBody(b2World* world, float x, float y, float width, float height, int change);
	void CreatestaticPhysicsBody(b2World* world, float x, float y, float width, float height);
	void UpdateFromPhysics();


public:
	Point2D WorldTransform(Point2D& f3Model);

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, Camera* pCamera);
};

class Player : public GameObject
{
public:
	Player();
	virtual ~Player();

private:
	Camera* m_pCamera = NULL;

public:
	int m_nthisDestoryItemCount = 3;	//파괴 아이템 2번 사용 가능 함을 의미한다.
	int m_notherDestoryItemCount = 3;	//남에꺼 파괴
	int m_nChangeBlockCount = 5; //현재 보이는 블럭을 변경해줄 수 있는 횟수.
	virtual void SetPosition(float x, float y);		//위치
	void SetRotation(float r);		//방향

	void Move(float x, float y);		//이동
	void Rotate(float r);	//회전

	void SetCamera(Camera* pCamera) { m_pCamera = pCamera; }
	Camera* GetCamera() { return(m_pCamera); }
};

