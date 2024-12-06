#include "MainH.h"
#include "GameObject.h"
#include "GraphicsPipeline.h"

b2Body* CreateTShapeBody(b2World& world, float x, float y, float width, float height, float thickness)
{
	// �ٵ� ����
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody; // ���� �ٵ�
	bodyDef.position.Set(x, y); // �ʱ� ��ġ ����

	// �ٵ� ����
	b2Body* body = world.CreateBody(&bodyDef);

	// ���� ���簢�� ��� ���� (T������ ���κ�)
	b2PolygonShape horizontalBox;
	horizontalBox.SetAsBox(width * 1.5f, thickness / 2.0f, b2Vec2(0.0f, thickness * 0.5f), 0.0f);

	// ���� ���簢�� ��� ���� (T������ �Ʒ��κ�)
	b2PolygonShape verticalBox;
	verticalBox.SetAsBox(thickness / 2.0f, height / 2.0f, b2Vec2(0.0f, -thickness * 0.5f), 0.0f);

	// ���� ��ó ����
	b2FixtureDef horizontalFixtureDef;
	horizontalFixtureDef.shape = &horizontalBox;
	horizontalFixtureDef.density = 100.0f; // �е�
	horizontalFixtureDef.friction = 1.0f; // ������


	// ���� ��ó ����
	b2FixtureDef verticalFixtureDef;
	verticalFixtureDef.shape = &verticalBox;
	verticalFixtureDef.density = 100.0f; // �е�
	verticalFixtureDef.friction = 1.0f; // ������

	// ��ó�� �ٵ� �߰�
	body->CreateFixture(&horizontalFixtureDef);
	body->CreateFixture(&verticalFixtureDef);

	return body;
}

//�� ���ΰ�
b2Body* Create2x2ShapeBody(b2World& world, float x, float y, float width, float height, float thickness, int change)
{
	// �ٵ� ����
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody; // ���� �ٵ�
	bodyDef.position.Set(x, y); // �ʱ� ��ġ ����

	// �ٵ� ����
	b2Body* body = world.CreateBody(&bodyDef);

	// ���� ���簢�� ��� ���� (�������� ���κ�)
	b2PolygonShape horizontalBox;
	// ���� ���簢�� ��� ���� (�������� �Ʒ��κ�)
	b2PolygonShape verticalBox;
	if (change == 0) //������
	{
		horizontalBox.SetAsBox(width, thickness / 2.0f, b2Vec2(width / 2.0f, thickness / 2.0f), 0.0f);
		verticalBox.SetAsBox(width, height / 2.0f, b2Vec2(-width / 2.0f, -height / 2.0f), 0.0f);
	}
	else if (change == 1) //����
	{
		horizontalBox.SetAsBox(width, thickness / 2.0f, b2Vec2(-width / 2.0f, thickness / 2.0f), 0.0f);
		verticalBox.SetAsBox(width, height / 2.0f, b2Vec2(width / 2.0f, -height / 2.0f), 0.0f);
	}

	// ���� ��ó ����
	b2FixtureDef horizontalFixtureDef;
	horizontalFixtureDef.shape = &horizontalBox;
	horizontalFixtureDef.density = 100.0f; // �е�
	horizontalFixtureDef.friction = 1.0f; // ������

	// ���� ��ó ����
	b2FixtureDef verticalFixtureDef;
	verticalFixtureDef.shape = &verticalBox;
	verticalFixtureDef.density = 100.0f; // �е�
	verticalFixtureDef.friction = 1.0f; // ������

	// ��ó�� �ٵ� �߰�
	body->CreateFixture(&horizontalFixtureDef);
	body->CreateFixture(&verticalFixtureDef);

	return body;
}
b2Body* CreateLShapeBody(b2World& world, float x, float y, float width, float height, int change)
{
	// �ٵ� ����
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody; // ���� �ٵ�
	bodyDef.position.Set(x, y); // �ʱ� ��ġ ����

	// �ٵ� ����
	b2Body* body = world.CreateBody(&bodyDef);

	// ���� ���簢�� ��� ���� (�������� ���κ�)
	b2PolygonShape horizontalBox1;
	// ���� ���簢�� ��� ���� (�������� �Ʒ��κ�)
	b2PolygonShape horizontalBox2;
	if (change == 0) //������
	{
		horizontalBox1.SetAsBox(width, height / 2.0f, b2Vec2(0.0f, height), 0.0f);
		horizontalBox2.SetAsBox(width / 2.0f, height, b2Vec2(-width / 2.0f, -height / 2.0f), 0.0f);
	}
	else if (change == 1) //����
	{
		horizontalBox1.SetAsBox(width, height / 2.0f, b2Vec2(0.0f, height), 0.0f);
		horizontalBox2.SetAsBox(width / 2.0f, height, b2Vec2(width / 2.0f, -height / 2.0f), 0.0f);
	}

	// ���� ��ó ����
	b2FixtureDef horizontalFixtureDef;
	horizontalFixtureDef.shape = &horizontalBox1;
	horizontalFixtureDef.density = 100.0f; // �е�
	horizontalFixtureDef.friction = 1.0f; // ������

	// ���� ��ó ����
	b2FixtureDef horizontalFixtureDef2;
	horizontalFixtureDef2.shape = &horizontalBox2;
	horizontalFixtureDef2.density = 100.0f; // �е�
	horizontalFixtureDef2.friction = 1.0f; // ������

	// ��ó�� �ٵ� �߰�
	body->CreateFixture(&horizontalFixtureDef);
	body->CreateFixture(&horizontalFixtureDef2);

	return body;
}
b2Body* CreatePhysicsBody(b2World& world, float x, float y, float width, float height)
{
	// �ٵ� ����
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody; // ���� �ٵ�
	bodyDef.position.Set(x, y); // �ʱ� ��ġ ����

	// �ٵ� ����
	b2Body* body = world.CreateBody(&bodyDef);

	// �簢�� ��� ����
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(width / 2.0f, height / 2.0f);

	// ��ó ����
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 100.0f; // �е�
	fixtureDef.friction = 1.0f; // ������

	// ��ó�� �ٵ� �߰�
	body->CreateFixture(&fixtureDef);

	return body;
}

