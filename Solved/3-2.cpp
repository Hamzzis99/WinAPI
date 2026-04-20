#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"My Window Class";
LPCTSTR lpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define PI         3.14159265358979

#define BG_CIRCLE  0
#define BG_RECT    1
#define BG_TRI     2

#define ROT_CIRCLE 0
#define ROT_RECT   1

typedef struct {
    int      cx, cy;
    COLORREF centerColor;
    int      orbitR;
    int      shapeR;
    double   theta;
    int      direction;
    int      rotShape;
    int      bgShape;
    COLORREF rotColor;
    int      timerInterval;
    BOOL     moved;
    int      savedCx, savedCy;
    BOOL     inverted;
} QUAD;

static QUAD g_quad[4];
static int  g_selected = -1;
static RECT g_rectView;
static int  g_speedDir = -1;

void InitQuads(HWND hWnd);
int  GetQuadIndex(int mx, int my);
void DrawBgShape(HDC hDC, QUAD* q);
void DrawRotShape(HDC hDC, QUAD* q, int x, int y);

COLORREF InvertColor(COLORREF c)
{
    return RGB(255 - GetRValue(c), 255 - GetGValue(c), 255 - GetBValue(c));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpszCmdParam, int nCmdShow)
{
    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;

    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    WndClass.lpfnWndProc = (WNDPROC)WndProc;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = lpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&WndClass);

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW,
        0, 0, 800, 600,
        NULL, (HMENU)NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&Message, 0, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return Message.wParam;
}

void InitQuads(HWND hWnd)
{
    GetClientRect(hWnd, &g_rectView);
    int halfW = g_rectView.right / 2;
    int halfH = g_rectView.bottom / 2;

    g_quad[0].cx = halfW / 2;           g_quad[0].cy = halfH / 2;
    g_quad[1].cx = halfW + halfW / 2;   g_quad[1].cy = halfH / 2;
    g_quad[2].cx = halfW / 2;           g_quad[2].cy = halfH + halfH / 2;
    g_quad[3].cx = halfW + halfW / 2;   g_quad[3].cy = halfH + halfH / 2;

    int intervals[4] = { 30, 50, 70, 90 };
    COLORREF colors[4] = {
        RGB(100, 150, 255),
        RGB(255, 180, 100),
        RGB(180, 255, 150),
        RGB(255, 150, 200)
    };

    for (int i = 0; i < 4; i++) {
        g_quad[i].centerColor = RGB(255, 0, 0);
        g_quad[i].orbitR = 80;
        g_quad[i].shapeR = 15;
        g_quad[i].theta = (rand() % 360) * PI / 180.0;
        g_quad[i].direction = (i % 2 == 0) ? +1 : -1;
        g_quad[i].rotShape = ROT_CIRCLE;
        g_quad[i].bgShape = BG_CIRCLE;
        g_quad[i].rotColor = colors[i];
        g_quad[i].timerInterval = intervals[i];
        g_quad[i].savedCx = g_quad[i].cx;
        g_quad[i].savedCy = g_quad[i].cy;
        g_quad[i].moved = FALSE;
        g_quad[i].inverted = FALSE;
    }
}

int GetQuadIndex(int mx, int my)
{
    int halfW = g_rectView.right / 2;
    int halfH = g_rectView.bottom / 2;
    if (mx < halfW && my < halfH) return 0;
    if (mx >= halfW && my < halfH) return 1;
    if (mx < halfW && my >= halfH) return 2;
    return 3;
}

void DrawBgShape(HDC hDC, QUAD* q)
{
    int r = q->orbitR;
    if (q->bgShape == BG_CIRCLE) {
        Ellipse(hDC, q->cx - r, q->cy - r, q->cx + r, q->cy + r);
    }
    else if (q->bgShape == BG_RECT) {
        Rectangle(hDC, q->cx - r, q->cy - r, q->cx + r, q->cy + r);
    }
    else {
        POINT pt[3];
        pt[0].x = q->cx;     pt[0].y = q->cy - r;
        pt[1].x = q->cx - r; pt[1].y = q->cy + r;
        pt[2].x = q->cx + r; pt[2].y = q->cy + r;
        Polygon(hDC, pt, 3);
    }
}

