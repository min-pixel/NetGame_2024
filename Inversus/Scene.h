#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "NetworkManager.h"
#include <mutex>
#include <functional> // std::function 사용을 위해 필요
#include <vector>     // std::vector 사용을 위해 필요



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
	int MeshInfo = 0; //0~7까지 메쉬에 유형을 분류할꺼다.
	DWORD color; //색상 정보.
};

class Scene
{
public:
	Scene(Player* pPlayer, Player* pPlayer2, b2World* world, HINSTANCE Instance);
	virtual ~Scene();
	MyContactListener contactListener;

private: //게임 오브젝트의 배열로 구성되어있다.
	HINSTANCE					m_hInstance = NULL;
	int							m_nObjectsOne = 0;		//현재 1번 플레이어가 쌓은 블럭 개수 오브젝트를 갖는다.
	//int							m_nObjectsTwo = 0;		//현재 2번 플레이어가 쌓은 블럭 개수
	//GameObject** m_ppObjects = NULL;		//이중 포인터.
	//MyContactListener m_contactListener;// 충돌 이벤트 처리를 위한 객체
	GameObject* m_pGround = NULL;
	GameObject* m_pGround2 = NULL;

	Player* m_pPlayer = NULL;
	//Player* m_pPlayer2 = NULL;

	b2World* m_b2World = NULL;

	//int m_nIndexPlayerOne = 0;		//현재 1번 플레이어의 오브젝트들의 몇 번째 인덱스가 플레이어의 입력 제어를 받을지
	//std::vector<GameObject*> m_objects;

	//int m_nIndexPlayerTwo = 0;
	//std::vector<GameObject*> m_objects2;

	//랜덤하게 생성되는 블럭 관련 부분.
	//GameObject* m_pNextGameObjectOne = NULL; //다음출력될 블럭을 미리 보여주는 거. 플레이어 1
	//GameObject* m_pNextGameObjectTwo = NULL; //다음출력될 블럭을 미리 보여주는 거.  플레이어 2

	//Point2D m_pTopPosition1;	//플레이어 1의 블럭들 중 가장 높은거 저장.
	Point2D m_pTopPosition2;



	//MeshGetInfo* m_pRandomMeshGet = NULL;	//8개의 메쉬 정보를 갖고있다.
	//int m_nRandomCount = 8;	//블럭 메쉬는 총 8개를 의미.
	//int m_nIndex = 0; //현재 선택된 메쉬의 종류를 판단해줄 index값 0~7
	//int m_nIndex2 = 0;
	DWORD m_dColor[8] = { RGB(125,0,0),RGB(200,0,0), RGB(120,120,0), RGB(120,200,0), RGB(120,0,102), RGB(200,0,200),
	RGB(0,125,125), RGB(0,200,125) };	//블럭들 색상

	HBITMAP m_bitmapBlend = NULL;
	HBITMAP m_bitmapBlend2 = NULL;
	int m_nHorizontalWidth = 0;	//바닥을 바라보고 있는 너비 저장.
	int m_nHorizontalWidth2 = 0;

	int m_MouseX; //마우스X값
	int m_MouseY; //마우스Y값
	int m_iPlayercnt1;
	int m_iPlayercnt2;
	float m_fTime = 0;
	//bool m_bKeyInput = false; //키입력 체크
	//bool m_bGameStop = false;
	bool m_bPauseGame = false; // 게임 멈추기
	bool m_bmainbtn = false;// 메인 화면으로 가기
	bool m_bhideMenu = false; // pause 메뉴 숨기기
	bool m_bGameStart = false; // GameStart를 시작할 때 체크하는 용도
	bool m_bExitBtn = false; // Exit 버튼을 눌렀을 때 체크하는 용도


	double m_DbtnLeft = FRAMEBUFFER_WIDTH / 3;// 좌표값 용도 입니다.
	double m_DbtnTop = FRAMEBUFFER_HEIGHT / 10 * 4;// 좌표값 용도 입니다.
	double m_DbtnRight = m_DbtnLeft + FRAMEBUFFER_WIDTH / 3;// 좌표값 용도 입니다.
	double m_DbtnBottom = m_DbtnTop + FRAMEBUFFER_HEIGHT / 10;// 좌표값 용도 입니다.

	NetworkManager* m_pNetworkManager = nullptr; //네트워크 매니저 추가
	

public:

	bool		m_bTriggerActive = true; // 이것이 true가 되면 동적 콜라이더로 중력의 영향을 받도록 바꿔준다.
	bool		m_bTriggerActive2 = true; // 이것이 true가 되면 동적 콜라이더로 중력의 영향을 받도록 바꿔준다.

	Player* m_pPlayer2 = NULL;
	bool m_bKeyInput = false; //키입력 체크
	bool m_bGameStop = false;
	std::vector<GameObject*> m_objects;
	GameObject* m_pNextGameObjectOne = NULL; //다음출력될 블럭을 미리 보여주는 거. 플레이어 1
	GameObject* m_pNextGameObjectTwo = NULL; //다음출력될 블럭을 미리 보여주는 거.  플레이어 2
	int m_nIndex = 0; //현재 선택된 메쉬의 종류를 판단해줄 index값 0~7
	int m_nIndex2 = 0;
	int m_nIndexPlayerOne = 0; 
	int m_nIndexPlayerTwo = 0; 
	int m_nRandomCount = 8;
	MeshGetInfo* m_pRandomMeshGet = NULL;
	std::vector<GameObject*> m_objects2;
	float m_fElapsedTimeForPlayerTwo; // 플레이어 2의 중력 타이머 
	const float GRAVITY_TRIGGER_TIME = 5.0f; // 5초마다 중력 발동 
	Point2D m_pTopPosition1;	//플레이어 1의 블럭들 중 가장 높은거 저장
	WPARAM m_lastKeyPressed; 
	int							m_nObjectsTwo = 0;
	int m_playerCount; // 서버에서 받은 플레이어 카운트 값 저장  12/07
	
	
	void LogDebugOutput(const std::string& message);
	

	

public:
	virtual void BuildObjects();	//오브젝트 생성
	virtual void ReleaseObjects();	//오브젝트 삭제
	virtual GameObject* RandomMesh(Player* pPlayer);	//이 친구가 다음 전달할 게임 오브젝트를 넘겨줄거야.
	virtual int RunTimeBuildObject(int index, Player* pPlayer);	//게임 중 오브젝트를 생성해줄 친구.
	virtual void RunTimeReleaseObjects(std::vector<GameObject*>& objects, int index);
	virtual void RunTimeReleaseObjects2(std::vector<GameObject*>& objects, int index);
	virtual Point2D CheckTopPostion(std::vector<GameObject*>& objects);	//가장 높은 블럭의 위치를 판단.
	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, Camera* pCamera, Camera* pCamera2);	//게임 오브젝트를 그릴려면 카메라가 필요해서 카메라를 받아온다.
	virtual void Blend(std::vector<GameObject*>& objects, HDC hDCFrameBuffer, Player* pPlayer);
	virtual void Blend2(std::vector<GameObject*>& objects, HDC hDCFrameBuffer, Player* pPlayer);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void LobbyBtn(HDC hDCFrameBuffer); // 로비화면 UI
	virtual void SKillUI(HDC hDCFrameBuffer); // 게임할 때 UI
	virtual void PauseUI(HDC hDCFrameBuffer); // Pause 할 때 UI
	virtual void timeUI(float fElapsedTime);// 시간 UI
	virtual void GameEndUI(HDC hDCFrameBuffer); //game끝날 때 UI

	


};

