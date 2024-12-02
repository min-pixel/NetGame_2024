#include "MainH.h"
#include "Scene.h"
#include "GraphicsPipeline.h"
#include <thread>


Scene::Scene(Player* pPlayer, Player* pPlayer2, b2World* world, HINSTANCE Instance) : m_lastKeyPressed(0), m_fElapsedTimeForPlayerTwo(0.0f)  // 초기화 리스트에서 m_lastKeyPressed를 0으로 초기화
{
	m_pPlayer = pPlayer;
	m_pPlayer2 = pPlayer2;
	m_b2World = world;
	m_b2World->SetContactListener(&contactListener);	//충돌 판정 관련해서 값을 넣어준다.
	m_hInstance = Instance;

	m_bitmapBlend = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP1));
	m_bitmapBlend2 = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP1));
}

Scene::~Scene()
{
}


void Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	//값을 받아오기 위함
	m_MouseX = LOWORD(lParam);
	m_MouseY = HIWORD(lParam);
	//버튼UI 로직
	if (nMessageID == WM_LBUTTONDOWN) {
		// start 버튼을 클릭 했을때 로직
		if (m_MouseX > m_DbtnLeft && m_MouseX< m_DbtnRight && m_MouseY > m_DbtnTop && m_MouseY < m_DbtnBottom)
		{
			m_pPlayer->m_nthisDestoryItemCount = 3;
			m_pPlayer->m_notherDestoryItemCount = 3;
			m_pPlayer->m_nChangeBlockCount = 5;
			m_pPlayer2->m_nthisDestoryItemCount = 3;
			m_pPlayer2->m_notherDestoryItemCount = 3;
			m_pPlayer2->m_nChangeBlockCount = 5;

			// 게임 시작을 서버에 알리는 패킷 전송
			/*if (m_pNetworkManager) {
				m_pNetworkManager->SendAcceptServerPacket();
			}*/

			//ReleaseObjects();	//오브젝트를 삭제하고
			//BuildObjects();	//오브젝트를 생성하고

			//m_bGameStart = false;
			//m_bGameStop = false; //11월 19일 추가 : 재시작 안 하면 전송 안 하던 걸 해결.
			m_pNetworkManager = new NetworkManager();
			if (m_pNetworkManager->ConnectToServer("127.0.0.1", 9000)) { // 서버 IP와 포트를 입력
				// 연결이 성공한 경우, 클라이언트 전송 스레드를 시작

				// 연결이 성공하면 서버의 시작 신호를 대기
				bool startSignal = false;
				while (!startSignal) {
					m_pNetworkManager->ReceiveStartSignal(startSignal);  // 시작 신호를 받는 함수를 추가
				}

				// 시작 신호를 받은 후 게임 시작
				ReleaseObjects();	//오브젝트를 삭제하고
				BuildObjects();	//오브젝트를 생성하고



				m_bGameStart = true;
				m_bGameStop = false; //11월 19일 추가 : 재시작 안 하면 전송 안 하던 걸 해결.

				std::thread clientSendThread(&NetworkManager::Client_Send_Thread, m_pNetworkManager, m_pPlayer, this);
				clientSendThread.detach(); // 스레드를 분리하여 독립적으로 실행

				std::thread receiveThread(&NetworkManager::ReceiveThread, m_pNetworkManager, m_pPlayer, this); 
				receiveThread.detach();

				

				

			}
			else {
				// 연결 실패 처리
				MessageBox(NULL, L"서버에 연결할 수 없습니다.", L"네트워크 오류", MB_OK | MB_ICONERROR);
			}



		}// exit 버튼을 클릭 했을때 로직
		else if (m_MouseX < m_DbtnLeft + (FRAMEBUFFER_WIDTH / 4) && m_MouseX> m_DbtnRight - (FRAMEBUFFER_WIDTH / 4) && m_MouseY > (m_DbtnTop + (FRAMEBUFFER_HEIGHT / 10) * 2) && m_MouseY < m_DbtnBottom + (FRAMEBUFFER_HEIGHT / 10) * 2)
		{
			PostQuitMessage(0);
		}
		else if (m_MouseX > m_DbtnLeft / 0.36 && m_MouseY > 0 && m_MouseX < FRAMEBUFFER_WIDTH - 10 && m_MouseY < m_DbtnBottom / 5)
		{
			m_bPauseGame = true;
		}
		else if (m_MouseX > 550 && m_MouseX < 590 && m_MouseY > 110 && m_MouseY < 140)// X키 눌렀을 때
		{
			m_bhideMenu = true;
		}
		if (m_bPauseGame)
		{
			if (m_MouseX > 300 && m_MouseX < 420 && m_MouseY > 200 && m_MouseY < 250) //pause에서 메인버튼 가기
			{
				m_bmainbtn = true;
				m_bGameStart = false;
				m_bPauseGame = false;
			}
			else if (m_MouseX > 300 && m_MouseX < 420 && m_MouseY > 300 && m_MouseY < 350) // pause에서 게임 끝내기
			{
				PostQuitMessage(0);
			}
			else if (m_MouseX > 550 && m_MouseX < 590 && m_MouseY > 110 && m_MouseY < 140)
			{
				m_bPauseGame = false;
			}
		}
		if (m_bGameStop)
		{
			if (m_MouseX > 700 && m_MouseX < 730 && m_MouseY > 55 && m_MouseY < 90)
			{
				m_bGameStop = false;

			}
			else if (m_MouseX > 265 && m_MouseX < 485 && m_MouseY > 450 && m_MouseY < 500)
			{
				m_fTime = 100.0f;//수정 해야함
				m_bGameStop = false;
				m_bGameStart = false;
				m_bPauseGame = false;
				m_bmainbtn = true;
			}
		}
	}
}

