#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <Windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <math.h>
#include <cstdlib>
#include <cstdio>
#include "box2d/b2_body.h"
#include "box2d/Box2d.h"
#include <Mmsystem.h>
#include <random>
#include "resource.h"
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")
#pragma comment(lib, "msimg32.lib")

#define FRAMEBUFFER_WIDTH		800
#define FRAMEBUFFER_HEIGHT		600

#define DegreeToRadian(x)		float((x)*3.141592654f/180.0f)

class Point2D
{
public:
    Point2D() { }
    Point2D(float x, float y) { this->x = x; this->y = y; }
    ~Point2D() { }

    float						x = 0.0f;
    float						y = 0.0f;

};

namespace Vector2
{
    inline float Magnitude(Point2D& vec) {
        return sqrt(vec.x * vec.x + vec.y * vec.y);
    }

    // 벡터를 단위 벡터(크기가 1인 벡터)로 정규화하는 함수
    inline Point2D normalize( Point2D& vec) {
        float mag = Magnitude(vec);
        if (mag > 0.0f) {
            return { vec.x / mag, vec.y / mag };
        }
        return vec; // 크기가 0인 경우 원래 벡터를 반환합니다.
    }

    // 두 벡터의 덧셈을 계산하는 함수
    inline Point2D Add(Point2D& vec1, Point2D& vec2) {
        return { vec1.x + vec2.x, vec1.y + vec2.y };
    }

    // 두 벡터의 뺄셈을 계산하는 함수
    inline Point2D Subtract( Point2D& vec1,  Point2D& vec2) {
        return { vec1.x - vec2.x, vec1.y - vec2.y };
    }
}

