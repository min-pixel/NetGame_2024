#include "MainH.h"
#include "GameObject.h"
#include "GraphicsPipeline.h"

b2Body* CreateTShapeBody(b2World& world, float x, float y, float width, float height, float thickness)
{
	// 바디 정의
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody; // 동적 바디
	bodyDef.position.Set(x, y); // 초기 위치 설정

	// 바디 생성
	b2Body* body = world.CreateBody(&bodyDef);

	// 수평 직사각형 모양 정의 (T자형의 윗부분)
	b2PolygonShape horizontalBox;
	horizontalBox.SetAsBox(width * 1.5f, thickness / 2.0f, b2Vec2(0.0f, thickness * 0.5f), 0.0f);

	// 수직 직사각형 모양 정의 (T자형의 아래부분)
	b2PolygonShape verticalBox;
	verticalBox.SetAsBox(thickness / 2.0f, height / 2.0f, b2Vec2(0.0f, -thickness * 0.5f), 0.0f);

	// 수평 피처 정의
	b2FixtureDef horizontalFixtureDef;
	horizontalFixtureDef.shape = &horizontalBox;
	horizontalFixtureDef.density = 100.0f; // 밀도
	horizontalFixtureDef.friction = 1.0f; // 마찰력


	// 수직 피처 정의
	b2FixtureDef verticalFixtureDef;
	verticalFixtureDef.shape = &verticalBox;
	verticalFixtureDef.density = 100.0f; // 밀도
	verticalFixtureDef.friction = 1.0f; // 마찰력

	// 피처를 바디에 추가
	body->CreateFixture(&horizontalFixtureDef);
	body->CreateFixture(&verticalFixtureDef);

	return body;
}

//그 꺾인거
b2Body* Create2x2ShapeBody(b2World& world, float x, float y, float width, float height, float thickness, int change)
{
	// 바디 정의
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody; // 동적 바디
	bodyDef.position.Set(x, y); // 초기 위치 설정

	// 바디 생성
	b2Body* body = world.CreateBody(&bodyDef);

	// 수평 직사각형 모양 정의 (ㄱ자형의 윗부분)
	b2PolygonShape horizontalBox;
	// 수직 직사각형 모양 정의 (ㄱ자형의 아래부분)
	b2PolygonShape verticalBox;
	if (change == 0) //오른쪽
	{
		horizontalBox.SetAsBox(width, thickness / 2.0f, b2Vec2(width / 2.0f, thickness / 2.0f), 0.0f);
		verticalBox.SetAsBox(width, height / 2.0f, b2Vec2(-width / 2.0f, -height / 2.0f), 0.0f);
	}
	else if (change == 1) //왼쪽
	{
		horizontalBox.SetAsBox(width, thickness / 2.0f, b2Vec2(-width / 2.0f, thickness / 2.0f), 0.0f);
		verticalBox.SetAsBox(width, height / 2.0f, b2Vec2(width / 2.0f, -height / 2.0f), 0.0f);
	}

	// 수평 피처 정의
	b2FixtureDef horizontalFixtureDef;
	horizontalFixtureDef.shape = &horizontalBox;
	horizontalFixtureDef.density = 100.0f; // 밀도
	horizontalFixtureDef.friction = 1.0f; // 마찰력

	// 수직 피처 정의
	b2FixtureDef verticalFixtureDef;
	verticalFixtureDef.shape = &verticalBox;
	verticalFixtureDef.density = 100.0f; // 밀도
	verticalFixtureDef.friction = 1.0f; // 마찰력

	// 피처를 바디에 추가
	body->CreateFixture(&horizontalFixtureDef);
	body->CreateFixture(&verticalFixtureDef);

	return body;
}
b2Body* CreateLShapeBody(b2World& world, float x, float y, float width, float height, int change)
{
	// 바디 정의
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody; // 동적 바디
	bodyDef.position.Set(x, y); // 초기 위치 설정

	// 바디 생성
	b2Body* body = world.CreateBody(&bodyDef);

	// 수평 직사각형 모양 정의 (ㄱ자형의 윗부분)
	b2PolygonShape horizontalBox1;
	// 수직 직사각형 모양 정의 (ㄱ자형의 아래부분)
	b2PolygonShape horizontalBox2;
	if (change == 0) //오른쪽
	{
		horizontalBox1.SetAsBox(width, height / 2.0f, b2Vec2(0.0f, height), 0.0f);
		horizontalBox2.SetAsBox(width / 2.0f, height, b2Vec2(-width / 2.0f, -height / 2.0f), 0.0f);
	}
	else if (change == 1) //왼쪽
	{
		horizontalBox1.SetAsBox(width, height / 2.0f, b2Vec2(0.0f, height), 0.0f);
		horizontalBox2.SetAsBox(width / 2.0f, height, b2Vec2(width / 2.0f, -height / 2.0f), 0.0f);
	}

	// 수평 피처 정의
	b2FixtureDef horizontalFixtureDef;
	horizontalFixtureDef.shape = &horizontalBox1;
	horizontalFixtureDef.density = 100.0f; // 밀도
	horizontalFixtureDef.friction = 1.0f; // 마찰력

	// 수직 피처 정의
	b2FixtureDef horizontalFixtureDef2;
	horizontalFixtureDef2.shape = &horizontalBox2;
	horizontalFixtureDef2.density = 100.0f; // 밀도
	horizontalFixtureDef2.friction = 1.0f; // 마찰력

	// 피처를 바디에 추가
	body->CreateFixture(&horizontalFixtureDef);
	body->CreateFixture(&horizontalFixtureDef2);

	return body;
}
b2Body* CreatePhysicsBody(b2World& world, float x, float y, float width, float height)
{
	// 바디 정의
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody; // 동적 바디
	bodyDef.position.Set(x, y); // 초기 위치 설정

	// 바디 생성
	b2Body* body = world.CreateBody(&bodyDef);

	// 사각형 모양 정의
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(width / 2.0f, height / 2.0f);

	// 피처 정의
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 100.0f; // 밀도
	fixtureDef.friction = 1.0f; // 마찰력

	// 피처를 바디에 추가
	body->CreateFixture(&fixtureDef);

	return body;
}

