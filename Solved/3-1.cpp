#include <windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"My Window Class";
LPCTSTR lpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define SHAPE_RECT  0
#define SHAPE_ELLIP 1
#define SHAPE_TRI   2

#define MOVE_STOP   0
#define MOVE_H      1
#define MOVE_V      2
#define MOVE_S      3

#define BSIZE 20

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpszCmdParam, int nCmdShow)
{
    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;

    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hDC;
    static int centerX, centerY;
    static int shapeType;
    static int moveMode;
    static int speed;
    static int dx, dy;
    static RECT rectView;

    switch (uMsg) {
    case WM_CREATE:
        GetClientRect(hWnd, &rectView);
        centerX = 100; centerY = 100;
        shapeType = SHAPE_RECT;
        moveMode = MOVE_STOP;
        speed = 50;
        dx = 1; dy = 1;
        break;

    case WM_SIZE:
        GetClientRect(hWnd, &rectView);
        break;

    case WM_LBUTTONDOWN:
        centerX = LOWORD(lParam);
        centerY = HIWORD(lParam);
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_KEYDOWN:
        switch (wParam) {
        case 'R':
            shapeType = SHAPE_RECT;
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case 'E':
            shapeType = SHAPE_ELLIP;
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case 'T':
            shapeType = SHAPE_TRI;
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case 'H':
            moveMode = MOVE_H;
            dx = 1;
            KillTimer(hWnd, 1);
            SetTimer(hWnd, 1, speed, NULL);
            break;
        case 'V':
            moveMode = MOVE_V;
            dy = 1;
            KillTimer(hWnd, 1);
            SetTimer(hWnd, 1, speed, NULL);
            break;
        case 'S':
            moveMode = MOVE_S;
            KillTimer(hWnd, 1);
            SetTimer(hWnd, 1, speed, NULL);
            break;
        case 'P':
            moveMode = MOVE_STOP;
            KillTimer(hWnd, 1);
            break;
        case 'A':
            if (speed > 10) speed -= 10;
            if (moveMode != MOVE_STOP) {
                KillTimer(hWnd, 1);
                SetTimer(hWnd, 1, speed, NULL);
            }
            break;
        case 'Z':
            if (speed < 500) speed += 10;
            if (moveMode != MOVE_STOP) {
                KillTimer(hWnd, 1);
                SetTimer(hWnd, 1, speed, NULL);
            }
            break;
        case 'Q':
            DestroyWindow(hWnd);
            break;
        }
        break;

    case WM_TIMER:
        switch (moveMode) {
        case MOVE_H:
            centerX += dx * 10;
            if (dx > 0 && centerX + BSIZE >= rectView.right) {
                dx = -1;
                centerY += 2 * BSIZE + 10;
                if (centerY + BSIZE >= rectView.bottom) {
                    centerX = BSIZE; centerY = BSIZE;
                    dx = 1;
                }
            }
            else if (dx < 0 && centerX - BSIZE <= 0) {
                dx = 1;
                centerY += 2 * BSIZE + 10;
                if (centerY + BSIZE >= rectView.bottom) {
                    centerX = BSIZE; centerY = BSIZE;
                    dx = 1;
                }
            }
            break;

        case MOVE_V:
            centerY += dy * 10;
            if (dy > 0 && centerY + BSIZE >= rectView.bottom) {
                dy = -1;
                centerX += 2 * BSIZE + 10;
                if (centerX + BSIZE >= rectView.right) {
                    centerX = BSIZE; centerY = BSIZE;
                    dy = 1;
                }
            }
            else if (dy < 0 && centerY - BSIZE <= 0) {
                dy = 1;
                centerX += 2 * BSIZE + 10;
                if (centerX + BSIZE >= rectView.right) {
                    centerX = BSIZE; centerY = BSIZE;
                    dy = 1;
                }
            }
            break;

        case MOVE_S:
            centerX += dx * 10;
            centerY += dy * 10;
            if (centerX - BSIZE <= 0)               dx = 1;
            if (centerX + BSIZE >= rectView.right)  dx = -1;
            if (centerY - BSIZE <= 0)               dy = 1;
            if (centerY + BSIZE >= rectView.bottom) dy = -1;
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);
        {
            HPEN   hPen = (HPEN)GetStockObject(BLACK_PEN);
            HPEN   hOldPen = (HPEN)SelectObject(hDC, hPen);
            HBRUSH hBrush, hOldBrush;

            if (shapeType == SHAPE_RECT) {
                hBrush = CreateSolidBrush(RGB(255, 150, 150));
                hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
                Rectangle(hDC, centerX - BSIZE, centerY - BSIZE,
                    centerX + BSIZE, centerY + BSIZE);
                SelectObject(hDC, hOldBrush);
                DeleteObject(hBrush);
            }
            else if (shapeType == SHAPE_ELLIP) {
                hBrush = CreateSolidBrush(RGB(150, 200, 255));
                hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
                Ellipse(hDC, centerX - BSIZE, centerY - BSIZE,
                    centerX + BSIZE, centerY + BSIZE);
                SelectObject(hDC, hOldBrush);
                DeleteObject(hBrush);
            }
            else if (shapeType == SHAPE_TRI) {
                POINT pt[3];
                pt[0].x = centerX;         pt[0].y = centerY - BSIZE;
                pt[1].x = centerX - BSIZE; pt[1].y = centerY + BSIZE;
                pt[2].x = centerX + BSIZE; pt[2].y = centerY + BSIZE;

                hBrush = CreateSolidBrush(RGB(255, 255, 0));
                hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
                Polygon(hDC, pt, 3);
                SelectObject(hDC, hOldBrush);
                DeleteObject(hBrush);
            }

            SelectObject(hDC, hOldPen);
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