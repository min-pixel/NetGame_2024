#include "MainH.h"
#include "Gameframework.h"

//-----------------------------------------------------------------------------
// File: GameFramework.cpp
//-----------------------------------------------------------------------------

b2Vec2 gravity(0, -98.0f);
b2World world(gravity);

void GameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)			//���� ������ �Լ��� ������� ����	
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	BuildFrameBuffer();		//������ ���� �����.

	BuildObjects();				//������Ʈ �����.

	::srand(timeGetTime());
}

void GameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_hBitmapFrameBuffer) ::DeleteObject(m_hBitmapFrameBuffer);
	if (m_hDCFrameBuffer) ::DeleteDC(m_hDCFrameBuffer);
}

void GameFramework::BuildFrameBuffer()
{

	::GetClientRect(m_hWnd, &m_rcClient);		//Ŭ���̾�Ʈ�� ũ�� ��������.

	HDC hDC = ::GetDC(m_hWnd);		//Ŭ���̾�Ʈ DC ��������.

	m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);		//HDC�� �����ͺ��� DC, �޸� DC�� �����.
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top);		//Ŭ���̾�Ʈ ������ ������� ���� ������ �����.
	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);		//m_hDCFrameBuffer������ ���ʿ� �׸��� �׸� �� �ְ� �ȴ�.
	::ReleaseDC(m_hWnd, hDC);
	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
	m_hBitmapBackGround = LoadBitmap(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
	//m_hBitmapBackGround = (HBITMAP)LoadImage(m_hInstance,
	//	TEXT("cat.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
}

void GameFramework::ClearFrameBuffer(DWORD dwColor)		//m_hDCFrameBuffer�̶�� �����ͺ� DC�� ���ؼ� ������ ���� ���ϴ� ����� ��� �����.
{
	HDC hDC = ::GetDC(m_hWnd);

	// ��� ��Ʈ�� ����
	HDC hDCBackGround = ::CreateCompatibleDC(hDC);
	::SelectObject(hDCBackGround, m_hBitmapBackGround);
	BITMAP bitmap;
	::GetObject(m_hBitmapBackGround, sizeof(BITMAP), &bitmap);
	::StretchBlt(m_hDCFrameBuffer, 0, 0, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top,
		hDCBackGround, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
	::DeleteDC(hDCBackGround);
	::ReleaseDC(m_hWnd, hDC);

}
void GameFramework::PresentFrameBuffer()
{
	HDC hDC = ::GetDC(m_hWnd);

	::BitBlt(hDC, m_rcClient.left, m_rcClient.top, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top, m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, SRCCOPY);		//�޸� DC �̹����� Ŭ���̾�Ʈ DC�� ���̰� ���.

	::ReleaseDC(m_hWnd, hDC);
}

void GameFramework::BuildObjects()		//ī�޶�, �÷��̾�
{
	Camera* pCamera = new Camera();
	pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH / 2, FRAMEBUFFER_HEIGHT);
	Camera* pCamera2 = new Camera();
	pCamera2->SetViewport(FRAMEBUFFER_WIDTH / 2, 0, FRAMEBUFFER_WIDTH / 2, FRAMEBUFFER_HEIGHT);
	cubeB* playerMesh = new cubeB(10.0f, 10.0f);
	m_pPlayer = new Player();
	m_pPlayer->SetCamera(pCamera);
	m_pPlayer->SetPosition(0.0f, 0.0f);
	//m_pPlayer->SetMesh(playerMesh);
	//m_pPlayer->SetColor(RGB(0, 0, 255));
	m_pPlayer2 = new Player();
	m_pPlayer2->SetCamera(pCamera2);
	m_pPlayer2->SetPosition(1000.0f, 0.0f);
	m_pScene = new Scene(m_pPlayer, m_pPlayer2, &world, m_hInstance);
	m_pScene->BuildObjects();
}

void GameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();

	if (m_pScene) delete m_pScene;

}

void GameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void GameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK GameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
			m_GameTimer.Stop();
		else
			m_GameTimer.Start();
		break;
	}
	case WM_SIZE:
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void GameFramework::ProcessInput()	// �Է� ���� ó�� ���� �÷��̾��� ������
{
	//static UCHAR pKeyBuffer[256];
	//if (::GetKeyboardState(pKeyBuffer))
	//{
	//	float cxKeyDelta = 0.0f, cyKeyDelta = 0.0f;
	//
	//	if (pKeyBuffer[VK_LEFT] & 0xF0) cxKeyDelta = -0.1f;
	//	if (pKeyBuffer[VK_RIGHT] & 0xF0) cxKeyDelta = +0.1f;
	//	if (pKeyBuffer[VK_UP] & 0xF0) cyKeyDelta = +0.1f;
	//	if (pKeyBuffer[VK_DOWN] & 0xF0) cyKeyDelta = -0.1f;

	//	if ((cxKeyDelta != 0.0f) || (cyKeyDelta != 0.0f) ) m_pPlayer->Move(cxKeyDelta, cyKeyDelta);
	//}

}

void GameFramework::AnimateObjects()	//��� �����ӿ� ���� ����� ���⼭ ����
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();
	if (m_pScene) m_pScene->Animate(fTimeElapsed);		//Scene�� �ִ� Animate�� ȣ��
	m_pScene->timeUI(fTimeElapsed);
}


void GameFramework::FrameAdvance()		//ó���ؾ��� ������ �޽����� ������ �Ź� ȣ���ϴ� �Լ�. �ϳ��� �������� ó���Ѵ�.
{
	m_GameTimer.Tick(0.0f);
	tim += m_GameTimer.GetTimeElapsed();
	if (tim >= 1.0f / 60.0f)
	{
		world.Step(1.0f / 60.0f, 6, 2);
		tim = 0.0f;
	}

	ProcessInput();	//���� �Է��� ó���ϰ�

	AnimateObjects();	//������ ���⼱ �ִϸ��̼�, �̵�, ȸ��, �浹���� ��� ���� ���

	ClearFrameBuffer(RGB(255, 255, 255));		//�޸�DC ȭ�� Ŭ����.
	Camera* pCamera = m_pPlayer->GetCamera();
	Camera* pCamera2 = m_pPlayer2->GetCamera();
	if (m_pScene) m_pScene->Render(m_hDCFrameBuffer, pCamera, pCamera2);

	PresentFrameBuffer();		//�޸� ������ �׷��� �̹����� Ŭ���̾�Ʈ DC�� Bitbltó��.


}