b2Body* CreatestaticPhysicsBody(b2World& world, float x, float y, float width, float height)
{
	b2BodyDef groundbodyDef;

	groundbodyDef.position.Set(x, y); // �ʱ� ��ġ ����

	// �ٵ� ����
	b2Body* body = world.CreateBody(&groundbodyDef);

	// �簢�� ��� ����
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(width / 2.0f, height / 2.0f);

	// ��ó ����
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f; // �е�
	fixtureDef.friction = 0.3f; // ������

	// ��ó�� �ٵ� �߰�
	body->CreateFixture(&fixtureDef);

	return body;
}
GameObject::~GameObject(void)
{
	if (m_pMesh) m_pMesh->Release();
}

Point2D GameObject::WorldTransform(Point2D& f2Model)	//���� ��ȯ ����=> ȸ��->��ġ�ű�� (���� ���)
{
	float fRoll = DegreeToRadian(m_fRotation);

	Point2D f2World = f2Model;
	Point2D f2Rotated = f2Model;

	if (fRoll != 0.0f)
	{
		f2Rotated.x = float(f2World.x * cos(fRoll) - f2World.y * sin(fRoll));
		f2Rotated.y = float(f2World.x * sin(fRoll) + f2World.y * cos(fRoll));
		f2World.x = f2Rotated.x;
		f2World.y = f2Rotated.y;
	}
	f2World.x += m_fxPosition;
	f2World.y += m_fyPosition;
	return(f2World);
}


void GameObject::CreatePhysicsBody(b2World* world, float x, float y, float width, float height)
{
	m_pWorld = world;
	m_pBody = ::CreatePhysicsBody(*world, x, y, width, height);
}
void GameObject::CreateTShapeBody(b2World* world, float x, float y, float width, float height, float thickness)
{
	m_pWorld = world;
	m_pBody = ::CreateTShapeBody(*world, x, y, width, height, thickness);
}
void GameObject::CreatestaticPhysicsBody(b2World* world, float x, float y, float width, float height)
{
	m_pWorld = world;
	m_pBody = ::CreatestaticPhysicsBody(*world, x, y, width, height);
}
void GameObject::Create2x2ShapeBody(b2World* world, float x, float y, float width, float height, float thickness, int change)
{
	m_pWorld = world;
	m_pBody = ::Create2x2ShapeBody(*world, x, y, width, height, thickness, change);
}
void GameObject::CreateLShapeBody(b2World* world, float x, float y, float width, float height, int change)
{
	m_pWorld = world;
	m_pBody = ::CreateLShapeBody(*world, x, y, width, height, change);
}
void GameObject::UpdateFromPhysics()
{
	if (m_pBody)
	{
		// Box2D�� ��ġ�� ȸ���� �����ͼ� GameObject�� �ݿ�
		const b2Vec2& pos = m_pBody->GetPosition();
		m_fxPosition = pos.x;
		m_fyPosition = pos.y;
		m_fRotation = m_pBody->GetAngle() * (180.0f / b2_pi); // ������ ���� ��ȯ
	}

}
void GameObject::Move(float x, float y)
{
	m_fxPosition += x;
	m_fyPosition += y;

}
void GameObject::Animate(float fElapsedTime)
{
	//if (!m_bgravity)	//�׷���Ƽ�� Ȱ��ȭ�Ǹ� ������ ���ϰ� ����.
	//{
	//	Rotate(m_fRotationSpeed * fElapsedTime);
	//	Move(fElapsedTime * m_fMovingSpeed * m_f2MovingDirection.x, fElapsedTime * m_fMovingSpeed * m_f2MovingDirection.y);
	//}
	UpdateFromPhysics();	//������ ����ȭ ��Ų��
}

void GameObject::Render(HDC hDCFrameBuffer, Camera* pCamera)	//Scene���� ȣ�� �ߴ�.
{
	GraphicsPipeline::SetGameObject(this);		//�׷��Ƚ����������ο� SetGameObject�� ȣ��
	HBRUSH hBrush, oldBrush;
	hBrush = CreateSolidBrush(m_dwColor);
	oldBrush = (HBRUSH)SelectObject(hDCFrameBuffer, hBrush);
	if (m_pMesh) m_pMesh->Render(hDCFrameBuffer);		//Mesh�� �����϶�
	SelectObject(hDCFrameBuffer, oldBrush);
	DeleteObject(hBrush);
}
/// <summary>
/// ///////////////////////////////////////////////////////////////////////////////////////////////////
/// </summary>
Player::Player()
{

}
Player::~Player()
{

}

void Player::SetPosition(float x, float y)
{
	GameObject::SetPosition(x, y);
	if (m_pCamera) m_pCamera->SetPosition(x, y);
}

void Player::SetRotation(float r)
{
	GameObject::SetRotation(r);
}

void Player::Move(float x, float y)
{
	GameObject::Move(0, y);	//�̰� ���� �����ؼ� �ش� ���̿� �����̴°� �������ٲ���.
	m_pCamera->Move(0, y);
}

void Player::Rotate(float r)
{
	GameObject::Rotate(r);
}