void RotateBody(b2Body* body, float angle)
{
	// 현재 바디의 각도를 가져옵니다.
	float currentAngle = body->GetAngle();
	// 새로운 각도를 계산합니다.
	float newAngle = currentAngle + b2_pi * angle / 180.0f; // angle을 라디안으로 변환하여 더합니다.
	// 바디의 각도를 설정합니다.
	body->SetTransform(body->GetPosition(), newAngle);
}
float NormalizeAngle(float angleRad) {	//각도를 어느정도 범위를 고정시켜준다.
	while (angleRad < 0) {
		angleRad += 2 * b2_pi;
	}
	while (angleRad >= 2 * b2_pi) {
		angleRad -= 2 * b2_pi;
	}
	return angleRad;
}
float truncate(float value, int decimal_places) {	//소수점 버리기 함수
	float factor = pow(10.0f, decimal_places);
	return trunc(value * factor) / factor;
}
void Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!m_bPauseGame)	//게임이 Puse상태면
	{
		switch (nMessageID)
		{
		case WM_KEYDOWN:
			m_bKeyInput = true;  // 키 입력이 발생했을 때 true로 설정
			m_lastKeyPressed = wParam;  // 마지막으로 누른 키를 업데이트
			switch (wParam)
			{
				//////플레이어 원에 동작.
			//case 'K':	//블럭을 제거하는 아이템 효과 활성화
			//	if (!m_bTriggerActive)
			//	{
			//		if (m_pPlayer->m_nthisDestoryItemCount > 0)
			//		{
			//			m_objects[m_nIndexPlayerOne]->m_bDestoryActive = true;
			//			m_objects[m_nIndexPlayerOne]->SetColor(RGB(0, 0, 0));
			//			m_pPlayer->m_nthisDestoryItemCount--;
			//		}
			//	}
			//	break;
			//case 'L':	//남의 블럭 부깨기
			//	if (!m_bTriggerActive2)
			//	{
			//		if (m_pPlayer->m_notherDestoryItemCount > 0)
			//		{
			//			m_objects2[m_nIndexPlayerTwo]->m_bDestoryActive = true;
			//			m_objects2[m_nIndexPlayerTwo]->SetColor(RGB(0, 0, 0));
			//			m_pPlayer->m_notherDestoryItemCount--;
			//		}
			//	}
			//	break;
			//case 'J':	//랜덤하게 다음 오브젝트를 변경.
			//	if (m_pPlayer->m_nChangeBlockCount > 0)
			//	{
			//		m_pNextGameObjectOne = RandomMesh(m_pPlayer);
			//		m_pPlayer->m_nChangeBlockCount--;
			//	}

			//	break;
			case VK_LEFT:
				if (!m_bTriggerActive)
				{
					b2Vec2 velocity(-50.0f, -50.0f);
					m_objects[m_nIndexPlayerOne]->m_pBody->SetLinearVelocity(velocity);
				}
				break;
			case VK_RIGHT:
				if (!m_bTriggerActive)
				{
					b2Vec2 velocity(50.0f, -50.0f);
					m_objects[m_nIndexPlayerOne]->m_pBody->SetLinearVelocity(velocity);
				}
				break;
			case VK_UP:
				if (!m_bTriggerActive)
				{
					b2Vec2 velocity(0.0f, -25.0f);
					m_objects[m_nIndexPlayerOne]->m_pBody->SetLinearVelocity(velocity);
				}
				break;
			case VK_DOWN:
				if (m_bTriggerActive)
				{
					m_nIndexPlayerOne = RunTimeBuildObject(m_nIndex, m_pPlayer);	//해당 인덱스에 대해 저장
					m_bTriggerActive = false;	//동작을 수행할 수 있게 만들어주고
					b2Vec2 velocity(0.0f, -50.0f);
					m_objects[m_nIndexPlayerOne]->m_pBody->SetLinearVelocity(velocity);
					m_pNextGameObjectOne = RandomMesh(m_pPlayer);

				}
				else if (!m_bTriggerActive)
				{
					b2Vec2 velocity(0.0f, -500.0f);
					m_objects[m_nIndexPlayerOne]->m_pBody->SetLinearVelocity(velocity);
				}
				break;
			case VK_OEM_COMMA:
				if (!m_bTriggerActive)
				{
					::RotateBody(m_objects[m_nIndexPlayerOne]->m_pBody, -90.0f);
					float normalizedAngle = NormalizeAngle(m_objects[m_nIndexPlayerOne]->m_pBody->GetAngle());
					normalizedAngle = truncate(normalizedAngle, 2);
					printf("%f   ", normalizedAngle);
				}
				break;
			case VK_OEM_PERIOD:
				if (!m_bTriggerActive)
				{
					::RotateBody(m_objects[m_nIndexPlayerOne]->m_pBody, 90.0f);
					float normalizedAngle = NormalizeAngle(m_objects[m_nIndexPlayerOne]->m_pBody->GetAngle());
					normalizedAngle = truncate(normalizedAngle, 2);
					printf("%f   ", normalizedAngle);
				}
				break;
				///////////////////여기부터는 플레이어 two의 동작
			//case '2':	//블럭을 제거하는 아이템 효과 활성화
			//	if (!m_bTriggerActive2)
			//	{
			//		if (m_pPlayer2->m_nthisDestoryItemCount > 0)
			//		{
			//			m_objects2[m_nIndexPlayerTwo]->m_bDestoryActive = true;
			//			m_objects2[m_nIndexPlayerTwo]->SetColor(RGB(0, 0, 0));
			//			m_pPlayer2->m_nthisDestoryItemCount--;
			//		}
			//	}
			//	break;
			//case '3':
			//	if (!m_bTriggerActive)
			//	{
			//		if (m_pPlayer2->m_notherDestoryItemCount > 0)
			//		{
			//			m_objects[m_nIndexPlayerOne]->m_bDestoryActive = true;
			//			m_objects[m_nIndexPlayerOne]->SetColor(RGB(0, 0, 0));
			//			m_pPlayer2->m_notherDestoryItemCount--;
			//		}
			//	}
			//	break;
			//case '1':	//랜덤하게 다음 오브젝트를 변경.
			//	if (m_pPlayer2->m_nChangeBlockCount > 0)
			//	{
			//		m_pNextGameObjectTwo = RandomMesh(m_pPlayer2);
			//		m_pPlayer2->m_nChangeBlockCount--;
			//	}
			//	break;
			//case 'A':
			//	if (!m_bTriggerActive2)
			//	{
			//		b2Vec2 velocity(-50.0f, -50.0f);
			//		m_objects2[m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
			//	}
			//	break;
			//case 'D':
			//	if (!m_bTriggerActive2)
			//	{
			//		b2Vec2 velocity(50.0f, -50.0f);
			//		m_objects2[m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
			//	}
			//	break;
			//case 'W':
			//	if (!m_bTriggerActive2)
			//	{
			//		b2Vec2 velocity(0.0f, -25.0f);
			//		m_objects2[m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
			//	}
			//	break;
			//case 'S':
			//	if (m_bTriggerActive2)
			//	{
			//		m_nIndexPlayerTwo = RunTimeBuildObject(m_nIndex2, m_pPlayer2);	//해당 인덱스에 대해 저장
			//		m_bTriggerActive2 = false;	//동작을 수행할 수 있게 만들어주고
			//		b2Vec2 velocity(0.0f, -50.0f);
			//		m_objects2[m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
			//		m_pNextGameObjectTwo = RandomMesh(m_pPlayer2);
			//		printf("%f", m_pPlayer2->GetCamera()->GetPosition().x);
			//	}
			//	else if (!m_bTriggerActive2)
			//	{
			//		b2Vec2 velocity(0.0f, -500.0f);
			//		m_objects2[m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
			//	}
			//	break;
			//case 'F':
			//	if (!m_bTriggerActive2)
			//	{
			//		::RotateBody(m_objects2[m_nIndexPlayerTwo]->m_pBody, -90.0f);
			//		float normalizedAngle = NormalizeAngle(m_objects2[m_nIndexPlayerTwo]->m_pBody->GetAngle());
			//		normalizedAngle = truncate(normalizedAngle, 2);
			//		printf("%f   ", normalizedAngle);
			//	}
			//	break;
			//case 'G':
			//	if (!m_bTriggerActive2)
			//	{
			//		::RotateBody(m_objects2[m_nIndexPlayerTwo]->m_pBody, 90.0f);
			//		float normalizedAngle = NormalizeAngle(m_objects2[m_nIndexPlayerTwo]->m_pBody->GetAngle());
			//		normalizedAngle = truncate(normalizedAngle, 2);
			//		printf("%f   ", normalizedAngle);
			//	}
			//	break;
			//default:
			//	break;
			}
		default:
			break;
		case WM_KEYUP:
			m_bKeyInput = false;  // 키 입력이 발생했을 때 true로 설정
			if (wParam == m_lastKeyPressed) {
				m_lastKeyPressed = 0;  // 마지막 키가 떼어졌다면 초기화
			}
			switch (wParam)
			{
			case VK_LEFT:
			case VK_RIGHT:
			case VK_UP:
			case VK_DOWN:
				if (!m_bTriggerActive)
				{
					b2Vec2 velocity(0.0f, -50.0f);
					m_objects[m_nIndexPlayerOne]->m_pBody->SetLinearVelocity(velocity);
				}
				break;
			case 'W':
			case 'A':
			case 'S':
			/*case 'D':
				if (!m_bTriggerActive2)
				{
					b2Vec2 velocity(0.0f, -50.0f);
					m_objects2[m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
				}
				break;*/
			default:
				break;

			}
		}
	}
}

