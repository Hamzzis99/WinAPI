#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SHAPES 15

// 도형 정보
typedef struct {
    int x, y;
    int size;
    COLORREF color;
} SHAPE;

// 영역별 도형
typedef struct {
    SHAPE shapes[MAX_SHAPES];
    int count;
    COLORREF shapeColor;
} REGION_DATA;

REGION_DATA g_regions[3];
RECT g_outerRect, g_innerRect, g_clientRect;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 랜덤 색상
COLORREF RandomColor(void)
{
    return RGB(rand() % 256, rand() % 256, rand() % 256);
}

// 영역 좌표 계산
void CalcRegions(HWND hWnd)
{
    GetClientRect(hWnd, &g_clientRect);
    int cw = g_clientRect.right;
    int ch = g_clientRect.bottom;

    g_outerRect.left = (int)(cw * 0.15);
    g_outerRect.top = (int)(ch * 0.15);
    g_outerRect.right = (int)(cw * 0.85);
    g_outerRect.bottom = (int)(ch * 0.85);

    g_innerRect.left = (int)(cw * 0.32);
    g_innerRect.top = (int)(ch * 0.32);
    g_innerRect.right = (int)(cw * 0.68);
    g_innerRect.bottom = (int)(ch * 0.68);
}

// 점이 사각형 바깥인지
BOOL IsOutsideRect(int x, int y, int margin, RECT* rc)
{
    return (x + margin < rc->left || x - margin > rc->right ||
        y + margin < rc->top || y - margin > rc->bottom);
}

// 영역1(바깥 사각형 외부) 랜덤 위치
BOOL RandomPosRegion1(int* px, int* py, int margin)
{
    int cw = g_clientRect.right;
    int ch = g_clientRect.bottom;
    for (int i = 0; i < 300; i++) {
        int x = margin + rand() % (cw - 2 * margin);
        int y = margin + rand() % (ch - 2 * margin);
        if (IsOutsideRect(x, y, margin, &g_outerRect)) {
            *px = x; *py = y;
            return TRUE;
        }
    }
    return FALSE;
}

// 영역2(바깥-안쪽 사이) 랜덤 위치
BOOL RandomPosRegion2(int* px, int* py, int margin)
{
    int ow = g_outerRect.right - g_outerRect.left - 2 * margin;
    int oh = g_outerRect.bottom - g_outerRect.top - 2 * margin;
    if (ow <= 0 || oh <= 0) return FALSE;
    for (int i = 0; i < 300; i++) {
        int x = g_outerRect.left + margin + rand() % ow;
        int y = g_outerRect.top + margin + rand() % oh;
        if (IsOutsideRect(x, y, margin, &g_innerRect)) {
            *px = x; *py = y;
            return TRUE;
        }
    }
    return FALSE;
}

// 영역3(안쪽 사각형 내부) 랜덤 위치
BOOL RandomPosRegion3(int* px, int* py, int margin)
{
    int w = g_innerRect.right - g_innerRect.left - 2 * margin;
    int h = g_innerRect.bottom - g_innerRect.top - 2 * margin;
    if (w <= 0 || h <= 0) return FALSE;
    *px = g_innerRect.left + margin + rand() % w;
    *py = g_innerRect.top + margin + rand() % h;
    return TRUE;
}

// 영역별 도형 생성
void GenerateShapes(int regionIdx)
{
    COLORREF color = RandomColor();
    g_regions[regionIdx].shapeColor = color;

    int count = 0;
    switch (regionIdx) {
    case 0: count = 5 + rand() % 4; break;   // 삼각형 5~8개
    case 1: count = 4 + rand() % 4; break;   // 사각형 4~7개
    case 2: count = 2 + rand() % 3; break;   // 원 2~4개
    }

    g_regions[regionIdx].count = 0;
    for (int i = 0; i < count; i++) {
        SHAPE* s = &g_regions[regionIdx].shapes[i];
        s->size = 25 + rand() % 20;
        s->color = color;

        BOOL ok = FALSE;
        switch (regionIdx) {
        case 0: ok = RandomPosRegion1(&s->x, &s->y, s->size / 2 + 2); break;
        case 1: ok = RandomPosRegion2(&s->x, &s->y, s->size / 2 + 2); break;
        case 2: ok = RandomPosRegion3(&s->x, &s->y, s->size / 2 + 5); break;
        }
        if (ok) g_regions[regionIdx].count++;
        else break;
    }
}

