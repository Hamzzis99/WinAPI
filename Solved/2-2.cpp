#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <math.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"windows program 1";
LPCTSTR lpszWindowName = L"windows program 3";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;
	DWORD dwStyle = WS_OVERLAPPEDWINDOW |
		WS_CAPTION |
		WS_SYSMENU |
		WS_THICKFRAME |
		WS_HSCROLL |
		WS_VSCROLL;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_APPSTARTING);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_ERROR);
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszWindowName, dwStyle, 100, 50, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, NULL, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	TCHAR str[100];
	static int N = 2;

	RECT rect;
	int sectionWidth, lineSpacing, startY1, startY2;
	int dan, x, y;

	switch (uMsg) {
	case WM_CREATE:
		srand((unsigned int)time(NULL));
		N = rand() % 15 + 2;
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);

		wsprintf(str, L"현재 화면은 %d 등분입니다.", N);
		TextOut(hDC, 10, 10, str, lstrlen(str));

		sectionWidth = rect.right / N;
		lineSpacing = rect.bottom / 22;
		startY1 = rect.bottom / 10;
		startY2 = rect.bottom / 2 + (rect.bottom / 20);

		for (int i = 0; i < N; i++) {
			dan = 2 + i;
			x = i * sectionWidth + 15;
			y = startY1;

			for (int j = 1; j <= 9; j++) {
				wsprintf(str, L"%d x %d = %d", dan, j, dan * j);
				TextOut(hDC, x, y, str, lstrlen(str));
				y += lineSpacing;
			}
		}

		for (int i = 0; i < N; i++) {
			dan = (2 + N - 1) - i;
			x = i * sectionWidth + 15;
			y = startY2;

			for (int j = 1; j <= 9; j++) {
				wsprintf(str, L"%d x %d = %d", dan, j, dan * j);
				TextOut(hDC, x, y, str, lstrlen(str));
				y += lineSpacing;
			}
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}