void Scene::BuildObjects()
{
	
	// 네트워크 매니저 초기화 및 서버 연결
	//m_pNetworkManager = new NetworkManager();
	//if (m_pNetworkManager->ConnectToServer("127.0.0.1", 9000)) { // 서버 IP와 포트를 입력
	//	// 연결이 성공한 경우, 클라이언트 전송 스레드를 시작
	//	std::thread clientSendThread(&NetworkManager::Client_Send_Thread, m_pNetworkManager, m_pPlayer, this);
	//	clientSendThread.detach(); // 스레드를 분리하여 독립적으로 실행
	//}
	//else {
	//	// 연결 실패 처리
	//	MessageBox(NULL, L"서버에 연결할 수 없습니다.", L"네트워크 오류", MB_OK | MB_ICONERROR);
	//}


	float x = 0.0f; // 초기 위치 x
	float y = 40.0f; // 초기 위치 y
	float width = 10.0f; // 물체 너비
	float height = 10.0f; // 물체 높이
	m_fTime = 100.0f; // 수정해야해요
	m_iPlayercnt1 = 3;
	m_iPlayercnt2 = 3;
	cubeB* pGroundMesh = new cubeB(100.0f, 20.0f);
	m_pGround = new GameObject();
	m_pGround->SetColor(RGB(0, 255, 0));
	m_pGround->SetMesh(pGroundMesh);
	m_pGround->SetPosition(0, -200);
	x = 0.0f; // 초기 위치 x
	y = -200.0f; // 초기 위치 y
	width = 100.0f; // 물체 너비
	height = 20.0f; // 물체 높이
	m_pGround->CreatestaticPhysicsBody(m_b2World, x, y, width, height);

	m_pGround2 = new GameObject();
	m_pGround2->SetColor(RGB(0, 255, 0));
	m_pGround2->SetMesh(pGroundMesh);
	m_pGround2->SetPosition(1000, -200);
	x = 1000;
	y = -200;
	m_pGround2->CreatestaticPhysicsBody(m_b2World, x, y, width, height);

	m_pRandomMeshGet = new MeshGetInfo[m_nRandomCount];	//미리 어떤 Mesh를 사용한 모델을 사용할지 저장.
	m_pRandomMeshGet[0].color = m_dColor[0];
	m_pRandomMeshGet[0].MeshInfo = 0;
	m_pRandomMeshGet[0].SelectMesh = new TShapeMesh(20.0f, 20.0f);
	m_pRandomMeshGet[1].color = m_dColor[1];
	m_pRandomMeshGet[1].MeshInfo = 1;
	m_pRandomMeshGet[1].SelectMesh = new LRShapeMesh(20.0f, 20.0f);
	m_pRandomMeshGet[2].color = m_dColor[2];
	m_pRandomMeshGet[2].MeshInfo = 2;
	m_pRandomMeshGet[2].SelectMesh = new LLShapeMesh(20.0f, 20.0f);
	m_pRandomMeshGet[3].color = m_dColor[3];
	m_pRandomMeshGet[3].MeshInfo = 3;
	m_pRandomMeshGet[3].SelectMesh = new Cube2x2LMesh(20.0f, 20.0f);
	m_pRandomMeshGet[4].color = m_dColor[4];
	m_pRandomMeshGet[4].MeshInfo = 4;
	m_pRandomMeshGet[4].SelectMesh = new Cube2x2RMesh(20.0f, 20.0f);
	m_pRandomMeshGet[5].color = m_dColor[5];
	m_pRandomMeshGet[5].MeshInfo = 5;
	m_pRandomMeshGet[5].SelectMesh = new Cube3x1Mesh(20.0f, 20.0f);
	m_pRandomMeshGet[6].color = m_dColor[6];
	m_pRandomMeshGet[6].MeshInfo = 6;
	m_pRandomMeshGet[6].SelectMesh = new Cube4x1Mesh(20.0f, 20.0f);
	m_pRandomMeshGet[7].color = m_dColor[7];
	m_pRandomMeshGet[7].MeshInfo = 7;
	m_pRandomMeshGet[7].SelectMesh = new Cube2x2Mesh(20.0f, 20.0f);
	m_pNextGameObjectOne = RandomMesh(m_pPlayer);	//맨 처음에 콜라이더를 선언하지 않은 놈을 만들고
	m_pNextGameObjectTwo = RandomMesh(m_pPlayer2);
}