void GenerateAllShapes(void)
{
    for (int i = 0; i < 3; i++)
        GenerateShapes(i);
}

// ===== 그리기 함수들 =====

void DrawTriangle(HDC hDC, int cx, int cy, int size, COLORREF color)
{
    HBRUSH hBrush, oldBrush;
    HPEN hPen, oldPen;
    POINT pts[3];

    pts[0].x = cx;             pts[0].y = cy - size / 2;
    pts[1].x = cx - size / 2;  pts[1].y = cy + size / 2;
    pts[2].x = cx + size / 2;  pts[2].y = cy + size / 2;

    hPen = CreatePen(PS_SOLID, 1, color);
    oldPen = (HPEN)SelectObject(hDC, hPen);
    hBrush = CreateSolidBrush(color);
    oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

    Polygon(hDC, pts, 3);

    SelectObject(hDC, oldPen);
    SelectObject(hDC, oldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

void DrawFilledRect(HDC hDC, int cx, int cy, int size, COLORREF color)
{
    HBRUSH hBrush, oldBrush;
    HPEN hPen, oldPen;

    hPen = CreatePen(PS_SOLID, 1, color);
    oldPen = (HPEN)SelectObject(hDC, hPen);
    hBrush = CreateSolidBrush(color);
    oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

    Rectangle(hDC, cx - size / 2, cy - size / 2, cx + size / 2, cy + size / 2);

    SelectObject(hDC, oldPen);
    SelectObject(hDC, oldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

void DrawFilledCircle(HDC hDC, int cx, int cy, int size, COLORREF color)
{
    HBRUSH hBrush, oldBrush;
    HPEN hPen, oldPen;

    hPen = CreatePen(PS_SOLID, 1, color);
    oldPen = (HPEN)SelectObject(hDC, hPen);
    hBrush = CreateSolidBrush(color);
    oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

    Ellipse(hDC, cx - size / 2, cy - size / 2, cx + size / 2, cy + size / 2);

    SelectObject(hDC, oldPen);
    SelectObject(hDC, oldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

void DrawRegion(HDC hDC, int regionIdx)
{
    for (int i = 0; i < g_regions[regionIdx].count; i++) {
        SHAPE* s = &g_regions[regionIdx].shapes[i];
        switch (regionIdx) {
        case 0: DrawTriangle(hDC, s->x, s->y, s->size, s->color); break;
        case 1: DrawFilledRect(hDC, s->x, s->y, s->size, s->color); break;
        case 2: DrawFilledCircle(hDC, s->x, s->y, s->size, s->color); break;
        }
    }
}

void DrawBorders(HDC hDC)
{
    HPEN hPen, oldPen;
    HBRUSH oldBrush;

    hPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 255));
    oldPen = (HPEN)SelectObject(hDC, hPen);
    oldBrush = (HBRUSH)SelectObject(hDC, (HBRUSH)GetStockObject(NULL_BRUSH));

    Rectangle(hDC, g_outerRect.left, g_outerRect.top,
        g_outerRect.right, g_outerRect.bottom);
    Rectangle(hDC, g_innerRect.left, g_innerRect.top,
        g_innerRect.right, g_innerRect.bottom);

    SelectObject(hDC, oldBrush);
    SelectObject(hDC, oldPen);
    DeleteObject(hPen);
}

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    static TCHAR szAppName[] = TEXT("Practice2_10");
    HWND hWnd;
    MSG msg;
    WNDCLASS wndclass;

    srand((unsigned int)time(NULL));

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT("등록 실패"), szAppName, MB_ICONERROR);
        return 0;
    }

    hWnd = CreateWindow(szAppName,
        TEXT("실습 2-10: 영역별 랜덤 도형 (Enter:전체, 1/2/3:개별 새로고침)"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hDC;
    PAINTSTRUCT ps;

    switch (iMsg) {
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
        case VK_RETURN:
            GenerateAllShapes();
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case '1':
            GenerateShapes(0);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case '2':
            GenerateShapes(1);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case '3':
            GenerateShapes(2);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
        return 0;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);

        DrawBorders(hDC);
        DrawRegion(hDC, 0);  // 삼각형
        DrawRegion(hDC, 1);  // 사각형
        DrawRegion(hDC, 2);  // 원

        EndPaint(hWnd, &ps);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, iMsg, wParam, lParam);
}
