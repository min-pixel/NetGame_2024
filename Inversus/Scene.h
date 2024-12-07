#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "NetworkManager.h"
#include <mutex>
#include <functional> // std::function ����� ���� �ʿ�
#include <vector>     // std::vector ����� ���� �ʿ�



class MyContactListener : public b2ContactListener {
public:
	float m_nApositionX = 0.0f;
	float m_nApositionY = 0.0f;
	float m_nBpositionX = 0.0f;
	float m_nBpositionY = 0.0f;
	void BeginContact(b2Contact* contact) override {
		b2Body* bodyA = contact->GetFixtureA()->GetBody();
		b2Body* bodyB = contact->GetFixtureB()->GetBody();

		bodyA->SetGravityScale(1.0f);
		m_nApositionX = bodyA->GetPosition().x;
		m_nApositionY = bodyA->GetPosition().y;
		bodyB->SetGravityScale(1.0f);
		m_nBpositionX = bodyB->GetPosition().x;
		m_nBpositionY = bodyB->GetPosition().y;
	}
};

struct  MeshGetInfo
{
	Mesh* SelectMesh;
	int MeshInfo = 0; //0~7���� �޽��� ������ �з��Ҳ���.
	DWORD color; //���� ����.
};

class Scene
{
public:
	Scene(Player* pPlayer, Player* pPlayer2, b2World* world, HINSTANCE Instance);
	virtual ~Scene();
	MyContactListener contactListener;

private: //���� ������Ʈ�� �迭�� �����Ǿ��ִ�.
	HINSTANCE					m_hInstance = NULL;
	int							m_nObjectsOne = 0;		//���� 1�� �÷��̾ ���� �� ���� ������Ʈ�� ���´�.
	//int							m_nObjectsTwo = 0;		//���� 2�� �÷��̾ ���� �� ����
	//GameObject** m_ppObjects = NULL;		//���� ������.
	//MyContactListener m_contactListener;// �浹 �̺�Ʈ ó���� ���� ��ü
	GameObject* m_pGround = NULL;
	GameObject* m_pGround2 = NULL;

	Player* m_pPlayer = NULL;
	//Player* m_pPlayer2 = NULL;

	b2World* m_b2World = NULL;

	//int m_nIndexPlayerOne = 0;		//���� 1�� �÷��̾��� ������Ʈ���� �� ��° �ε����� �÷��̾��� �Է� ��� ������
	//std::vector<GameObject*> m_objects;

	//int m_nIndexPlayerTwo = 0;
	//std::vector<GameObject*> m_objects2;

	//�����ϰ� �����Ǵ� �� ���� �κ�.
	//GameObject* m_pNextGameObjectOne = NULL; //������µ� ���� �̸� �����ִ� ��. �÷��̾� 1
	//GameObject* m_pNextGameObjectTwo = NULL; //������µ� ���� �̸� �����ִ� ��.  �÷��̾� 2

	//Point2D m_pTopPosition1;	//�÷��̾� 1�� ���� �� ���� ������ ����.
	Point2D m_pTopPosition2;



	//MeshGetInfo* m_pRandomMeshGet = NULL;	//8���� �޽� ������ �����ִ�.
	//int m_nRandomCount = 8;	//�� �޽��� �� 8���� �ǹ�.
	//int m_nIndex = 0; //���� ���õ� �޽��� ������ �Ǵ����� index�� 0~7
	//int m_nIndex2 = 0;
	DWORD m_dColor[8] = { RGB(125,0,0),RGB(200,0,0), RGB(120,120,0), RGB(120,200,0), RGB(120,0,102), RGB(200,0,200),
	RGB(0,125,125), RGB(0,200,125) };	//���� ����

	HBITMAP m_bitmapBlend = NULL;
	HBITMAP m_bitmapBlend2 = NULL;
	int m_nHorizontalWidth = 0;	//�ٴ��� �ٶ󺸰� �ִ� �ʺ� ����.
	int m_nHorizontalWidth2 = 0;

	int m_MouseX; //���콺X��
	int m_MouseY; //���콺Y��
	int m_iPlayercnt1;
	int m_iPlayercnt2;
	float m_fTime = 0;
	//bool m_bKeyInput = false; //Ű�Է� üũ
	//bool m_bGameStop = false;
	bool m_bPauseGame = false; // ���� ���߱�
	bool m_bmainbtn = false;// ���� ȭ������ ����
	bool m_bhideMenu = false; // pause �޴� �����
	bool m_bGameStart = false; // GameStart�� ������ �� üũ�ϴ� �뵵
	bool m_bExitBtn = false; // Exit ��ư�� ������ �� üũ�ϴ� �뵵


