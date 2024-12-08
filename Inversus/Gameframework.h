// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "Scene.h"
#include "Timer.h"
#include "NetworkManager.h"

class GameFramework
{
public:
	GameFramework() { }
	~GameFramework() { }


private:
	HINSTANCE					m_hInstance = NULL;
	HWND						m_hWnd = NULL;

	RECT						m_rcClient;		//메인 윈도우 클라이언트 영역을 나타내는 사각형

	HDC							m_hDCFrameBuffer = NULL;		//HDC window API 핸들의미 클라이언트 DC를 의미 ==> 클라이언트 DC와 컨팩터블 할 메모리 DC인데?
		
	HBITMAP						m_hBitmapFrameBuffer = NULL;		//미리 한 프레임 이미지를 그릴 빔맵이다.
	HBITMAP						m_hBitmapBackGround = NULL;
	

	float tim = 0.0f;
	//뒷 배경화면을 갖는 BitMAP

	Player* m_pPlayer = NULL;
	Player* m_pPlayer2 = NULL;

	Scene* m_pScene = NULL;	//Scene은 게임 오브젝트들의 배열, 게임 오브젝트들은 메쉬에 대한 포인터와 월드변환에 필요한 정보들을 갖고있는 클래스이다.

	GameTimer					m_GameTimer;

	POINT						m_ptOldCursorPos;

	int m_playerCount; // 서버에서 받은 플레이어 카운트 값 저장  12/07

	void LogDebugOutput(const std::string& message);

	// 네트워크 관련 멤버
	NetworkManager* m_pNetworkManager = NULL; // NetworkManager 객체
	bool m_bGameStart = false; // 게임 시작 여부 플래그
	bool m_bGameStop = false; // 게임 정지 여부 플래그

public:
	void OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	
};

