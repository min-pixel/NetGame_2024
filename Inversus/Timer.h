#pragma once


// 최대 프레임 시간 샘플 수
const ULONG MAX_SAMPLE_COUNT = 50;

class GameTimer
{
public:
    // 생성자
    GameTimer();
    // 소멸자
    virtual ~GameTimer();

    // 게임 타이머를 업데이트하는 메서드
    void Tick(float fLockFPS = 0.0f);
    // 타이머를 시작하는 메서드
    void Start();
    // 타이머를 정지하는 메서드
    void Stop();
    // 타이머를 재설정하는 메서드
    void Reset();

    // 현재 FPS를 문자열로 반환하고 해당 문자열의 길이를 반환하는 메서드
    unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
    // 경과한 시간을 반환하는 메서드
    float GetTimeElapsed();
    // 총 경과 시간을 반환하는 메서드
    float GetTotalTime();

private:
    double m_fTimeScale;                      // 시간 스케일
    float m_fTimeElapsed;                     // 경과 시간

    __int64 m_nBasePerformanceCounter;        // 시작 성능 카운터
    __int64 m_nPausedPerformanceCounter;      // 일시 정지된 성능 카운터
    __int64 m_nStopPerformanceCounter;        // 정지된 성능 카운터
    __int64 m_nCurrentPerformanceCounter;     // 현재 성능 카운터
    __int64 m_nLastPerformanceCounter;        // 마지막 성능 카운터

    __int64 m_PerformanceFrequencyPerSec;     // 성능 카운터 주파수

    float m_fFrameTime[MAX_SAMPLE_COUNT];     // 프레임 시간 샘플 배열
    ULONG m_nSampleCount;                      // 샘플 수

    unsigned long m_nCurrentFrameRate;        // 현재 프레임 레이트
    unsigned long m_FramePerSecond;           // 프레임당 초
    float m_fFPSTimeElapsed;                  // FPS 경과 시간

    bool m_bStopped;                          // 타이머가 정지했는지 여부
};