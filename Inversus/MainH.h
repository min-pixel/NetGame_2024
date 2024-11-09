#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����
#include <Windows.h>
// C ��Ÿ�� ��� �����Դϴ�.
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

    // ���͸� ���� ����(ũ�Ⱑ 1�� ����)�� ����ȭ�ϴ� �Լ�
    inline Point2D normalize( Point2D& vec) {
        float mag = Magnitude(vec);
        if (mag > 0.0f) {
            return { vec.x / mag, vec.y / mag };
        }
        return vec; // ũ�Ⱑ 0�� ��� ���� ���͸� ��ȯ�մϴ�.
    }

    // �� ������ ������ ����ϴ� �Լ�
    inline Point2D Add(Point2D& vec1, Point2D& vec2) {
        return { vec1.x + vec2.x, vec1.y + vec2.y };
    }

    // �� ������ ������ ����ϴ� �Լ�
    inline Point2D Subtract( Point2D& vec1,  Point2D& vec2) {
        return { vec1.x - vec2.x, vec1.y - vec2.y };
    }
}

