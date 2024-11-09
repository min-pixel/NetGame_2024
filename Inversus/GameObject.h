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
	float					m_fxPosition = 0.0f;		//��ġ ====> ���庯ȯ�� �ʿ��� ������.
	float					m_fyPosition = 0.0f;		//

	float					m_fRotation = 0.0f;		//ȸ�� ����

	Point2D				m_f2MovingDirection = Point2D(0.0f, 0.0f);
	float					m_fMovingSpeed = 0.0f;		//�����̴� �ӵ�
	Mesh* m_pMesh = NULL;        //�޽��� ���� ������
	DWORD					m_dwColor = RGB(255, 0, 0);		//�޽��� �׸��� ���� ����.

	float					m_fRotationSpeed = 0.0f;	//z�� ���� ȸ�� �ӵ��ε� 2D�ϱ� �տ� ������ ���� �ִ� �����ϴ°�.
	bool m_bgravity = false;		//�����ϸ� �� �����ϱ� ���ؼ�

	int	m_nWidth = 0;	//�̰� ����� �Ҵ�ǰ��� �ش� ����� ���� ���� �ִ� ���� ���� ����.
	int m_nHeihgt = 0;

	bool m_bDestoryActive = false; //�̰� true���¸� �ε�ġ�� �浹�ϴ� ������Ʈ�� �����Ѵ�.


public:
	void SetMesh(Mesh* pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }

	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }

	void SetPosition(float x, float y) { m_fxPosition = x; m_fyPosition = y; }
	void SetRotation(float r) { m_fRotation = r; }
	void SetRotationSpeed(float f) { m_fRotationSpeed = f; }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }	//�����̴� �ӵ�
	void SetMovingDirection(Point2D& f2MovingDirection) { m_f2MovingDirection = Vector2::normalize(f2MovingDirection); }	// ��������
	void Move(float x, float y);//�����̵�
	void Rotate(float r) { m_fRotation += r; }	//ȸ��

	void CreatePhysicsBody(b2World* world, float x, float y, float width, float height);	//�ܼ� �簢��
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
	int m_nthisDestoryItemCount = 3;	//�ı� ������ 2�� ��� ���� ���� �ǹ��Ѵ�.
	int m_notherDestoryItemCount = 3;	//������ �ı�
	int m_nChangeBlockCount = 5; //���� ���̴� ���� �������� �� �ִ� Ƚ��.
	virtual void SetPosition(float x, float y);		//��ġ
	void SetRotation(float r);		//����

	void Move(float x, float y);		//�̵�
	void Rotate(float r);	//ȸ��

	void SetCamera(Camera* pCamera) { m_pCamera = pCamera; }
	Camera* GetCamera() { return(m_pCamera); }
};