void Scene::ReleaseObjects()
{
	m_nObjectsOne = m_objects.size();
	m_nObjectsTwo = m_objects2.size();
	for (int i = m_nObjectsOne; i >= 0; i--)
	{
		RunTimeReleaseObjects(m_objects, i);
	}
	for (int i = m_nObjectsTwo; i >= 0; i--)
	{
		RunTimeReleaseObjects2(m_objects2, i);
	}
	m_nObjectsOne = 0;
	m_nIndexPlayerOne = 0;
	m_bTriggerActive = true;
	m_bTriggerActive2 = true;
}

GameObject* Scene::RandomMesh(Player* pPlayer)
{
	Point2D spawnPosition = pPlayer->GetCamera()->GetPosition();
	std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int> dist(0, 7);
	int randomIndex = dist(rng);
	GameObject* pNewObject = new GameObject();
	pNewObject->SetMesh(m_pRandomMeshGet[randomIndex].SelectMesh);
	pNewObject->SetColor(m_pRandomMeshGet[randomIndex].color);
	pNewObject->SetPosition(spawnPosition.x - 100, spawnPosition.y + 220);
	if (pPlayer == m_pPlayer)
		m_nIndex = randomIndex;
	

	return pNewObject;
}
int Scene::RunTimeBuildObject(int index, Player* pPlayer)	//키 입력 받으면 m_pNextGameObjectOne 이걸 기반으로 추가
{
	GameObject* pNewObject = NULL;
	if (pPlayer == m_pPlayer)
		pNewObject = m_pNextGameObjectOne;
	else if (pPlayer == m_pPlayer2)
		pNewObject = m_pNextGameObjectTwo;
	Point2D spawnPosition = pPlayer->GetCamera()->GetPosition();
	printf("좌표는:%f,%f이다", spawnPosition.x, spawnPosition.y);

	switch (index)	//해당하는 메쉬에 정보를 따라서 콜라이더 생성. 처음 생성 기준 가로 세로 너비 값 지정.
	{
	case 0:
		pNewObject->CreateTShapeBody(m_b2World, spawnPosition.x, spawnPosition.y + 150, 20, 20, 20);
		pNewObject->m_nWidth = 60;
		pNewObject->m_nHeihgt = 40;
		break;
	case 1:
		pNewObject->CreateLShapeBody(m_b2World, spawnPosition.x, spawnPosition.y + 150, 20, 20, 0);
		pNewObject->m_nWidth = 40;
		pNewObject->m_nHeihgt = 60;
		break;
	case 2:
		pNewObject->CreateLShapeBody(m_b2World, spawnPosition.x, spawnPosition.y + 150, 20, 20, 1);
		pNewObject->m_nWidth = 40;
		pNewObject->m_nHeihgt = 60;
		break;
	case 3:
		pNewObject->Create2x2ShapeBody(m_b2World, spawnPosition.x, spawnPosition.y + 150, 20, 20, 20, 1);
		pNewObject->m_nWidth = 60;
		pNewObject->m_nHeihgt = 40;
		break;
	case 4:
		pNewObject->Create2x2ShapeBody(m_b2World, spawnPosition.x, spawnPosition.y + 150, 20, 20, 20, 0);
		pNewObject->m_nWidth = 60;
		pNewObject->m_nHeihgt = 40;
		break;
	case 5:
		pNewObject->CreatePhysicsBody(m_b2World, spawnPosition.x, spawnPosition.y + 150, 60, 20);
		pNewObject->m_nWidth = 60;
		pNewObject->m_nHeihgt = 20;
		break;
	case 6:
		pNewObject->CreatePhysicsBody(m_b2World, spawnPosition.x, spawnPosition.y + 150, 80, 20);
		pNewObject->m_nWidth = 80;
		pNewObject->m_nHeihgt = 20;
		break;
	case 7:
		pNewObject->CreatePhysicsBody(m_b2World, spawnPosition.x, spawnPosition.y + 150, 40, 40);
		pNewObject->m_nWidth = 40;
		pNewObject->m_nHeihgt = 40;
		break;
	default:
		break;
	}
	pNewObject->m_pBody->SetGravityScale(0.0f);
	if (pPlayer == m_pPlayer)
	{
		m_objects.push_back(pNewObject);
		m_nObjectsOne = m_objects.size();
		return m_nObjectsOne - 1;
	}
	else if (pPlayer == m_pPlayer2)
	{
		printf("플레이어2가 작동했어");
		m_objects2.push_back(pNewObject);
		m_nObjectsTwo = m_objects2.size();
		return m_nObjectsTwo - 1;
	}
	return -99;
}

