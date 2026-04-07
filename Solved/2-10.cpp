#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SHAPES 15

// 전역 변수 및 상수 (사용자 요청 스타일)
HINSTANCE g_hInst;
LPCTSTR lpszClass = L"windows program 10";
LPCTSTR lpszWindowName = L"windows program 10";

// 도형 정보 구조체
typedef struct {
    int x, y;
    int size;
    COLORREF color;
} SHAPE;

// 영역별 데이터 구조체
typedef struct {
    SHAPE shapes[MAX_SHAPES];
    int count;
    COLORREF shapeColor;
} REGION_DATA;

// 전역 변수들
REGION_DATA g_regions[3];
RECT g_outerRect, g_innerRect, g_clientRect;

// 함수 프로토타입 선언
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CalcRegions(HWND hWnd);
void GenerateAllShapes(void);
void GenerateShapes(int regionIdx);
void DrawRegion(HDC hDC, int regionIdx);
void DrawBorders(HDC hDC);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;
    DWORD dwStyle = WS_OVERLAPPEDWINDOW;

    srand((unsigned int)time(NULL)); // 랜덤 시드 초기화

    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc = (WNDPROC)WndProc; // 함수 포인터 캐스팅
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_IBEAM);          // 커서: I-BEAM
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = lpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_ERROR);            // 작은 아이콘: ERROR

    RegisterClassEx(&WndClass);

    // 좌표: (100, 50), 크기: (800, 600)
    hWnd = CreateWindow(lpszClass, lpszWindowName, dwStyle, 100, 50, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&Message, NULL, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return (int)Message.wParam;
}

// ==================== 도형 생성 로직 함수들 ====================

COLORREF RandomColor(void) { return RGB(rand() % 256, rand() % 256, rand() % 256); }

void CalcRegions(HWND hWnd) {
    GetClientRect(hWnd, &g_clientRect);
    int cw = g_clientRect.right;
    int ch = g_clientRect.bottom;

    g_outerRect.left = (int)(cw * 0.15); g_outerRect.top = (int)(ch * 0.15);
    g_outerRect.right = (int)(cw * 0.85); g_outerRect.bottom = (int)(ch * 0.85);

    g_innerRect.left = (int)(cw * 0.32); g_innerRect.top = (int)(ch * 0.32);
    g_innerRect.right = (int)(cw * 0.68); g_innerRect.bottom = (int)(ch * 0.68);
}

BOOL IsOutsideRect(int x, int y, int margin, RECT* rc) {
    return (x + margin < rc->left || x - margin > rc->right ||
        y + margin < rc->top || y - margin > rc->bottom);
}

// 영역별 랜덤 위치 계산 (Region 1, 2, 3)
BOOL RandomPosRegion1(int* px, int* py, int margin) {
    for (int i = 0; i < 300; i++) {
        int x = margin + rand() % (g_clientRect.right - 2 * margin);
        int y = margin + rand() % (g_clientRect.bottom - 2 * margin);
        if (IsOutsideRect(x, y, margin, &g_outerRect)) { *px = x; *py = y; return TRUE; }
    }
    return FALSE;
}

BOOL RandomPosRegion2(int* px, int* py, int margin) {
    int ow = g_outerRect.right - g_outerRect.left - 2 * margin;
    int oh = g_outerRect.bottom - g_outerRect.top - 2 * margin;
    if (ow <= 0 || oh <= 0) return FALSE;
    for (int i = 0; i < 300; i++) {
        int x = g_outerRect.left + margin + rand() % ow;
        int y = g_outerRect.top + margin + rand() % oh;
        if (IsOutsideRect(x, y, margin, &g_innerRect)) { *px = x; *py = y; return TRUE; }
    }
    return FALSE;
}

BOOL RandomPosRegion3(int* px, int* py, int margin) {
    int w = g_innerRect.right - g_innerRect.left - 2 * margin;
    int h = g_innerRect.bottom - g_innerRect.top - 2 * margin;
    if (w <= 0 || h <= 0) return FALSE;
    *px = g_innerRect.left + margin + rand() % w;
    *py = g_innerRect.top + margin + rand() % h;
    return TRUE;
}

