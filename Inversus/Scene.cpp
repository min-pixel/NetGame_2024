#include "MainH.h"
#include "Scene.h"
#include "GraphicsPipeline.h"
#include <thread>


Scene::Scene(Player* pPlayer, Player* pPlayer2, b2World* world, HINSTANCE Instance) : m_lastKeyPressed(0), m_fElapsedTimeForPlayerTwo(0.0f)  // �ʱ�ȭ ����Ʈ���� m_lastKeyPressed�� 0���� �ʱ�ȭ
{
	m_pPlayer = pPlayer;
	m_pPlayer2 = pPlayer2;
	m_b2World = world;
	m_b2World->SetContactListener(&contactListener);	//�浹 ���� �����ؼ� ���� �־��ش�.
	m_hInstance = Instance;

	m_bitmapBlend = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP1));
	m_bitmapBlend2 = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP1));
}

Scene::~Scene()
{
}


void Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	//���� �޾ƿ��� ����
	m_MouseX = LOWORD(lParam);
	m_MouseY = HIWORD(lParam);
	//��ưUI ����
	if (nMessageID == WM_LBUTTONDOWN) {
		// start ��ư�� Ŭ�� ������ ����
		if (m_MouseX > m_DbtnLeft && m_MouseX< m_DbtnRight && m_MouseY > m_DbtnTop && m_MouseY < m_DbtnBottom)
		{
			m_pPlayer->m_nthisDestoryItemCount = 3;
			m_pPlayer->m_notherDestoryItemCount = 3;
			m_pPlayer->m_nChangeBlockCount = 5;
			m_pPlayer2->m_nthisDestoryItemCount = 3;
			m_pPlayer2->m_notherDestoryItemCount = 3;
			m_pPlayer2->m_nChangeBlockCount = 5;

			// ���� ������ ������ �˸��� ��Ŷ ����
			/*if (m_pNetworkManager) {
				m_pNetworkManager->SendAcceptServerPacket();
			}*/

			//ReleaseObjects();	//������Ʈ�� �����ϰ�
			//BuildObjects();	//������Ʈ�� �����ϰ�

			//m_bGameStart = false;
			//m_bGameStop = false; //11�� 19�� �߰� : ����� �� �ϸ� ���� �� �ϴ� �� �ذ�.
			m_pNetworkManager = new NetworkManager();
			if (m_pNetworkManager->ConnectToServer("127.0.0.1", 9000)) { // ���� IP�� ��Ʈ�� �Է�
				// ������ ������ ���, Ŭ���̾�Ʈ ���� �����带 ����

				// ������ �����ϸ� ������ ���� ��ȣ�� ���
				bool startSignal = false;
				while (!startSignal) {
					m_pNetworkManager->ReceiveStartSignal(startSignal);  // ���� ��ȣ�� �޴� �Լ��� �߰�
				}

				// ���� ��ȣ�� ���� �� ���� ����
				ReleaseObjects();	//������Ʈ�� �����ϰ�
				BuildObjects();	//������Ʈ�� �����ϰ�



				m_bGameStart = true;
				m_bGameStop = false; //11�� 19�� �߰� : ����� �� �ϸ� ���� �� �ϴ� �� �ذ�.

				std::thread clientSendThread(&NetworkManager::Client_Send_Thread, m_pNetworkManager, m_pPlayer, this);
				clientSendThread.detach(); // �����带 �и��Ͽ� ���������� ����

				std::thread receiveThread(&NetworkManager::ReceiveThread, m_pNetworkManager, m_pPlayer, this); 
				receiveThread.detach();

				

				

			}
			else {
				// ���� ���� ó��
				MessageBox(NULL, L"������ ������ �� �����ϴ�.", L"��Ʈ��ũ ����", MB_OK | MB_ICONERROR);
			}



		}// exit ��ư�� Ŭ�� ������ ����
		else if (m_MouseX < m_DbtnLeft + (FRAMEBUFFER_WIDTH / 4) && m_MouseX> m_DbtnRight - (FRAMEBUFFER_WIDTH / 4) && m_MouseY > (m_DbtnTop + (FRAMEBUFFER_HEIGHT / 10) * 2) && m_MouseY < m_DbtnBottom + (FRAMEBUFFER_HEIGHT / 10) * 2)
		{
			PostQuitMessage(0);
		}
		else if (m_MouseX > m_DbtnLeft / 0.36 && m_MouseY > 0 && m_MouseX < FRAMEBUFFER_WIDTH - 10 && m_MouseY < m_DbtnBottom / 5)
		{
			m_bPauseGame = true;
		}
		else if (m_MouseX > 550 && m_MouseX < 590 && m_MouseY > 110 && m_MouseY < 140)// XŰ ������ ��
		{
			m_bhideMenu = true;
		}
		if (m_bPauseGame)
		{
			if (m_MouseX > 300 && m_MouseX < 420 && m_MouseY > 200 && m_MouseY < 250) //pause���� ���ι�ư ����
			{
				m_bmainbtn = true;
				m_bGameStart = false;
				m_bPauseGame = false;
			}
			else if (m_MouseX > 300 && m_MouseX < 420 && m_MouseY > 300 && m_MouseY < 350) // pause���� ���� ������
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
				m_fTime = 100.0f;//���� �ؾ���
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
	// ���� �ٵ��� ������ �����ɴϴ�.
	float currentAngle = body->GetAngle();
	// ���ο� ������ ����մϴ�.
	float newAngle = currentAngle + b2_pi * angle / 180.0f; // angle�� �������� ��ȯ�Ͽ� ���մϴ�.
	// �ٵ��� ������ �����մϴ�.
	body->SetTransform(body->GetPosition(), newAngle);
}
float NormalizeAngle(float angleRad) {	//������ ������� ������ ���������ش�.
	while (angleRad < 0) {
		angleRad += 2 * b2_pi;
	}
	while (angleRad >= 2 * b2_pi) {
		angleRad -= 2 * b2_pi;
	}
	return angleRad;
}
float truncate(float value, int decimal_places) {	//�Ҽ��� ������ �Լ�
	float factor = pow(10.0f, decimal_places);
	return trunc(value * factor) / factor;
}
void Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!m_bPauseGame)	//������ Puse���¸�
	{
		switch (nMessageID)
		{
		case WM_KEYDOWN:
			m_bKeyInput = true;  // Ű �Է��� �߻����� �� true�� ����
			m_lastKeyPressed = wParam;  // ���������� ���� Ű�� ������Ʈ
			switch (wParam)
			{
				//////�÷��̾� ���� ����.
			//case 'K':	//���� �����ϴ� ������ ȿ�� Ȱ��ȭ
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
			//case 'L':	//���� �� �α���
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
			//case 'J':	//�����ϰ� ���� ������Ʈ�� ����.
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
					m_nIndexPlayerOne = RunTimeBuildObject(m_nIndex, m_pPlayer);	//�ش� �ε����� ���� ����
					m_bTriggerActive = false;	//������ ������ �� �ְ� ������ְ�
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
				///////////////////������ʹ� �÷��̾� two�� ����
			//case '2':	//���� �����ϴ� ������ ȿ�� Ȱ��ȭ
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
			//case '1':	//�����ϰ� ���� ������Ʈ�� ����.
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
			//		m_nIndexPlayerTwo = RunTimeBuildObject(m_nIndex2, m_pPlayer2);	//�ش� �ε����� ���� ����
			//		m_bTriggerActive2 = false;	//������ ������ �� �ְ� ������ְ�
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
			m_bKeyInput = false;  // Ű �Է��� �߻����� �� true�� ����
			if (wParam == m_lastKeyPressed) {
				m_lastKeyPressed = 0;  // ������ Ű�� �������ٸ� �ʱ�ȭ
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
	
	// ��Ʈ��ũ �Ŵ��� �ʱ�ȭ �� ���� ����
	//m_pNetworkManager = new NetworkManager();
	//if (m_pNetworkManager->ConnectToServer("127.0.0.1", 9000)) { // ���� IP�� ��Ʈ�� �Է�
	//	// ������ ������ ���, Ŭ���̾�Ʈ ���� �����带 ����
	//	std::thread clientSendThread(&NetworkManager::Client_Send_Thread, m_pNetworkManager, m_pPlayer, this);
	//	clientSendThread.detach(); // �����带 �и��Ͽ� ���������� ����
	//}
	//else {
	//	// ���� ���� ó��
	//	MessageBox(NULL, L"������ ������ �� �����ϴ�.", L"��Ʈ��ũ ����", MB_OK | MB_ICONERROR);
	//}


	float x = 0.0f; // �ʱ� ��ġ x
	float y = 40.0f; // �ʱ� ��ġ y
	float width = 10.0f; // ��ü �ʺ�
	float height = 10.0f; // ��ü ����
	m_fTime = 100.0f; // �����ؾ��ؿ�
	m_iPlayercnt1 = 3;
	m_iPlayercnt2 = 3;
	cubeB* pGroundMesh = new cubeB(100.0f, 20.0f);
	m_pGround = new GameObject();
	m_pGround->SetColor(RGB(0, 255, 0));
	m_pGround->SetMesh(pGroundMesh);
	m_pGround->SetPosition(0, -200);
	x = 0.0f; // �ʱ� ��ġ x
	y = -200.0f; // �ʱ� ��ġ y
	width = 100.0f; // ��ü �ʺ�
	height = 20.0f; // ��ü ����
	m_pGround->CreatestaticPhysicsBody(m_b2World, x, y, width, height);

	m_pGround2 = new GameObject();
	m_pGround2->SetColor(RGB(0, 255, 0));
	m_pGround2->SetMesh(pGroundMesh);
	m_pGround2->SetPosition(1000, -200);
	x = 1000;
	y = -200;
	m_pGround2->CreatestaticPhysicsBody(m_b2World, x, y, width, height);

	m_pRandomMeshGet = new MeshGetInfo[m_nRandomCount];	//�̸� � Mesh�� ����� ���� ������� ����.
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
	m_pNextGameObjectOne = RandomMesh(m_pPlayer);	//�� ó���� �ݶ��̴��� �������� ���� ���� �����
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
int Scene::RunTimeBuildObject(int index, Player* pPlayer)	//Ű �Է� ������ m_pNextGameObjectOne �̰� ������� �߰�
{
	GameObject* pNewObject = NULL;
	if (pPlayer == m_pPlayer)
		pNewObject = m_pNextGameObjectOne;
	else if (pPlayer == m_pPlayer2)
		pNewObject = m_pNextGameObjectTwo;
	Point2D spawnPosition = pPlayer->GetCamera()->GetPosition();
	printf("��ǥ��:%f,%f�̴�", spawnPosition.x, spawnPosition.y);

	switch (index)	//�ش��ϴ� �޽��� ������ ���� �ݶ��̴� ����. ó�� ���� ���� ���� ���� �ʺ� �� ����.
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
		printf("�÷��̾�2�� �۵��߾�");
		m_objects2.push_back(pNewObject);
		m_nObjectsTwo = m_objects2.size();
		return m_nObjectsTwo - 1;
	}
	return -99;
}

void Scene::RunTimeReleaseObjects(std::vector<GameObject*>& objects, int index)	//�������� �������
{
	printf("������Ʈ �ϳ��� ���� �߾�");
	if (index >= 0 && index < objects.size()) {
		m_b2World->DestroyBody(objects[index]->m_pBody);
		delete objects[index]; // ��ü ����
		objects.erase(objects.begin() + index); // ���Ϳ��� ����
		m_nIndexPlayerOne--;
		m_nObjectsOne--;
	}
}

void Scene::RunTimeReleaseObjects2(std::vector<GameObject*>& objects, int index)	//�������� �������
{
	printf("������Ʈ �ϳ��� ���� �߾�");
	if (index >= 0 && index < objects.size()) {

		m_b2World->DestroyBody(objects[index]->m_pBody);
		delete objects[index]; // ��ü ����
		objects.erase(objects.begin() + index); // ���Ϳ��� ����
		m_nIndexPlayerTwo--;
		m_nObjectsTwo--;
	}
}

Point2D Scene::CheckTopPostion(std::vector<GameObject*>& objects)	//��ġ�� �о ī�޶󿡰� ����.
{
	Point2D topPoint(0.0f, -200.0f);	//�ٴڿ� �ִ� ���� ������ ��ġ�� ���� ����(�ٴں��� ���� ���� ���ؼ� �߿�ġ�����ϱ�)
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i]->m_bgravity)
		{
			if (topPoint.y <= objects[i]->m_pBody->GetPosition().y)	//���� �� ũ�ų� ������
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
		//-------------------�ӽ÷� 5�ʸ��� �������� �Ѱ� 11/28
		// Ÿ�̸� ������Ʈ
		//m_fElapsedTimeForPlayerTwo += fElapsedTime;

		//// �÷��̾� 2�� ����� 5�ʸ��� ���������� ����
		//if (m_fElapsedTimeForPlayerTwo >= GRAVITY_TRIGGER_TIME) {
		//	m_fElapsedTimeForPlayerTwo = 0.0f; // Ÿ�̸� ����

		//	if (m_bTriggerActive2) { // ���ο� ��� ����
		//		m_nIndexPlayerTwo = RunTimeBuildObject(m_nIndex2, m_pPlayer2);
		//		m_bTriggerActive2 = false;
		//		b2Vec2 velocity(0.0f, -50.0f); // �⺻ �ӵ� ����
		//		m_objects2[m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
		//		m_pNextGameObjectTwo = RandomMesh(m_pPlayer2);
		//	}
		//	else { // ���� ��� �߷� Ȱ��ȭ
		//		b2Vec2 velocity(0.0f, -500.0f); // ������ ���������� ����
		//		m_objects2[m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
		//	}
		//}


		for (int i = 0; i < m_nObjectsOne; i++)	//�÷��̾� ���� ����
		{
			if (m_objects[m_nIndexPlayerOne]->m_pBody->GetGravityScale() != 0.0f && !m_objects[m_nIndexPlayerOne]->m_bgravity)
			{
				m_objects[m_nIndexPlayerOne]->m_bgravity = true;	//�׷���Ƽ�� Ȱ��ȭ ��ٴ¼Ҹ���
				b2Vec2 velocity(0.0f, 0.0f);
				m_objects[m_nIndexPlayerOne]->m_pBody->SetLinearVelocity(velocity);	//�� �̻� �����̵��� ���Ѵ�.
				m_objects[m_nIndexPlayerOne]->m_pBody->SetGravityScale(1.0f);
				m_bTriggerActive = true;
				if (m_objects[m_nIndexPlayerOne]->m_bDestoryActive)	//�ش� �������� ȿ���� Ȱ��ȭ ���� ���.
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

				m_objects[i]->Animate(fElapsedTime);	//���⼭ ��ϰ� body�� ��ġ�� ����ȭ
				if (m_objects[i]->m_fyPosition < -300)	// �������� ������ڱ�
				{
					RunTimeReleaseObjects(m_objects, i);
				}
			}
		}
		//ī�޶��� ��ġ�� �����ؼ� ������ �������ִ� ���̴�.
		m_pTopPosition1 = CheckTopPostion(m_objects);
		//printf("%f",m_pTopPosition1.y);
		if (m_pPlayer->GetCamera()->CameraTransform(m_pTopPosition1).y > 50.0f)	//���� ���̰� �Ǹ�
		{
			m_pPlayer->GetCamera()->SetPosition(0.0f, m_pTopPosition1.y + 100);
		}
		else if (m_pPlayer->GetCamera()->CameraTransform(m_pTopPosition1).y <= -200.0f)
		{
			m_pPlayer->GetCamera()->SetPosition(0.0f, m_pTopPosition1.y + 200);
		}

		for (int i = 0; i < m_nObjectsTwo; i++)	//�÷��̾� ���� ���� 
		{
			if (m_objects2[m_nIndexPlayerTwo]->m_pBody->GetGravityScale() != 0.0f && !m_objects2[m_nIndexPlayerTwo]->m_bgravity)
			{
				m_objects2[m_nIndexPlayerTwo]->m_bgravity = true;	//�׷���Ƽ�� Ȱ��ȭ ��ٴ¼Ҹ���
				b2Vec2 velocity(0.0f, 0.0f);
				m_objects2[m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);	//�� �̻� �����̵��� ���Ѵ�.
				m_objects2[m_nIndexPlayerTwo]->m_pBody->SetGravityScale(1.0f);
				m_bTriggerActive2 = true;
				if (m_objects2[m_nIndexPlayerTwo]->m_bDestoryActive)	//�ش� �������� ȿ���� Ȱ��ȭ ���� ���.
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

				m_objects2[i]->Animate(fElapsedTime);	//���⼭ ��ϰ� body�� ��ġ�� ����ȭ
				if (m_objects2[i]->m_fyPosition < -300)	// �������� ������ڱ�
				{
					RunTimeReleaseObjects2(m_objects2, i);
				}
			}
		}
		//ī�޶��� ��ġ�� �����ؼ� ������ �������ִ� ���̴�.
		m_pTopPosition2 = CheckTopPostion(m_objects2);
		if (m_pPlayer2->GetCamera()->CameraTransform(m_pTopPosition2).y > 50.0f)	//���� ���̰� �Ǹ�
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
	blendFunc.BlendOp = AC_SRC_OVER;        // ���� ������ �����մϴ�.
	blendFunc.BlendFlags = 0;               // �Ϲ������� 0���� �����մϴ�.
	blendFunc.SourceConstantAlpha = 127;    // ������ �����մϴ�. 0�� ���� ����, 255�� �������Դϴ�.
	blendFunc.AlphaFormat = 0;
	int IndexPlayer;

	IndexPlayer = m_nIndexPlayerOne;


	float normalizedAngle = NormalizeAngle(objects[IndexPlayer]->m_pBody->GetAngle());	//���� �о����
	normalizedAngle = truncate(normalizedAngle, 2);
	if (normalizedAngle == 1.57f || normalizedAngle == 4.71f)	//����
	{
		m_nHorizontalWidth = objects[IndexPlayer]->m_nHeihgt;
	}
	else if (normalizedAngle == 0 || normalizedAngle == 3.14f || normalizedAngle == 6.28f)//����
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
	blendFunc.BlendOp = AC_SRC_OVER;        // ���� ������ �����մϴ�.
	blendFunc.BlendFlags = 0;               // �Ϲ������� 0���� �����մϴ�.
	blendFunc.SourceConstantAlpha = 127;    // ������ �����մϴ�. 0�� ���� ����, 255�� �������Դϴ�.
	blendFunc.AlphaFormat = 0;
	int IndexPlayer = m_nIndexPlayerTwo;




	float normalizedAngle = NormalizeAngle(objects[IndexPlayer]->m_pBody->GetAngle());	//���� �о����
	normalizedAngle = truncate(normalizedAngle, 2);
	if (normalizedAngle == 1.57f || normalizedAngle == 4.71f)	//����
	{
		m_nHorizontalWidth2 = objects[IndexPlayer]->m_nHeihgt;
	}
	else if (normalizedAngle == 0 || normalizedAngle == 3.14f || normalizedAngle == 6.28f)//����
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
		if (pCamera) GraphicsPipeline::SetCamera(pCamera);	//ī�޶� �����ϸ� �׷��Ƚ����������ο� ī�޶� set�ض�.
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
		sprintf_s(szTime, "�ð�: %f", m_fTime);
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

	//�����µ� ���� �� ���� ��� �� ������ ���� ���� �ؾ���.
	Rectangle(hDCFrameBuffer, m_DbtnLeft / 0.36, 0, FRAMEBUFFER_WIDTH - 10, m_DbtnBottom / 5);

	hBrush = CreateSolidBrush(RGB(128, 128, 128));
	//�÷��̾� 1
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

	//�÷��̾�2
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
