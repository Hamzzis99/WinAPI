#include <windows.h>
#include <tchar.h>
#include <time.h>

#define MAX_LINES 5
#define MAX_CHARS 20

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"windows program 1";
LPCTSTR lpszWindowName = L"windows programing 2-5";

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
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	SIZE size;

	static TCHAR lines[MAX_LINES][MAX_CHARS + 1];
	static int counts[MAX_LINES];
	static bool isDone = false;

	static int startX, startY;
	static COLORREF textColor;

	switch (uMsg) {
	case WM_CREATE:
		srand((unsigned int)time(NULL));

		startX = rand() % 601;
		startY = rand() % 501;
		textColor = RGB(rand() % 256, rand() % 256, rand() % 256);

		for (int i = 0; i < MAX_LINES; i++) counts[i] = 0;
		isDone = false;

		CreateCaret(hWnd, NULL, 5, 15);
		ShowCaret(hWnd);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, textColor);

		for (int i = 0; i < MAX_LINES; i++) {
			if (counts[i] > 0) {
				TextOut(hDC, startX, startY + (i * 20), lines[i], counts[i]);
			}
		}

		if (!isDone) {
			if (counts[0] == 0) {
				SetCaretPos(startX, startY);
			}
			else {
				GetTextExtentPoint32(hDC, lines[0], counts[0], &size);
				SetCaretPos(startX + size.cx, startY);
			}
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_CHAR:
		if (wParam == VK_ESCAPE) {
			startX = rand() % 601;
			startY = rand() % 501;
			textColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			for (int i = 0; i < MAX_LINES; i++) counts[i] = 0;

			if (isDone) {
				isDone = false;
				CreateCaret(hWnd, NULL, 5, 15);
				ShowCaret(hWnd);
			}
		}
		else if (!isDone) {
			if (wParam == VK_BACK) {
				if (counts[0] > 0) {
					counts[0]--;
				}
			}
			else if (wParam == VK_RETURN) {
				if (counts[0] > 0) {
					if (counts[MAX_LINES - 1] > 0) {
						isDone = true;
						HideCaret(hWnd);
						DestroyCaret();
					}
					else {
						for (int i = MAX_LINES - 1; i > 0; i--) {
							for (int j = 0; j < counts[i - 1]; j++) {
								lines[i][j] = lines[i - 1][j];
							}
							counts[i] = counts[i - 1];
						}
						counts[0] = 0;
					}
				}
			}
			else {
				if ((wParam >= 'a' && wParam <= 'z') || (wParam >= 'A' && wParam <= 'Z')) {
					if (counts[0] >= MAX_CHARS) {
						if (counts[MAX_LINES - 1] > 0) {
							isDone = true;
							HideCaret(hWnd);
							DestroyCaret();
						}
						else {
							for (int i = MAX_LINES - 1; i > 0; i--) {
								for (int j = 0; j < counts[i - 1]; j++) {
									lines[i][j] = lines[i - 1][j];
								}
								counts[i] = counts[i - 1];
							}
							counts[0] = 0;
							lines[0][counts[0]++] = (TCHAR)wParam;
						}
					}
					else {
						lines[0][counts[0]++] = (TCHAR)wParam;
						if (counts[0] == MAX_CHARS && counts[MAX_LINES - 1] > 0) {
							isDone = true;
							HideCaret(hWnd);
							DestroyCaret();
						}
					}
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