void Scene::RunTimeReleaseObjects(std::vector<GameObject*>& objects, int index)	//떨어지면 사라지고
{
	printf("오브젝트 하나를 삭제 했어");
	if (index >= 0 && index < objects.size()) {
		m_b2World->DestroyBody(objects[index]->m_pBody);
		delete objects[index]; // 객체 삭제
		objects.erase(objects.begin() + index); // 벡터에서 제거
		m_nIndexPlayerOne--;
		m_nObjectsOne--;
	}
}

void Scene::RunTimeReleaseObjects2(std::vector<GameObject*>& objects, int index)	//떨어지면 사라지고
{
	printf("오브젝트 하나를 삭제 했어");
	if (index >= 0 && index < objects.size()) {

		m_b2World->DestroyBody(objects[index]->m_pBody);
		delete objects[index]; // 객체 삭제
		objects.erase(objects.begin() + index); // 벡터에서 제거
		m_nIndexPlayerTwo--;
		m_nObjectsTwo--;
	}
}

Point2D Scene::CheckTopPostion(std::vector<GameObject*>& objects)	//위치를 읽어서 카메라에게 전달.
{
	Point2D topPoint(0.0f, -200.0f);	//바닥에 있는 블러과 동일한 위치에 대해 저장(바닥보다 낮은 블럭에 대해선 중요치않으니까)
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i]->m_bgravity)
		{
			if (topPoint.y <= objects[i]->m_pBody->GetPosition().y)	//만약 더 크거나 같으면
			{
				topPoint.y = objects[i]->m_pBody->GetPosition().y;
			}
		}
	}
	return topPoint;
}