	double m_DbtnLeft = FRAMEBUFFER_WIDTH / 3;// ��ǥ�� �뵵 �Դϴ�.
	double m_DbtnTop = FRAMEBUFFER_HEIGHT / 10 * 4;// ��ǥ�� �뵵 �Դϴ�.
	double m_DbtnRight = m_DbtnLeft + FRAMEBUFFER_WIDTH / 3;// ��ǥ�� �뵵 �Դϴ�.
	double m_DbtnBottom = m_DbtnTop + FRAMEBUFFER_HEIGHT / 10;// ��ǥ�� �뵵 �Դϴ�.

	NetworkManager* m_pNetworkManager = nullptr; //��Ʈ��ũ �Ŵ��� �߰�
	

public:

	bool		m_bTriggerActive = true; // �̰��� true�� �Ǹ� ���� �ݶ��̴��� �߷��� ������ �޵��� �ٲ��ش�.
	bool		m_bTriggerActive2 = true; // �̰��� true�� �Ǹ� ���� �ݶ��̴��� �߷��� ������ �޵��� �ٲ��ش�.

	Player* m_pPlayer2 = NULL;
	bool m_bKeyInput = false; //Ű�Է� üũ
	bool m_bGameStop = false;
	std::vector<GameObject*> m_objects;
	GameObject* m_pNextGameObjectOne = NULL; //������µ� ���� �̸� �����ִ� ��. �÷��̾� 1
	GameObject* m_pNextGameObjectTwo = NULL; //������µ� ���� �̸� �����ִ� ��.  �÷��̾� 2
	int m_nIndex = 0; //���� ���õ� �޽��� ������ �Ǵ����� index�� 0~7
	int m_nIndex2 = 0;
	int m_nIndexPlayerOne = 0; 
	int m_nIndexPlayerTwo = 0; 
	int m_nRandomCount = 8;
	MeshGetInfo* m_pRandomMeshGet = NULL;
	std::vector<GameObject*> m_objects2;
	float m_fElapsedTimeForPlayerTwo; // �÷��̾� 2�� �߷� Ÿ�̸� 
	const float GRAVITY_TRIGGER_TIME = 5.0f; // 5�ʸ��� �߷� �ߵ� 
	Point2D m_pTopPosition1;	//�÷��̾� 1�� ���� �� ���� ������ ����
	WPARAM m_lastKeyPressed; 
	int							m_nObjectsTwo = 0;
	int m_playerCount; // �������� ���� �÷��̾� ī��Ʈ �� ����  12/07
	
	
	void LogDebugOutput(const std::string& message);
	

	

public:
	virtual void BuildObjects();	//������Ʈ ����
	virtual void ReleaseObjects();	//������Ʈ ����
	virtual GameObject* RandomMesh(Player* pPlayer);	//�� ģ���� ���� ������ ���� ������Ʈ�� �Ѱ��ٰž�.
	virtual int RunTimeBuildObject(int index, Player* pPlayer);	//���� �� ������Ʈ�� �������� ģ��.
	virtual void RunTimeReleaseObjects(std::vector<GameObject*>& objects, int index);
	virtual void RunTimeReleaseObjects2(std::vector<GameObject*>& objects, int index);
	virtual Point2D CheckTopPostion(std::vector<GameObject*>& objects);	//���� ���� ���� ��ġ�� �Ǵ�.
	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, Camera* pCamera, Camera* pCamera2);	//���� ������Ʈ�� �׸����� ī�޶� �ʿ��ؼ� ī�޶� �޾ƿ´�.
	virtual void Blend(std::vector<GameObject*>& objects, HDC hDCFrameBuffer, Player* pPlayer);
	virtual void Blend2(std::vector<GameObject*>& objects, HDC hDCFrameBuffer, Player* pPlayer);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void LobbyBtn(HDC hDCFrameBuffer); // �κ�ȭ�� UI
	virtual void SKillUI(HDC hDCFrameBuffer); // ������ �� UI
	virtual void PauseUI(HDC hDCFrameBuffer); // Pause �� �� UI
	virtual void timeUI(float fElapsedTime);// �ð� UI
	virtual void GameEndUI(HDC hDCFrameBuffer); //game���� �� UI

	


};

