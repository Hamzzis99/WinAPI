#include <windows.h>
#include <tchar.h>
#include <time.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"windows program 1";
LPCTSTR lpszWindowName = L"실습 2-4: 들여쓰기 구구단";

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
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	RECT rect;
	SIZE size;
	TCHAR str[100];

	static int dan;
	static int sectionIndex;
	static COLORREF rowColor[10];

	int sectionWidth, lineSpacing, baseX, startY;
	int x, y, spaceWidth;

	switch (uMsg) {
	case WM_CREATE:
		srand((unsigned int)time(NULL));

		dan = rand() % 19 + 2;
		sectionIndex = rand() % dan;

		for (int j = 1; j <= 9; j++) {
			rowColor[j] = RGB(rand() % 200, rand() % 200, rand() % 200);
		}
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		SetBkMode(hDC, TRANSPARENT);

		sectionWidth = rect.right / dan;
		lineSpacing = rect.bottom / 15;
		baseX = sectionIndex * sectionWidth;
		startY = rect.bottom / 10;

		GetTextExtentPoint32(hDC, L" ", 1, &size);
		spaceWidth = size.cx;

		y = startY;

		for (int j = 1; j <= 9; j++) {
			x = baseX + (j - 1) * (spaceWidth * 3);

			SetTextColor(hDC, rowColor[j]);

			wsprintf(str, L"%d x %d = %d", dan, j, dan * j);
			TextOut(hDC, x, y, str, lstrlen(str));

			y += lineSpacing;
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}