void Scene::Animate(float fElapsedTime)
{
	if (!m_bGameStop && !m_bPauseGame) {
		//-------------------임시로 5초마다 떨어지게 한거 11/28
		// 타이머 업데이트
		//m_fElapsedTimeForPlayerTwo += fElapsedTime;

		//// 플레이어 2의 블록이 5초마다 떨어지도록 설정
		//if (m_fElapsedTimeForPlayerTwo >= GRAVITY_TRIGGER_TIME) {
		//	m_fElapsedTimeForPlayerTwo = 0.0f; // 타이머 리셋

		//	if (m_bTriggerActive2) { // 새로운 블록 생성
		//		m_nIndexPlayerTwo = RunTimeBuildObject(m_nIndex2, m_pPlayer2);
		//		m_bTriggerActive2 = false;
		//		b2Vec2 velocity(0.0f, -50.0f); // 기본 속도 적용
		//		m_objects2[m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
		//		m_pNextGameObjectTwo = RandomMesh(m_pPlayer2);
		//	}
		//	else { // 기존 블록 중력 활성화
		//		b2Vec2 velocity(0.0f, -500.0f); // 빠르게 떨어지도록 설정
		//		m_objects2[m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
		//	}
		//}


		for (int i = 0; i < m_nObjectsOne; i++)	//플레이어 원에 블럭들
		{
			if (m_objects[m_nIndexPlayerOne]->m_pBody->GetGravityScale() != 0.0f && !m_objects[m_nIndexPlayerOne]->m_bgravity)
			{
				m_objects[m_nIndexPlayerOne]->m_bgravity = true;	//그래비티가 활성화 됬다는소리고
				b2Vec2 velocity(0.0f, 0.0f);
				m_objects[m_nIndexPlayerOne]->m_pBody->SetLinearVelocity(velocity);	//더 이상 평행이동을 안한다.
				m_objects[m_nIndexPlayerOne]->m_pBody->SetGravityScale(1.0f);
				m_bTriggerActive = true;
				if (m_objects[m_nIndexPlayerOne]->m_bDestoryActive)	//해당 아이템의 효과가 활성화 됬을 경우.
				{
					for (int j = 0; j < m_nObjectsOne; j++)
					{
						if (contactListener.m_nApositionX <= m_objects[j]->m_pBody->GetPosition().x + 1 && contactListener.m_nApositionX >= m_objects[j]->m_pBody->GetPosition().x - 1)
						{
							if (contactListener.m_nApositionY <= m_objects[j]->m_pBody->GetPosition().y + 1 && contactListener.m_nApositionY >= m_objects[j]->m_pBody->GetPosition().y - 1)
								RunTimeReleaseObjects(m_objects, j);
						}
						else if (contactListener.m_nBpositionX <= m_objects[j]->m_pBody->GetPosition().x + 1 && contactListener.m_nBpositionX >= m_objects[j]->m_pBody->GetPosition().x - 1)
						{
							if (contactListener.m_nBpositionY <= m_objects[j]->m_pBody->GetPosition().y + 1 && contactListener.m_nBpositionY >= m_objects[j]->m_pBody->GetPosition().y - 1)
								RunTimeReleaseObjects(m_objects, j);
						}
					}
					RunTimeReleaseObjects(m_objects, m_nIndexPlayerOne);
				}
			}
			if (m_nIndexPlayerOne >= 0)
			{
				if (m_objects[m_nIndexPlayerOne]->m_fyPosition < -290)
				{
					m_bTriggerActive = true;
				}

				m_objects[i]->Animate(fElapsedTime);	//여기서 블록과 body에 위치를 동기화
				if (m_objects[i]->m_fyPosition < -300)	// 떨어지면 사라지겠금
				{
					RunTimeReleaseObjects(m_objects, i);
				}
			}
		}
		//카메라의 위치에 관련해서 정보를 전달해주는 것이다.
		m_pTopPosition1 = CheckTopPostion(m_objects);
		//printf("%f",m_pTopPosition1.y);
		if (m_pPlayer->GetCamera()->CameraTransform(m_pTopPosition1).y > 50.0f)	//일정 높이가 되면
		{
			m_pPlayer->GetCamera()->SetPosition(0.0f, m_pTopPosition1.y + 100);
		}
		else if (m_pPlayer->GetCamera()->CameraTransform(m_pTopPosition1).y <= -200.0f)
		{
			m_pPlayer->GetCamera()->SetPosition(0.0f, m_pTopPosition1.y + 200);
		}

		for (int i = 0; i < m_nObjectsTwo; i++)	//플레이어 투에 블럭들 
		{
			if (m_objects2[m_nIndexPlayerTwo]->m_pBody->GetGravityScale() != 0.0f && !m_objects2[m_nIndexPlayerTwo]->m_bgravity)
			{
				m_objects2[m_nIndexPlayerTwo]->m_bgravity = true;	//그래비티가 활성화 됬다는소리고
				b2Vec2 velocity(0.0f, 0.0f);
				m_objects2[m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);	//더 이상 평행이동을 안한다.
				m_objects2[m_nIndexPlayerTwo]->m_pBody->SetGravityScale(1.0f);
				m_bTriggerActive2 = true;
				if (m_objects2[m_nIndexPlayerTwo]->m_bDestoryActive)	//해당 아이템의 효과가 활성화 됬을 경우.
				{
					for (int j = 0; j < m_nObjectsTwo; j++)
					{
						if (contactListener.m_nApositionX <= m_objects2[j]->m_pBody->GetPosition().x + 1 && contactListener.m_nApositionX >= m_objects2[j]->m_pBody->GetPosition().x - 1)
						{
							if (contactListener.m_nApositionY <= m_objects2[j]->m_pBody->GetPosition().y + 1 && contactListener.m_nApositionY >= m_objects2[j]->m_pBody->GetPosition().y - 1)
								RunTimeReleaseObjects2(m_objects2, j);
						}
						else if (contactListener.m_nBpositionX <= m_objects2[j]->m_pBody->GetPosition().x + 1 && contactListener.m_nBpositionX >= m_objects2[j]->m_pBody->GetPosition().x - 1)
						{
							if (contactListener.m_nBpositionY <= m_objects2[j]->m_pBody->GetPosition().y + 1 && contactListener.m_nBpositionY >= m_objects2[j]->m_pBody->GetPosition().y - 1)
								RunTimeReleaseObjects2(m_objects2, j);
						}
					}
					RunTimeReleaseObjects2(m_objects2, m_nIndexPlayerTwo);
				}
			}
			if (m_nIndexPlayerTwo >= 0)
			{
				if (m_objects2[m_nIndexPlayerTwo]->m_fyPosition < -290)
				{
					m_bTriggerActive2 = true;
				}

				m_objects2[i]->Animate(fElapsedTime);	//여기서 블록과 body에 위치를 동기화
				if (m_objects2[i]->m_fyPosition < -300)	// 떨어지면 사라지겠금
				{
					RunTimeReleaseObjects2(m_objects2, i);
				}
			}
		}
		//카메라의 위치에 관련해서 정보를 전달해주는 것이다.
		m_pTopPosition2 = CheckTopPostion(m_objects2);
		if (m_pPlayer2->GetCamera()->CameraTransform(m_pTopPosition2).y > 50.0f)	//일정 높이가 되면
		{
			m_pPlayer2->GetCamera()->SetPosition(1000.0f, m_pTopPosition2.y + 100);
		}
		else if (m_pPlayer2->GetCamera()->CameraTransform(m_pTopPosition2).y <= -200.0f)
		{
			m_pPlayer2->GetCamera()->SetPosition(1000.0f, m_pTopPosition2.y + 200);
		}
	}

}

