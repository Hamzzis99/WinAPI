#include <windows.h>
#include <tchar.h>
#include <time.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"windows program 1";
LPCTSTR lpszWindowName = L"windows program 2-4-2";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;

    DWORD dwStyle = WS_OVERLAPPEDWINDOW;

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
    WndClass.hIconSm = LoadIcon(NULL, IDI_ERROR);
    RegisterClassEx(&WndClass);

    hWnd = CreateWindow(lpszClass, lpszWindowName, dwStyle, 100, 50, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&Message, NULL, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hDC;
    PAINTSTRUCT ps;
    SIZE size;
    RECT clientRect;

    static int rectX, rectY, rectW, rectH;
    static COLORREF textColor;
    static TCHAR randomStr[2000];

    switch (uMsg) {
    case WM_CREATE:
    {
        srand((unsigned int)time(NULL));

        // 800x600 윈도우 크기를 '절대적인 기준'으로 잡고 랜덤 수식 적용
        rectW = rand() % 299 + 101;
        rectH = rand() % 199 + 101;
        rectX = rand() % 399 + 1;
        rectY = rand() % 299 + 1;

        textColor = RGB(rand() % 256, rand() % 256, rand() % 256);

        for (int i = 0; i < 1999; i++) {
            randomStr[i] = (TCHAR)(rand() % 26 + 'A');
        }
        randomStr[1999] = '\0';
        break;
    }

    case WM_PAINT:
    {
        hDC = BeginPaint(hWnd, &ps);

        GetClientRect(hWnd, &clientRect);

        SetBkMode(hDC, TRANSPARENT);
        SetTextColor(hDC, textColor);

        GetTextExtentPoint32(hDC, L"A", 1, &size);
        int charWidth = size.cx;
        int charHeight = size.cy;

        int charIdx = 0;

        for (int x = rectX; x <= rectX + rectW; x += charWidth) {
            TextOut(hDC, x, rectY, &randomStr[charIdx++ % 1999], 1);
        }
        for (int x = rectX; x <= rectX + rectW; x += charWidth) {
            TextOut(hDC, x, rectY + rectH, &randomStr[charIdx++ % 1999], 1);
        }
        for (int y = rectY + charHeight; y < rectY + rectH; y += charHeight) {
            TextOut(hDC, rectX, y, &randomStr[charIdx++ % 1999], 1);
        }
        for (int y = rectY + charHeight; y < rectY + rectH; y += charHeight) {
            TextOut(hDC, rectX + rectW, y, &randomStr[charIdx++ % 1999], 1);
        }

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}