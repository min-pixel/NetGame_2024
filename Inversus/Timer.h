#pragma once


// �ִ� ������ �ð� ���� ��
const ULONG MAX_SAMPLE_COUNT = 50;

class GameTimer
{
public:
    // ������
    GameTimer();
    // �Ҹ���
    virtual ~GameTimer();

    // ���� Ÿ�̸Ӹ� ������Ʈ�ϴ� �޼���
    void Tick(float fLockFPS = 0.0f);
    // Ÿ�̸Ӹ� �����ϴ� �޼���
    void Start();
    // Ÿ�̸Ӹ� �����ϴ� �޼���
    void Stop();
    // Ÿ�̸Ӹ� �缳���ϴ� �޼���
    void Reset();

    // ���� FPS�� ���ڿ��� ��ȯ�ϰ� �ش� ���ڿ��� ���̸� ��ȯ�ϴ� �޼���
    unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
    // ����� �ð��� ��ȯ�ϴ� �޼���
    float GetTimeElapsed();
    // �� ��� �ð��� ��ȯ�ϴ� �޼���
    float GetTotalTime();

private:
    double m_fTimeScale;                      // �ð� ������
    float m_fTimeElapsed;                     // ��� �ð�

    __int64 m_nBasePerformanceCounter;        // ���� ���� ī����
    __int64 m_nPausedPerformanceCounter;      // �Ͻ� ������ ���� ī����
    __int64 m_nStopPerformanceCounter;        // ������ ���� ī����
    __int64 m_nCurrentPerformanceCounter;     // ���� ���� ī����
    __int64 m_nLastPerformanceCounter;        // ������ ���� ī����

    __int64 m_PerformanceFrequencyPerSec;     // ���� ī���� ���ļ�

    float m_fFrameTime[MAX_SAMPLE_COUNT];     // ������ �ð� ���� �迭
    ULONG m_nSampleCount;                      // ���� ��

    unsigned long m_nCurrentFrameRate;        // ���� ������ ����Ʈ
    unsigned long m_FramePerSecond;           // �����Ӵ� ��
    float m_fFPSTimeElapsed;                  // FPS ��� �ð�

    bool m_bStopped;                          // Ÿ�̸Ӱ� �����ߴ��� ����
};