void Scene::Blend(std::vector<GameObject*>& objects, HDC hDCFrameBuffer, Player* pPlayer)
{
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;        // 블렌딩 연산을 설정합니다.
	blendFunc.BlendFlags = 0;               // 일반적으로 0으로 설정합니다.
	blendFunc.SourceConstantAlpha = 127;    // 투명도를 설정합니다. 0은 완전 투명, 255는 불투명입니다.
	blendFunc.AlphaFormat = 0;
	int IndexPlayer;

	IndexPlayer = m_nIndexPlayerOne;


	float normalizedAngle = NormalizeAngle(objects[IndexPlayer]->m_pBody->GetAngle());	//각도 읽어오기
	normalizedAngle = truncate(normalizedAngle, 2);
	if (normalizedAngle == 1.57f || normalizedAngle == 4.71f)	//세로
	{
		m_nHorizontalWidth = objects[IndexPlayer]->m_nHeihgt;
	}
	else if (normalizedAngle == 0 || normalizedAngle == 3.14f || normalizedAngle == 6.28f)//가로
	{
		m_nHorizontalWidth = objects[IndexPlayer]->m_nWidth;
	}
	Point2D objectPosition(objects[IndexPlayer]->m_fxPosition, objects[IndexPlayer]->m_fyPosition);

	Point2D position = GraphicsPipeline::ScreenTransform(pPlayer->GetCamera()->CameraTransform(objectPosition));
	HDC mDC = CreateCompatibleDC(hDCFrameBuffer);
	::SelectObject(mDC, m_bitmapBlend);

	AlphaBlend(hDCFrameBuffer, position.x - m_nHorizontalWidth / 2,
		0, m_nHorizontalWidth, FRAMEBUFFER_HEIGHT,
		mDC, 0, 0, m_nHorizontalWidth, FRAMEBUFFER_HEIGHT, blendFunc);
	::DeleteDC(mDC);
}
void Scene::Blend2(std::vector<GameObject*>& objects, HDC hDCFrameBuffer, Player* pPlayer)
{
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;        // 블렌딩 연산을 설정합니다.
	blendFunc.BlendFlags = 0;               // 일반적으로 0으로 설정합니다.
	blendFunc.SourceConstantAlpha = 127;    // 투명도를 설정합니다. 0은 완전 투명, 255는 불투명입니다.
	blendFunc.AlphaFormat = 0;
	int IndexPlayer = m_nIndexPlayerTwo;




	float normalizedAngle = NormalizeAngle(objects[IndexPlayer]->m_pBody->GetAngle());	//각도 읽어오기
	normalizedAngle = truncate(normalizedAngle, 2);
	if (normalizedAngle == 1.57f || normalizedAngle == 4.71f)	//세로
	{
		m_nHorizontalWidth2 = objects[IndexPlayer]->m_nHeihgt;
	}
	else if (normalizedAngle == 0 || normalizedAngle == 3.14f || normalizedAngle == 6.28f)//가로
	{
		m_nHorizontalWidth2 = objects[IndexPlayer]->m_nWidth;
	}
	Point2D objectPosition(objects[IndexPlayer]->m_fxPosition, objects[IndexPlayer]->m_fyPosition);
	Point2D position = GraphicsPipeline::ScreenTransform(pPlayer->GetCamera()->CameraTransform(objectPosition));
	HDC mDC = CreateCompatibleDC(hDCFrameBuffer);
	::SelectObject(mDC, m_bitmapBlend2);

	AlphaBlend(hDCFrameBuffer, position.x - m_nHorizontalWidth2 / 2,
		0, m_nHorizontalWidth2, FRAMEBUFFER_HEIGHT,
		mDC, 0, 0, m_nHorizontalWidth2, FRAMEBUFFER_HEIGHT, blendFunc);
	::DeleteDC(mDC);
}
void Scene::Render(HDC hDCFrameBuffer, Camera* pCamera, Camera* pCamera2)
{
	if (m_bGameStart == true)
	{
		if (pCamera) GraphicsPipeline::SetCamera(pCamera);	//카메라가 존재하면 그래픽스파이프라인에 카메라를 set해라.
		if (m_nObjectsOne != 0)
		{
			if (!m_bTriggerActive)
				Blend(m_objects, hDCFrameBuffer, m_pPlayer);
		}
		for (int i = 0; i < m_nObjectsOne; i++)
		{
			m_objects[i]->Render(hDCFrameBuffer, pCamera);
		}
		m_pGround->Render(hDCFrameBuffer, pCamera);
		m_pNextGameObjectOne->Render(hDCFrameBuffer, pCamera);

		if (pCamera2) GraphicsPipeline::SetCamera(pCamera2);
		if (m_nObjectsTwo != 0)
		{
			if (!m_bTriggerActive2)
				Blend2(m_objects2, hDCFrameBuffer, m_pPlayer2);
		}
		for (int i = 0; i < m_nObjectsTwo; i++)
		{
			m_objects2[i]->Render(hDCFrameBuffer, pCamera2);
		}
		m_pGround2->Render(hDCFrameBuffer, pCamera2);
		m_pNextGameObjectTwo->Render(hDCFrameBuffer, pCamera2);
		Scene::SKillUI(hDCFrameBuffer);
		if (m_bPauseGame == true)
		{
			Scene::PauseUI(hDCFrameBuffer);
		}
		char szTime[64];
		sprintf_s(szTime, "시간: %f", m_fTime);
		TextOutA(hDCFrameBuffer, 380, 0, szTime, strlen(szTime) - 4);
		if (m_bGameStop)
		{
			Scene::GameEndUI(hDCFrameBuffer);
		}
	}
	Scene::LobbyBtn(hDCFrameBuffer);

}
void Scene::LobbyBtn(HDC hDCFrameBuffer)
{

	if (m_bGameStart == false)
	{
		HFONT hFont = CreateFont(30, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Consolas"));
		static double left = m_DbtnLeft + (FRAMEBUFFER_WIDTH / 4);
		static double top = m_DbtnTop + (FRAMEBUFFER_HEIGHT / 10) * 2;
		static double right = m_DbtnRight - (FRAMEBUFFER_WIDTH / 4);
		static double bottom = m_DbtnBottom + (FRAMEBUFFER_HEIGHT / 10) * 2;

		SelectObject(hDCFrameBuffer, hFont);

		TextOut(hDCFrameBuffer, (m_DbtnLeft + m_DbtnRight) / 2.5, (m_DbtnTop + m_DbtnBottom) / 6, L"Tricky Tower", strlen("Tricky Tower"));

		Rectangle(hDCFrameBuffer, m_DbtnLeft, m_DbtnTop, m_DbtnRight, m_DbtnBottom); //GameStart
		TextOut(hDCFrameBuffer, (m_DbtnLeft + m_DbtnRight) / 2.5, (m_DbtnTop + m_DbtnBottom) / 2.15, L"GameStart", strlen("GameStart"));

		Rectangle(hDCFrameBuffer, left, top, right, bottom); //Exit
		TextOut(hDCFrameBuffer, (m_DbtnLeft + m_DbtnRight) / 2.2, (m_DbtnTop + m_DbtnBottom) / 1.45, L"Exit", strlen("Exit"));


		if (m_MouseX > m_DbtnLeft && m_MouseX< m_DbtnRight && m_MouseY > m_DbtnTop && m_MouseY < m_DbtnBottom)
		{
			RECT rect = { m_DbtnLeft, m_DbtnTop, m_DbtnRight, m_DbtnBottom };
			InvertRect(hDCFrameBuffer, &rect);
		}
		if (m_MouseX < left && m_MouseX > right && m_MouseY > top && m_MouseY < bottom)
		{
			RECT rect = { left , top, right , bottom };
			InvertRect(hDCFrameBuffer, &rect);
		}
		DeleteObject(hFont);
	}
}

void Scene::SKillUI(HDC hDCFrameBuffer)
{
	HBRUSH hBrush;
	HPEN hPen;

	//나눴는데 이제 이 값을 어떻게 할 건지에 대해 생각 해야함.
	Rectangle(hDCFrameBuffer, m_DbtnLeft / 0.36, 0, FRAMEBUFFER_WIDTH - 10, m_DbtnBottom / 5);

	hBrush = CreateSolidBrush(RGB(128, 128, 128));
	//플레이어 1
	SelectObject(hDCFrameBuffer, hBrush);
	Rectangle(hDCFrameBuffer, 140, 20, 180, 60);
	Rectangle(hDCFrameBuffer, 180, 20, 220, 60);
	Rectangle(hDCFrameBuffer, 220, 20, 260, 60);
	char szTime[64];
	sprintf_s(szTime, "%d", m_pPlayer->m_nChangeBlockCount);
	TextOutA(hDCFrameBuffer, 150, 20, szTime, strlen(szTime));
	sprintf_s(szTime, "%d", m_pPlayer->m_nthisDestoryItemCount);
	TextOutA(hDCFrameBuffer, 190, 20, szTime, strlen(szTime));
	sprintf_s(szTime, "%d", m_pPlayer->m_notherDestoryItemCount);
	TextOutA(hDCFrameBuffer, 230, 20, szTime, strlen(szTime));

	//플레이어2
	Rectangle(hDCFrameBuffer, 560, 20, 600, 60);
	Rectangle(hDCFrameBuffer, 600, 20, 640, 60);
	Rectangle(hDCFrameBuffer, 640, 20, 680, 60);

	sprintf_s(szTime, "%d", m_pPlayer2->m_nChangeBlockCount);
	TextOutA(hDCFrameBuffer, 570, 20, szTime, strlen(szTime));
	sprintf_s(szTime, "%d", m_pPlayer2->m_nthisDestoryItemCount);
	TextOutA(hDCFrameBuffer, 610, 20, szTime, strlen(szTime));
	sprintf_s(szTime, "%d", m_pPlayer2->m_notherDestoryItemCount);
	TextOutA(hDCFrameBuffer, 650, 20, szTime, strlen(szTime));
	DeleteObject(hBrush);

}

void Scene::PauseUI(HDC hDCFrameBuffer)
{
	HBRUSH hBrush;
	HPEN hPen;

	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	SelectObject(hDCFrameBuffer, hBrush);
	Rectangle(hDCFrameBuffer, 100, 100, 600, 400);
	Rectangle(hDCFrameBuffer, 550, 110, 590, 140);
	TextOut(hDCFrameBuffer, 565, 110, L"X", strlen("X"));


	Rectangle(hDCFrameBuffer, 300, 200, 420, 250);
	TextOut(hDCFrameBuffer, 330, 210, L"Lobby", strlen("Lobby"));

	Rectangle(hDCFrameBuffer, 300, 300, 420, 350);
	TextOut(hDCFrameBuffer, 330, 310, L"Exit", strlen("Exit"));
	DeleteObject(hBrush);

	if (m_MouseX > 300 && m_MouseX < 420 && m_MouseY > 200 && m_MouseY < 250)
	{
		RECT rect = { 300, 200, 420, 250 };
		InvertRect(hDCFrameBuffer, &rect);
	}
	else if (m_MouseX > 300 && m_MouseX < 420 && m_MouseY > 300 && m_MouseY < 350)
	{
		RECT rect = { 300, 300, 420, 350 };
		InvertRect(hDCFrameBuffer, &rect);
	}
	else if (m_MouseX > 550 && m_MouseX < 590 && m_MouseY > 110 && m_MouseY < 140)
	{
		RECT rect = { 550, 110, 590, 140 };
		InvertRect(hDCFrameBuffer, &rect);
	}

}

void Scene::timeUI(float fElapsedTime)
{

	if (m_fTime > 0 && m_bGameStart)
	{
		if(!m_bPauseGame)
			m_fTime -= fElapsedTime;
		m_bGameStop = false;
	}
	else
	{
		m_bGameStop = true;
	}
}

void Scene::GameEndUI(HDC hDCFrameBuffer)
{
	if (m_bGameStop)
	{
		Rectangle(hDCFrameBuffer, 50, 50, 750, 550);
		Rectangle(hDCFrameBuffer, 700, 55, 730, 90);
		TextOut(hDCFrameBuffer, 710, 55, L"X", strlen("X"));
		Rectangle(hDCFrameBuffer, 265, 450, 485, 500);
		TextOut(hDCFrameBuffer, 300, 450, L"Return to Lobby", strlen("Return to Lobby"));
		if (m_pTopPosition1.y > m_pTopPosition2.y)
		{
			TextOut(hDCFrameBuffer, 350, 250, L"Player 1 Win!", strlen("Player 1 Win!"));
		}
		else if (m_pTopPosition1.y < m_pTopPosition2.y)
		{
			TextOut(hDCFrameBuffer, 350, 250, L"Player 2 Win!", strlen("Player 2 Win!"));
		}
		else
		{
			TextOut(hDCFrameBuffer, 350, 250, L"Draw", strlen("Draw"));
		}
		if (m_MouseX > 700 && m_MouseX < 730 && m_MouseY > 55 && m_MouseY < 90)
		{
			RECT rect = { 700, 55, 730, 90 };
			InvertRect(hDCFrameBuffer, &rect);
		}
		else if (m_MouseX > 265 && m_MouseX < 485 && m_MouseY > 450 && m_MouseY < 500)
		{
			RECT rect = { 265, 450, 485, 500 };
			InvertRect(hDCFrameBuffer, &rect);
		}
	}
}