void GenerateShapes(int regionIdx) {
    COLORREF color = RandomColor();
    g_regions[regionIdx].shapeColor = color;
    int count = 0;
    if (regionIdx == 0) count = 5 + rand() % 4;      // 삼각형 5~8
    else if (regionIdx == 1) count = 4 + rand() % 4; // 사각형 4~7
    else count = 2 + rand() % 3;                     // 원 2~4

    g_regions[regionIdx].count = 0;
    for (int i = 0; i < count; i++) {
        SHAPE* s = &g_regions[regionIdx].shapes[i];
        s->size = 25 + rand() % 20;
        s->color = color;
        BOOL ok = FALSE;
        if (regionIdx == 0) ok = RandomPosRegion1(&s->x, &s->y, s->size / 2 + 2);
        else if (regionIdx == 1) ok = RandomPosRegion2(&s->x, &s->y, s->size / 2 + 2);
        else ok = RandomPosRegion3(&s->x, &s->y, s->size / 2 + 5);

        if (ok) g_regions[regionIdx].count++;
        else break;
    }
}

void GenerateAllShapes(void) { for (int i = 0; i < 3; i++) GenerateShapes(i); }

// ==================== 그리기 함수들 ====================

void DrawTriangle(HDC hDC, int cx, int cy, int size, COLORREF color) {
    POINT pts[3] = { {cx, cy - size / 2}, {cx - size / 2, cy + size / 2}, {cx + size / 2, cy + size / 2} };
    HBRUSH hB = CreateSolidBrush(color); HPEN hP = CreatePen(PS_SOLID, 1, color);
    HBRUSH oB = (HBRUSH)SelectObject(hDC, hB); HPEN oP = (HPEN)SelectObject(hDC, hP);
    Polygon(hDC, pts, 3);
    SelectObject(hDC, oB); SelectObject(hDC, oP); DeleteObject(hB); DeleteObject(hP);
}

void DrawFilledRect(HDC hDC, int cx, int cy, int size, COLORREF color) {
    HBRUSH hB = CreateSolidBrush(color); HPEN hP = CreatePen(PS_SOLID, 1, color);
    HBRUSH oB = (HBRUSH)SelectObject(hDC, hB); HPEN oP = (HPEN)SelectObject(hDC, hP);
    Rectangle(hDC, cx - size / 2, cy - size / 2, cx + size / 2, cy + size / 2);
    SelectObject(hDC, oB); SelectObject(hDC, oP); DeleteObject(hB); DeleteObject(hP);
}

void DrawFilledCircle(HDC hDC, int cx, int cy, int size, COLORREF color) {
    HBRUSH hB = CreateSolidBrush(color); HPEN hP = CreatePen(PS_SOLID, 1, color);
    HBRUSH oB = (HBRUSH)SelectObject(hDC, hB); HPEN oP = (HPEN)SelectObject(hDC, hP);
    Ellipse(hDC, cx - size / 2, cy - size / 2, cx + size / 2, cy + size / 2);
    SelectObject(hDC, oB); SelectObject(hDC, oP); DeleteObject(hB); DeleteObject(hP);
}

void DrawRegion(HDC hDC, int regionIdx) {
    for (int i = 0; i < g_regions[regionIdx].count; i++) {
        SHAPE* s = &g_regions[regionIdx].shapes[i];
        if (regionIdx == 0) DrawTriangle(hDC, s->x, s->y, s->size, s->color);
        else if (regionIdx == 1) DrawFilledRect(hDC, s->x, s->y, s->size, s->color);
        else DrawFilledCircle(hDC, s->x, s->y, s->size, s->color);
    }
}

void DrawBorders(HDC hDC) {
    HPEN hP = CreatePen(PS_SOLID, 2, RGB(100, 100, 255));
    HPEN oP = (HPEN)SelectObject(hDC, hP);
    HBRUSH oB = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
    Rectangle(hDC, g_outerRect.left, g_outerRect.top, g_outerRect.right, g_outerRect.bottom);
    Rectangle(hDC, g_innerRect.left, g_innerRect.top, g_innerRect.right, g_innerRect.bottom);
    SelectObject(hDC, oP); SelectObject(hDC, oB); DeleteObject(hP);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    HDC hDC;
    PAINTSTRUCT ps;

    switch (iMessage) {
    case WM_CREATE:
        CalcRegions(hWnd);
        GenerateAllShapes();
        return 0;

    case WM_SIZE:
        CalcRegions(hWnd);
        GenerateAllShapes();
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;

    case WM_KEYDOWN:
        switch (wParam) {
        case VK_RETURN: GenerateAllShapes(); break;
        case '1': GenerateShapes(0); break;
        case '2': GenerateShapes(1); break;
        case '3': GenerateShapes(2); break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);
        DrawBorders(hDC);
        DrawRegion(hDC, 0); // 삼각형
        DrawRegion(hDC, 1); // 사각형
        DrawRegion(hDC, 2); // 원
        EndPaint(hWnd, &ps);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}