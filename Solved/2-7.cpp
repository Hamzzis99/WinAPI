#include <windows.h>
#include <tchar.h>

#define MAX_LINES 10
#define MAX_CHARS 30

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"windows program 1";
LPCTSTR lpszWindowName = L"windows program 2-7";

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
	WndClass.hCursor = LoadCursor(NULL, IDC_IBEAM);
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
	SIZE size;

	static TCHAR lines[MAX_LINES][MAX_CHARS + 1];
	static int counts[MAX_LINES];

	static int currentLine = 0;
	static int currentCol = 0;

	switch (uMsg) {
	case WM_CREATE:
		for (int i = 0; i < MAX_LINES; i++) counts[i] = 0;

		CreateCaret(hWnd, NULL, 5, 15);
		ShowCaret(hWnd);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		SetBkMode(hDC, TRANSPARENT);

		for (int i = 0; i < MAX_LINES; i++) {
			if (counts[i] > 0) {
				TextOut(hDC, 10, 10 + (i * 20), lines[i], counts[i]);
			}
		}

		if (currentCol == 0) {
			SetCaretPos(10, 10 + (currentLine * 20));
		}
		else {
			GetTextExtentPoint32(hDC, lines[currentLine], currentCol, &size);
			SetCaretPos(10 + size.cx, 10 + (currentLine * 20));
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_CHAR:
		if (wParam == VK_ESCAPE) {
			currentLine = 0;
			currentCol = 0;
			for (int i = 0; i < MAX_LINES; i++) counts[i] = 0;
		}
		else if (wParam == VK_BACK) {
			if (currentCol > 0) {
				for (int i = currentCol; i < counts[currentLine]; i++) {
					lines[currentLine][i - 1] = lines[currentLine][i];
				}
				counts[currentLine]--;
				currentCol--;
			}
			else {
				if (currentLine > 0) {
					currentLine--;
					currentCol = counts[currentLine];
				}
			}
		}
		else if (wParam == VK_RETURN) {
			currentLine++;
			if (currentLine >= MAX_LINES) currentLine = 0;
			currentCol = 0;
		}
		else if (wParam >= 32) {
			if (currentCol < MAX_CHARS) {
				lines[currentLine][currentCol] = (TCHAR)wParam;
				currentCol++;

				if (currentCol > counts[currentLine]) {
					counts[currentLine] = currentCol;
				}

				if (currentCol >= MAX_CHARS) {
					currentLine++;
					if (currentLine >= MAX_LINES) currentLine = 0;
					currentCol = 0;
				}
			}
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_DESTROY:
		HideCaret(hWnd);
		DestroyCaret();
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}