void DrawRotShape(HDC hDC, QUAD* q, int x, int y)
{
    int r = q->shapeR;
    if (q->rotShape == ROT_CIRCLE)
        Ellipse(hDC, x - r, y - r, x + r, y + r);
    else
        Rectangle(hDC, x - r, y - r, x + r, y + r);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hDC;
    int mx, my;

    switch (uMsg) {
    case WM_CREATE:
        srand((unsigned)time(NULL));
        InitQuads(hWnd);
        SetTimer(hWnd, 1, 15, NULL);
        break;

    case WM_SIZE:
        InitQuads(hWnd);
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_ERASEBKGND:
        return 1;

    case WM_TIMER:
        if (wParam == 1) {
            for (int i = 0; i < 4; i++) {
                double step = 0.05 * (60.0 / g_quad[i].timerInterval);
                g_quad[i].theta += g_quad[i].direction * step;
                if (g_quad[i].theta > 2 * PI) g_quad[i].theta -= 2 * PI;
                if (g_quad[i].theta < -2 * PI) g_quad[i].theta += 2 * PI;
            }
            InvalidateRect(hWnd, NULL, FALSE);
        }
        break;

    case WM_KEYDOWN:
        switch (wParam) {
        case '1': g_selected = 0; InvalidateRect(hWnd, NULL, FALSE); break;
        case '2': g_selected = 1; InvalidateRect(hWnd, NULL, FALSE); break;
        case '3': g_selected = 2; InvalidateRect(hWnd, NULL, FALSE); break;
        case '4': g_selected = 3; InvalidateRect(hWnd, NULL, FALSE); break;

        case 'C':
            if (g_selected >= 0)
                g_quad[g_selected].direction *= -1;
            break;
        case 'M':
            if (g_selected >= 0) {
                g_quad[g_selected].rotShape =
                    (g_quad[g_selected].rotShape == ROT_CIRCLE)
                    ? ROT_RECT : ROT_CIRCLE;
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
        case 'R':
            if (g_selected >= 0) {
                g_quad[g_selected].bgShape = BG_RECT;
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
        case 'T':
            if (g_selected >= 0) {
                g_quad[g_selected].bgShape = BG_TRI;
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
        case 'Q':
            DestroyWindow(hWnd);
            break;
        }
        break;

    case WM_LBUTTONDOWN:
        if (g_selected >= 0) {
            QUAD* q = &g_quad[g_selected];
            q->timerInterval += g_speedDir * 10;
            if (q->timerInterval <= 10) { q->timerInterval = 10;  g_speedDir = +1; }
            if (q->timerInterval >= 120) { q->timerInterval = 120; g_speedDir = -1; }
        }
        break;

    case WM_RBUTTONDOWN:
        if (g_selected >= 0) {
            QUAD* q = &g_quad[g_selected];
            mx = LOWORD(lParam);
            my = HIWORD(lParam);
            if (!q->moved) {
                q->savedCx = q->cx;
                q->savedCy = q->cy;
                q->cx = mx;
                q->cy = my;
                q->moved = TRUE;
            }
            else {
                q->cx = q->savedCx;
                q->cy = q->savedCy;
                q->moved = FALSE;
            }
            InvalidateRect(hWnd, NULL, FALSE);
        }
        break;

    case WM_RBUTTONDBLCLK:
        mx = LOWORD(lParam);
        my = HIWORD(lParam);
        {
            int idx = GetQuadIndex(mx, my);
            g_quad[idx].inverted = !g_quad[idx].inverted;
            InvalidateRect(hWnd, NULL, FALSE);
        }
        break;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);
        {
            int halfW = g_rectView.right / 2;
            int halfH = g_rectView.bottom / 2;

            HBRUSH hWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
            HPEN   hWhitePen = (HPEN)GetStockObject(WHITE_PEN);
            HBRUSH hOldBgBrush = (HBRUSH)SelectObject(hDC, hWhiteBrush);
            HPEN   hOldBgPen = (HPEN)SelectObject(hDC, hWhitePen);
            Rectangle(hDC, 0, 0, g_rectView.right + 1, g_rectView.bottom + 1);
            SelectObject(hDC, hOldBgBrush);
            SelectObject(hDC, hOldBgPen);

            HPEN hDivPen = CreatePen(PS_DOT, 1, RGB(200, 200, 200));
            HPEN hOldPen = (HPEN)SelectObject(hDC, hDivPen);
            MoveToEx(hDC, halfW, 0, NULL);
            LineTo(hDC, halfW, g_rectView.bottom);
            MoveToEx(hDC, 0, halfH, NULL);
            LineTo(hDC, g_rectView.right, halfH);
            SelectObject(hDC, hOldPen);
            DeleteObject(hDivPen);

            HPEN hBlackPen = (HPEN)GetStockObject(BLACK_PEN);
            HPEN hOldBlackPen = (HPEN)SelectObject(hDC, hBlackPen);

            for (int i = 0; i < 4; i++) {
                QUAD* q = &g_quad[i];

                COLORREF useCenterColor = q->inverted ? InvertColor(q->centerColor) : q->centerColor;
                COLORREF useRotColor = q->inverted ? InvertColor(q->rotColor) : q->rotColor;

                HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hNullBrush);
                DrawBgShape(hDC, q);
                SelectObject(hDC, hOldBrush);

                HBRUSH hCenterBrush = CreateSolidBrush(useCenterColor);
                HBRUSH hOld2 = (HBRUSH)SelectObject(hDC, hCenterBrush);
                Ellipse(hDC, q->cx - 4, q->cy - 4, q->cx + 4, q->cy + 4);
                SelectObject(hDC, hOld2);
                DeleteObject(hCenterBrush);

                int rx = q->cx + (int)(q->orbitR * cos(q->theta));
                int ry = q->cy + (int)(q->orbitR * sin(q->theta));

                HBRUSH hRotBrush = CreateSolidBrush(useRotColor);
                HBRUSH hOld3 = (HBRUSH)SelectObject(hDC, hRotBrush);
                DrawRotShape(hDC, q, rx, ry);
                SelectObject(hDC, hOld3);
                DeleteObject(hRotBrush);
            }

            if (g_selected >= 0) {
                int left, top, right, bottom;
                switch (g_selected) {
                case 0: left = 0;     top = 0;     right = halfW;             bottom = halfH;             break;
                case 1: left = halfW; top = 0;     right = g_rectView.right;  bottom = halfH;             break;
                case 2: left = 0;     top = halfH; right = halfW;             bottom = g_rectView.bottom; break;
                case 3: left = halfW; top = halfH; right = g_rectView.right;  bottom = g_rectView.bottom; break;
                }
                HPEN   hSelPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
                HPEN   hOldSelPen = (HPEN)SelectObject(hDC, hSelPen);
                HBRUSH hNull = (HBRUSH)GetStockObject(NULL_BRUSH);
                HBRUSH hOldBr = (HBRUSH)SelectObject(hDC, hNull);
                Rectangle(hDC, left, top, right, bottom);
                SelectObject(hDC, hOldSelPen);
                SelectObject(hDC, hOldBr);
                DeleteObject(hSelPen);
            }

            SelectObject(hDC, hOldBlackPen);
        }
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}