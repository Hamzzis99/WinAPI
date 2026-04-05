#include <windows.h>
#include <tchar.h>

#define MAX_LINES 10
#define MAX_CHARS 30

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"windows program 1";
LPCTSTR lpszWindowName = L"Insert -> F9";

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

	static int cx = 0;
	static int cy = 0;
	static bool bInsertMode = false;

	static int f1State = 0;
	static bool bF2 = false;
	static bool bF3 = false;
	static bool bF4 = false;
	static bool bF5 = false;

	switch (uMsg) {
	case WM_CREATE:
		for (int i = 0; i < MAX_LINES; i++) counts[i] = 0;
		CreateCaret(hWnd, NULL, 5, 15);
		SetCaretPos(10, 10);
		ShowCaret(hWnd);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		SetBkMode(hDC, TRANSPARENT);

		for (int i = 0; i < MAX_LINES; i++) {
			if (counts[i] > 0) {
				TCHAR f5Str[MAX_CHARS + 1];
				if (bF5) {
					int freq[256] = { 0 };
					int maxFreq = 0;
					TCHAR maxChar = 0;
					for (int j = 0; j < counts[i]; j++) {
						if (lines[i][j] != ' ') {
							freq[lines[i][j]]++;
							if (freq[lines[i][j]] > maxFreq) {
								maxFreq = freq[lines[i][j]];
								maxChar = lines[i][j];
							}
						}
					}
					for (int j = 0; j < counts[i]; j++) {
						f5Str[j] = (lines[i][j] == maxChar && maxChar != 0) ? '@' : lines[i][j];
					}
				}
				else {
					for (int j = 0; j < counts[i]; j++) f5Str[j] = lines[i][j];
				}

				TCHAR drawStr[500];
				int dLen = 0;
				bool inWord = false;

				for (int j = 0; j < counts[i]; j++) {
					TCHAR c = f5Str[j];

					if (bF4 && c == ' ') continue;

					if (bF2 && c >= '0' && c <= '9') {
						if (j == 0 || !(f5Str[j - 1] >= '0' && f5Str[j - 1] <= '9')) {
							drawStr[dLen++] = '*'; drawStr[dLen++] = '*'; drawStr[dLen++] = '*'; drawStr[dLen++] = '*';
						}
					}

					if (bF3) {
						if (c != ' ' && !inWord) {
							drawStr[dLen++] = '(';
							inWord = true;
						}
						else if (c == ' ' && inWord) {
							drawStr[dLen++] = ')';
							inWord = false;
						}
					}

					if (bF3) {
						if (c >= 'a' && c <= 'z') c -= 32;
					}
					else if (bF4) {
						if (c >= 'A' && c <= 'Z') c += 32;
					}
					else if (f1State == 1) {
						if (c >= 'a' && c <= 'z') c -= 32;
					}
					else if (f1State == 2) {
						if (c >= 'A' && c <= 'Z') c += 32;
					}

					drawStr[dLen++] = c;
				}
				if (bF3 && inWord) drawStr[dLen++] = ')';

				TextOut(hDC, 10, 10 + (i * 20), drawStr, dLen);
			}
		}

		{
			TCHAR f5Str_cy[MAX_CHARS + 1];
			if (bF5) {
				int freq[256] = { 0 };
				int maxFreq = 0;
				TCHAR maxChar = 0;
				for (int j = 0; j < counts[cy]; j++) {
					if (lines[cy][j] != ' ') {
						freq[lines[cy][j]]++;
						if (freq[lines[cy][j]] > maxFreq) {
							maxFreq = freq[lines[cy][j]];
							maxChar = lines[cy][j];
						}
					}
				}
				for (int j = 0; j < cx; j++) {
					f5Str_cy[j] = (lines[cy][j] == maxChar && maxChar != 0) ? '@' : lines[cy][j];
				}
			}
			else {
				for (int j = 0; j < cx; j++) f5Str_cy[j] = lines[cy][j];
			}

			TCHAR caretStr[500];
			int cLen = 0;
			bool inWord_cy = false;

			for (int j = 0; j < cx; j++) {
				TCHAR c = f5Str_cy[j];

				if (bF4 && c == ' ') continue;

				if (bF2 && c >= '0' && c <= '9') {
					if (j == 0 || !(f5Str_cy[j - 1] >= '0' && f5Str_cy[j - 1] <= '9')) {
						caretStr[cLen++] = '*'; caretStr[cLen++] = '*'; caretStr[cLen++] = '*'; caretStr[cLen++] = '*';
					}
				}

				if (bF3) {
					if (c != ' ' && !inWord_cy) {
						caretStr[cLen++] = '(';
						inWord_cy = true;
					}
					else if (c == ' ' && inWord_cy) {
						caretStr[cLen++] = ')';
						inWord_cy = false;
					}
				}

				if (bF3) {
					if (c >= 'a' && c <= 'z') c -= 32;
				}
				else if (bF4) {
					if (c >= 'A' && c <= 'Z') c += 32;
				}
				else if (f1State == 1) {
					if (c >= 'a' && c <= 'z') c -= 32;
				}
				else if (f1State == 2) {
					if (c >= 'A' && c <= 'Z') c += 32;
				}

				caretStr[cLen++] = c;
			}

			if (cLen == 0) {
				SetCaretPos(10, 10 + (cy * 20));
			}
			else {
				GetTextExtentPoint32(hDC, caretStr, cLen, &size);
				SetCaretPos(10 + size.cx, 10 + (cy * 20));
			}
		}

		EndPaint(hWnd, &ps);
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			if (cx > 0) cx--;
			break;
		case VK_RIGHT:
			if (cx < counts[cy]) cx++;
			break;
		case VK_UP:
			if (cy > 0) {
				cy--;
				if (cx > counts[cy]) cx = counts[cy];
			}
			break;
		case VK_DOWN:
			if (cy < MAX_LINES - 1) {
				if (counts[cy + 1] > 0) {
					cy++;
					if (cx > counts[cy]) cx = counts[cy];
				}
				else if (counts[cy] > 0) {
					cy++;
					cx = 0;
				}
			}
			break;
		case VK_HOME:
			cx = 0;
			break;
		case VK_END:
			cx = counts[cy];
			if (cx == MAX_CHARS && cy < MAX_LINES - 1) {
				cy++;
				cx = 0;
			}
			break;
		case VK_PRIOR:
			cy = (cy - 3 + MAX_LINES) % MAX_LINES;
			if (cx > counts[cy]) cx = counts[cy];
			break;
		case VK_NEXT:
			cy = (cy + 3) % MAX_LINES;
			if (cx > counts[cy]) cx = counts[cy];
			break;
		case VK_F9:
			bInsertMode = !bInsertMode;
			break;
		case VK_DELETE:
			if (cx < counts[cy]) {
				int wStart = cx;
				int wEnd = cx;
				if (lines[cy][cx] != ' ') {
					while (wStart > 0 && lines[cy][wStart - 1] != ' ') wStart--;
					while (wEnd < counts[cy] && lines[cy][wEnd] != ' ') wEnd++;
				}
				else {
					while (wEnd < counts[cy] && lines[cy][wEnd] == ' ') wEnd++;
				}

				int len = wEnd - wStart;
				for (int i = wEnd; i < counts[cy]; i++) {
					lines[cy][i - len] = lines[cy][i];
				}
				counts[cy] -= len;
				if (cx > counts[cy]) cx = counts[cy];
			}
			break;
		case VK_F1:
			f1State = (f1State + 1) % 3;
			break;
		case VK_F2:
			bF2 = !bF2;
			break;
		case VK_F3:
			bF3 = !bF3;
			break;
		case VK_F4:
			bF4 = !bF4;
			break;
		case VK_F5:
			bF5 = !bF5;
			break;
		case VK_F6:
		{
			int activeLines = 0;
			for (int i = MAX_LINES - 1; i >= 0; i--) {
				if (counts[i] > 0) {
					activeLines = i + 1;
					break;
				}
			}

			if (activeLines > 1) {
				TCHAR tempLine[MAX_CHARS + 1];
				int tempCount = counts[0];

				for (int j = 0; j < tempCount; j++) tempLine[j] = lines[0][j];

				for (int i = 0; i < activeLines - 1; i++) {
					counts[i] = counts[i + 1];
					for (int j = 0; j < counts[i]; j++) lines[i][j] = lines[i + 1][j];
				}

				counts[activeLines - 1] = tempCount;
				for (int j = 0; j < tempCount; j++) lines[activeLines - 1][j] = tempLine[j];
			}
			break;
		}
		case VK_F7:
			for (int i = 0; i < MAX_LINES; i++) {
				for (int j = 0; j < counts[i]; j++) {
					if (lines[i][j] >= '0' && lines[i][j] < '9') lines[i][j]++;
					else if (lines[i][j] == '9') lines[i][j] = '0';
				}
			}
			break;
		case VK_F8:
			for (int i = 0; i < MAX_LINES; i++) {
				for (int j = 0; j < counts[i]; j++) {
					if (lines[i][j] > '0' && lines[i][j] <= '9') lines[i][j]--;
					else if (lines[i][j] == '0') lines[i][j] = '9';
				}
			}
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_CHAR:
		if (wParam == VK_ESCAPE) {
			cy = 0;
			cx = 0;
			for (int i = 0; i < MAX_LINES; i++) counts[i] = 0;
			f1State = 0;
			bF2 = false; bF3 = false; bF4 = false; bF5 = false;
		}
		else if (wParam == VK_BACK) {
			if (cx > 0) {
				for (int i = cx; i < counts[cy]; i++) {
					lines[cy][i - 1] = lines[cy][i];
				}
				counts[cy]--;
				cx--;
			}
			else if (cy > 0) {
				cy--;
				cx = counts[cy];
			}
		}
		else if (wParam == VK_RETURN) {
			cy++;
			cx = 0;
			if (cy >= MAX_LINES) {
				cy = 0;
			}
		}
		else if (wParam == VK_TAB) {
			// Tab은 무조건 공백을 4번 삽입하고 연쇄적으로 밀어냅니다.
			for (int k = 0; k < 4; k++) {
				TCHAR carry = ' '; // 삽입할 문자 (공백)
				int curr_x = cx;

				// 현재 줄부터 아래로 내려가면서 글자들을 밀어냄
				for (int curr_y = cy; curr_y < MAX_LINES; curr_y++) {
					if (counts[curr_y] < MAX_CHARS) {
						// 줄에 공간이 있으면 밀고 끝냄
						for (int i = counts[curr_y]; i > curr_x; i--) {
							lines[curr_y][i] = lines[curr_y][i - 1];
						}
						lines[curr_y][curr_x] = carry;
						counts[curr_y]++;
						break;
					}
					else {
						// 꽉 차 있으면 맨 끝 글자를 다음 줄로 넘김
						TCHAR next_carry = lines[curr_y][MAX_CHARS - 1];
						for (int i = MAX_CHARS - 1; i > curr_x; i--) {
							lines[curr_y][i] = lines[curr_y][i - 1];
						}
						lines[curr_y][curr_x] = carry;
						carry = next_carry; // 밀려난 글자를 다음 줄의 첫 글자로 삽입 준비
						curr_x = 0;         // 다음 줄은 항상 0번 인덱스부터 밀어냄
					}
				}

				cx++;
				if (cx >= MAX_CHARS) {
					cy++;
					cx = 0;
					if (cy >= MAX_LINES) cy = 0;
				}
			}
		}
		else if (wParam >= 32) {
			if (bInsertMode) {
				TCHAR carry = (TCHAR)wParam;
				int curr_x = cx;

				// 현재 줄부터 아래로 내려가면서 연쇄적으로 글자들을 밀어냄
				for (int curr_y = cy; curr_y < MAX_LINES; curr_y++) {
					if (counts[curr_y] < MAX_CHARS) {
						// 줄에 공간이 있으면 밀어내고 끝냄
						for (int i = counts[curr_y]; i > curr_x; i--) {
							lines[curr_y][i] = lines[curr_y][i - 1];
						}
						lines[curr_y][curr_x] = carry;
						counts[curr_y]++;
						break; // 연쇄 이동 종료
					}
					else {
						// 30자가 꽉 찼다면 마지막 문자를 임시 저장하고 넘김
						TCHAR next_carry = lines[curr_y][MAX_CHARS - 1];
						for (int i = MAX_CHARS - 1; i > curr_x; i--) {
							lines[curr_y][i] = lines[curr_y][i - 1];
						}
						lines[curr_y][curr_x] = carry;
						carry = next_carry; // 밀려난 문자를 다음 줄 첫 번째 칸으로 삽입
						curr_x = 0;         // 다음 줄부터는 항상 맨 앞(0)부터 밀어냄
					}
				}
				cx++;
			}
			else {
				// 덮어쓰기 모드
				if (cx < MAX_CHARS) {
					lines[cy][cx] = (TCHAR)wParam;
					if (cx == counts[cy]) counts[cy]++;
					cx++;
				}
			}

			// 캐럿 위치가 30자를 넘어가면 다음 줄로 이동
			if (cx >= MAX_CHARS) {
				cy++;
				cx = 0;
				if (cy >= MAX_LINES) {
					cy = 0;
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