b2Body* CreatestaticPhysicsBody(b2World& world, float x, float y, float width, float height)
{
	b2BodyDef groundbodyDef;

	groundbodyDef.position.Set(x, y); // 초기 위치 설정

	// 바디 생성
	b2Body* body = world.CreateBody(&groundbodyDef);

	// 사각형 모양 정의
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(width / 2.0f, height / 2.0f);

	// 피처 정의
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f; // 밀도
	fixtureDef.friction = 0.3f; // 마찰력

	// 피처를 바디에 추가
	body->CreateFixture(&fixtureDef);

	return body;
}
GameObject::~GameObject(void)
{
	if (m_pMesh) m_pMesh->Release();
}

Point2D GameObject::WorldTransform(Point2D& f2Model)	//월드 변환 순서=> 회전->위치옮기기 (자전 방식)
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
		// Box2D의 위치와 회전을 가져와서 GameObject에 반영
		const b2Vec2& pos = m_pBody->GetPosition();
		m_fxPosition = pos.x;
		m_fyPosition = pos.y;
		m_fRotation = m_pBody->GetAngle() * (180.0f / b2_pi); // 라디안을 도로 변환
	}

}
void GameObject::Move(float x, float y)
{
	m_fxPosition += x;
	m_fyPosition += y;

}
void GameObject::Animate(float fElapsedTime)
{
	//if (!m_bgravity)	//그래비티가 활성화되면 동작을 못하게 설정.
	//{
	//	Rotate(m_fRotationSpeed * fElapsedTime);
	//	Move(fElapsedTime * m_fMovingSpeed * m_f2MovingDirection.x, fElapsedTime * m_fMovingSpeed * m_f2MovingDirection.y);
	//}
	UpdateFromPhysics();	//동작을 동기화 시킨다
}

void GameObject::Render(HDC hDCFrameBuffer, Camera* pCamera)	//Scene에서 호출 했다.
{
	GraphicsPipeline::SetGameObject(this);		//그래픽스파이프라인에 SetGameObject를 호출
	HBRUSH hBrush, oldBrush;
	hBrush = CreateSolidBrush(m_dwColor);
	oldBrush = (HBRUSH)SelectObject(hDCFrameBuffer, hBrush);
	if (m_pMesh) m_pMesh->Render(hDCFrameBuffer);		//Mesh를 렌더하라
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
	GameObject::Move(0, y);	//이거 높이 측정해서 해당 높이에 움직이는거 구현해줄꺼다.
	m_pCamera->Move(0, y);
}

void Player::Rotate(float r)
{
	GameObject::Rotate(r);
}


