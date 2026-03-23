#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdio.h>

struct ShapeData {
	int x, y;
	int n, count;
	COLORREF textColor;
	COLORREF bgColor;
};

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"windows program 1";
LPCTSTR lpszWindowName = L"windows program 2-6";

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

	static ShapeData history[10];
	static int histCount = 0;
	static bool bShowAll = false;

	static TCHAR inputBuf[100];
	static int inputLen = 0;

	switch (uMsg) {
	case WM_CREATE:
		srand((unsigned int)time(NULL));
		CreateCaret(hWnd, NULL, 5, 15);
		ShowCaret(hWnd);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		for (int i = 0; i < histCount; i++) {
			if (!bShowAll && i != histCount - 1) continue;

			SetTextColor(hDC, history[i].textColor);
			SetBkColor(hDC, history[i].bgColor);
			SetBkMode(hDC, OPAQUE);

			TCHAR rowStr[500] = L"";
			TCHAR temp[20];
			for (int c = 0; c < history[i].count; c++) {
				wsprintf(temp, L"%d", history[i].n);
				lstrcat(rowStr, temp);
			}

			for (int r = 0; r < history[i].count; r++) {
				TextOut(hDC, history[i].x, history[i].y + (r * 20), rowStr, lstrlen(rowStr));
			}
		}

		SetTextColor(hDC, RGB(0, 0, 0));
		SetBkColor(hDC, RGB(255, 255, 255));
		SetBkMode(hDC, OPAQUE);

		TextOut(hDC, 10, 500, inputBuf, inputLen);

		if (inputLen == 0) {
			SetCaretPos(10, 500);
		}
		else {
			GetTextExtentPoint32(hDC, inputBuf, inputLen, &size);
			SetCaretPos(10 + size.cx, 500);
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_CHAR:
		if (wParam == 'q' || wParam == 'Q') {
			PostQuitMessage(0);
		}
		else if (wParam == 'r' || wParam == 'R') {
			histCount = 0;
			inputLen = 0;
			inputBuf[0] = 0;
			bShowAll = false;
		}
		else if (wParam == 'a' || wParam == 'A') {
			bShowAll = !bShowAll;
		}
		else if (wParam == VK_BACK) {
			if (inputLen > 0) {
				inputLen--;
				inputBuf[inputLen] = 0;
			}
		}
		else if (wParam == VK_RETURN) {
			int tx, ty, tn, tc;
			if (swscanf_s(inputBuf, L"%d %d %d %d", &tx, &ty, &tn, &tc) == 4) {
				if (histCount < 10) {
					history[histCount].x = tx;
					history[histCount].y = ty;
					history[histCount].n = tn;
					history[histCount].count = tc;
					history[histCount].textColor = RGB(rand() % 256, rand() % 256, rand() % 256);
					history[histCount].bgColor = RGB(rand() % 256, rand() % 256, rand() % 256);
					histCount++;
				}
			}
			inputLen = 0;
			inputBuf[0] = 0;
		}
		else {
			if ((wParam >= '0' && wParam <= '9') || wParam == ' ') {
				if (inputLen < 99) {
					inputBuf[inputLen++] = (TCHAR)wParam;
					inputBuf[inputLen] = 0;
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