#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define PI 3.14159265358979323846

/* ===== 도형 종류 ===== */
#define SHAPE_CIRCLE    0
#define SHAPE_HOURGLASS 1
#define SHAPE_PENTAGON  2
#define SHAPE_PIE       3

/* ===== 위치 (시계방향) ===== */
#define POS_TOP    0
#define POS_RIGHT  1
#define POS_BOTTOM 2
#define POS_LEFT   3

/* ===== 전역 상태 ===== */
int g_shapeAtPos[4];            // 각 위치에 있는 도형 종류
int g_selectedType;             // 현재 선택된 도형 종류
COLORREF g_origColor[4];        // 도형별 원래 색상 (인덱스 = 도형 종류)
COLORREF g_dispColor[4];        // 도형별 현재 표시 색상
BOOL g_keyPressed;              // c/s/p/e 키가 눌려있는지
RECT g_centerRect;              // 중앙 사각형 좌표
RECT g_clientRect;              // 클라이언트 영역

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/* ===== 랜덤 색상 생성 ===== */
COLORREF RandomColor(void)
{
    return RGB(rand() % 256, rand() % 256, rand() % 256);
}

/* ===== 초기 상태 설정 ===== */
void InitState(void)
{
    // 초기 배치: 위=모래시계, 오른쪽=오각형, 아래=파이, 왼쪽=원
    g_shapeAtPos[POS_TOP] = SHAPE_HOURGLASS;
    g_shapeAtPos[POS_RIGHT] = SHAPE_PENTAGON;
    g_shapeAtPos[POS_BOTTOM] = SHAPE_PIE;
    g_shapeAtPos[POS_LEFT] = SHAPE_CIRCLE;

    // 각 도형의 원래 색상을 랜덤으로 설정
    for (int i = 0; i < 4; i++) {
        g_origColor[i] = RandomColor();
        g_dispColor[i] = g_origColor[i];
    }

    // 랜덤으로 하나 선택
    g_selectedType = g_shapeAtPos[rand() % 4];
    g_keyPressed = FALSE;
}

/* ===== 레이아웃 계산 ===== */
void CalcLayout(HWND hWnd)
{
    GetClientRect(hWnd, &g_clientRect);
    int cw = g_clientRect.right;
    int ch = g_clientRect.bottom;

    int rw = cw * 30 / 100;
    int rh = ch * 30 / 100;
    g_centerRect.left = (cw - rw) / 2;
    g_centerRect.top = (ch - rh) / 2;
    g_centerRect.right = g_centerRect.left + rw;
    g_centerRect.bottom = g_centerRect.top + rh;
}

/* ===== 위치 회전/교환 함수 ===== */

// ← 키: 시계방향 회전
void RotateClockwise(void)
{
    int temp = g_shapeAtPos[POS_TOP];
    g_shapeAtPos[POS_TOP] = g_shapeAtPos[POS_LEFT];
    g_shapeAtPos[POS_LEFT] = g_shapeAtPos[POS_BOTTOM];
    g_shapeAtPos[POS_BOTTOM] = g_shapeAtPos[POS_RIGHT];
    g_shapeAtPos[POS_RIGHT] = temp;
    g_selectedType = g_shapeAtPos[POS_TOP];
}

// → 키: 반시계방향 회전
void RotateCounterClockwise(void)
{
    int temp = g_shapeAtPos[POS_TOP];
    g_shapeAtPos[POS_TOP] = g_shapeAtPos[POS_RIGHT];
    g_shapeAtPos[POS_RIGHT] = g_shapeAtPos[POS_BOTTOM];
    g_shapeAtPos[POS_BOTTOM] = g_shapeAtPos[POS_LEFT];
    g_shapeAtPos[POS_LEFT] = temp;
    g_selectedType = g_shapeAtPos[POS_TOP];
}

// ↑ 키: 위아래 교환
void SwapTopBottom(void)
{
    int temp = g_shapeAtPos[POS_TOP];
    g_shapeAtPos[POS_TOP] = g_shapeAtPos[POS_BOTTOM];
    g_shapeAtPos[POS_BOTTOM] = temp;
    g_selectedType = g_shapeAtPos[POS_TOP];
}

// ↓ 키: 좌우 교환
void SwapLeftRight(void)
{
    int temp = g_shapeAtPos[POS_LEFT];
    g_shapeAtPos[POS_LEFT] = g_shapeAtPos[POS_RIGHT];
    g_shapeAtPos[POS_RIGHT] = temp;
}

/* ==========================================================
 *  도형 그리기 함수들
 *  (GDI 사용 순서: CreatePen/Brush → SelectObject → 그리기
 *                  → SelectObject 복구 → DeleteObject)
 * ========================================================== */

 /* ----- 원 그리기 ----- */
void DrawCircleShape(HDC hDC, int cx, int cy, int r, COLORREF color)
{
    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;

    hPen = CreatePen(PS_SOLID, 2, color);
    oldPen = (HPEN)SelectObject(hDC, hPen);
    hBrush = CreateSolidBrush(color);
    oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

    Ellipse(hDC, cx - r, cy - r, cx + r, cy + r);

    SelectObject(hDC, oldPen);
    SelectObject(hDC, oldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

/* ----- 타원 그리기 (원의 변형 → 중앙용) ----- */
void DrawEllipseShape(HDC hDC, int cx, int cy, int rw, int rh, COLORREF color)
{
    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;

    hPen = CreatePen(PS_SOLID, 2, color);
    oldPen = (HPEN)SelectObject(hDC, hPen);
    hBrush = CreateSolidBrush(color);
    oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

    Ellipse(hDC, cx - rw, cy - rh, cx + rw, cy + rh);

    SelectObject(hDC, oldPen);
    SelectObject(hDC, oldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

/* ----- 모래시계 그리기 (위아래 삼각형 2개) ----- */
void DrawHourglass(HDC hDC, int cx, int cy, int size, COLORREF color)
{
    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;
    POINT pts[3];

    hPen = CreatePen(PS_SOLID, 2, color);
    oldPen = (HPEN)SelectObject(hDC, hPen);
    hBrush = CreateSolidBrush(color);
    oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

    // 위쪽 삼각형 (역삼각형)
    pts[0].x = cx - size / 2;  pts[0].y = cy - size / 2;
    pts[1].x = cx + size / 2;  pts[1].y = cy - size / 2;
    pts[2].x = cx;             pts[2].y = cy;
    Polygon(hDC, pts, 3);

    // 아래쪽 삼각형
    pts[0].x = cx;             pts[0].y = cy;
    pts[1].x = cx - size / 2;  pts[1].y = cy + size / 2;
    pts[2].x = cx + size / 2;  pts[2].y = cy + size / 2;
    Polygon(hDC, pts, 3);

    SelectObject(hDC, oldPen);
    SelectObject(hDC, oldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

/* ----- 나비모양 그리기 (모래시계의 변형 → 중앙용, 좌우 삼각형) ----- */
void DrawButterfly(HDC hDC, int cx, int cy, int size, COLORREF color)
{
    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;
    POINT pts[3];

    hPen = CreatePen(PS_SOLID, 2, color);
    oldPen = (HPEN)SelectObject(hDC, hPen);
    hBrush = CreateSolidBrush(color);
    oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

    // 왼쪽 삼각형
    pts[0].x = cx - size / 2;  pts[0].y = cy - size / 2;
    pts[1].x = cx;             pts[1].y = cy;
    pts[2].x = cx - size / 2;  pts[2].y = cy + size / 2;
    Polygon(hDC, pts, 3);

    // 오른쪽 삼각형
    pts[0].x = cx + size / 2;  pts[0].y = cy - size / 2;
    pts[1].x = cx;             pts[1].y = cy;
    pts[2].x = cx + size / 2;  pts[2].y = cy + size / 2;
    Polygon(hDC, pts, 3);

    SelectObject(hDC, oldPen);
    SelectObject(hDC, oldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

/* ----- 오각형 그리기 (flipped=TRUE이면 뒤집힌 오각형) ----- */
void DrawPentagon(HDC hDC, int cx, int cy, int r, COLORREF color, BOOL flipped)
{
    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;
    POINT pts[5];
    double startAngle = flipped ? PI / 2.0 : -PI / 2.0;

    hPen = CreatePen(PS_SOLID, 2, color);
    oldPen = (HPEN)SelectObject(hDC, hPen);
    hBrush = CreateSolidBrush(color);
    oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

    for (int i = 0; i < 5; i++) {
        double angle = startAngle + i * 2.0 * PI / 5.0;
        pts[i].x = cx + (int)(r * cos(angle));
        pts[i].y = cy + (int)(r * sin(angle));
    }
    Polygon(hDC, pts, 5);

    SelectObject(hDC, oldPen);
    SelectObject(hDC, oldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

/* ----- 파이 그리기 (remaining=TRUE이면 남은 부분만) ----- */
void DrawPieShape(HDC hDC, int cx, int cy, int r, COLORREF color, BOOL remaining)
{
    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;
    int left = cx - r, top = cy - r, right = cx + r, bottom = cy + r;

    double angle1 = 40.0 * PI / 180.0;
    double angle2 = -40.0 * PI / 180.0;

    int xS, yS, xE, yE;

    hPen = CreatePen(PS_SOLID, 2, color);
    oldPen = (HPEN)SelectObject(hDC, hPen);
    hBrush = CreateSolidBrush(color);
    oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

    if (!remaining) {
        // 팩맨 모양: 입 벌린 부분 빼고 그림
        xS = cx + (int)(r * cos(angle1));
        yS = cy - (int)(r * sin(angle1));
        xE = cx + (int)(r * cos(angle2));
        yE = cy - (int)(r * sin(angle2));
    }
    else {
        // 남은 조각: 입 벌린 부분만 그림
        xS = cx + (int)(r * cos(angle2));
        yS = cy - (int)(r * sin(angle2));
        xE = cx + (int)(r * cos(angle1));
        yE = cy - (int)(r * sin(angle1));
    }
    Pie(hDC, left, top, right, bottom, xS, yS, xE, yE);

    SelectObject(hDC, oldPen);
    SelectObject(hDC, oldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

/* ===== 위치별 도형 그리기 ===== */
void DrawShapeAtPos(HDC hDC, int pos)
{
    int cw = g_clientRect.right;
    int ch = g_clientRect.bottom;
    int shapeType = g_shapeAtPos[pos];
    COLORREF color = g_dispColor[shapeType];
    int size;
    int cx, cy;

    // 도형 크기: 클라이언트 크기에 비례
    size = min(cw, ch) / 8;
    if (size < 20) size = 20;

    // 위치별 중심 좌표 계산
    switch (pos) {
    case POS_TOP:
        cx = cw / 2;
        cy = g_centerRect.top / 2;
        break;
    case POS_BOTTOM:
        cx = cw / 2;
        cy = (g_centerRect.bottom + ch) / 2;
        break;
    case POS_LEFT:
        cx = g_centerRect.left / 2;
        cy = ch / 2;
        break;
    case POS_RIGHT:
        cx = (g_centerRect.right + cw) / 2;
        cy = ch / 2;
        break;
    }

    // 도형 종류에 따라 그리기
    switch (shapeType) {
    case SHAPE_CIRCLE:
        DrawCircleShape(hDC, cx, cy, size / 2, color);
        break;
    case SHAPE_HOURGLASS:
        DrawHourglass(hDC, cx, cy, size, color);
        break;
    case SHAPE_PENTAGON:
        DrawPentagon(hDC, cx, cy, size / 2, color, FALSE);
        break;
    case SHAPE_PIE:
        DrawPieShape(hDC, cx, cy, size / 2, color, FALSE);
        break;
    }
}

/* ===== 중앙 사각형 안에 변형된 도형 그리기 ===== */
void DrawCenterShape(HDC hDC)
{
    int cx = (g_centerRect.left + g_centerRect.right) / 2;
    int cy = (g_centerRect.top + g_centerRect.bottom) / 2;
    int w = (g_centerRect.right - g_centerRect.left) / 2 - 15;
    int h = (g_centerRect.bottom - g_centerRect.top) / 2 - 15;
    int size = min(w, h);
    COLORREF color = g_dispColor[g_selectedType];

    switch (g_selectedType) {
    case SHAPE_CIRCLE:
        // 원 → 타원으로 변형
        DrawEllipseShape(hDC, cx, cy, w, h * 2 / 3, color);
        break;
    case SHAPE_HOURGLASS:
        // 모래시계 → 나비모양으로 변형
        DrawButterfly(hDC, cx, cy, size, color);
        break;
    case SHAPE_PENTAGON:
        // 오각형 → 위아래 뒤집힌 오각형
        DrawPentagon(hDC, cx, cy, size * 2 / 3, color, TRUE);
        break;
    case SHAPE_PIE:
        // 파이 → 남은 부분
        DrawPieShape(hDC, cx, cy, size * 2 / 3, color, TRUE);
        break;
    }
}

/* ===== 중앙 사각형 테두리 그리기 ===== */
void DrawCenterBorder(HDC hDC)
{
    HPEN hPen, oldPen;
    HBRUSH oldBrush;

    hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 200));
    oldPen = (HPEN)SelectObject(hDC, hPen);
    oldBrush = (HBRUSH)SelectObject(hDC, (HBRUSH)GetStockObject(NULL_BRUSH));

    Rectangle(hDC, g_centerRect.left, g_centerRect.top,
        g_centerRect.right, g_centerRect.bottom);

    SelectObject(hDC, oldBrush);
    SelectObject(hDC, oldPen);
    DeleteObject(hPen);
}

/* ===== WinMain ===== */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    static TCHAR szAppName[] = TEXT("Practice2_11");
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
        TEXT("실습 2-11: 도형 선택/회전/색상 (c/s/p/e:선택, ←→↑↓:이동, q:종료)"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 700, 600,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

/* ===== 윈도우 프로시저 함수 ===== */
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hDC;
    PAINTSTRUCT ps;

    switch (iMsg) {

    case WM_CREATE:                             // 윈도우 생성 시 초기화
        CalcLayout(hWnd);
        InitState();
        return 0;

    case WM_SIZE:                               // 윈도우 크기 변경
        CalcLayout(hWnd);
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;

    case WM_KEYDOWN:                            // 키보드 누를 때
        switch (wParam) {

            /* --- 도형 선택 + 색상 변경 --- */
        case 'C':                               // c: 원 선택
            g_selectedType = SHAPE_CIRCLE;
            g_dispColor[SHAPE_CIRCLE] = RandomColor();
            g_keyPressed = TRUE;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case 'S':                               // s: 모래시계 선택
            g_selectedType = SHAPE_HOURGLASS;
            g_dispColor[SHAPE_HOURGLASS] = RandomColor();
            g_keyPressed = TRUE;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case 'P':                               // p: 오각형 선택
            g_selectedType = SHAPE_PENTAGON;
            g_dispColor[SHAPE_PENTAGON] = RandomColor();
            g_keyPressed = TRUE;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case 'E':                               // e: 파이 선택
            g_selectedType = SHAPE_PIE;
            g_dispColor[SHAPE_PIE] = RandomColor();
            g_keyPressed = TRUE;
            InvalidateRect(hWnd, NULL, TRUE);
            break;

            /* --- 도형 위치 변경 --- */
        case VK_LEFT:                           // ←: 시계방향 회전
            RotateClockwise();
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case VK_RIGHT:                          // →: 반시계방향 회전
            RotateCounterClockwise();
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case VK_UP:                             // ↑: 위아래 교환
            SwapTopBottom();
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case VK_DOWN:                           // ↓: 좌우 교환
            SwapLeftRight();
            InvalidateRect(hWnd, NULL, TRUE);
            break;

            /* --- 프로그램 종료 --- */
        case 'Q':                               // q/Q: 종료
            DestroyWindow(hWnd);
            break;
        }
        return 0;

    case WM_KEYUP:                              // 키보드 뗄 때
        switch (wParam) {
        case 'C': case 'S': case 'P': case 'E':
            // 원래 색상으로 복구
            for (int i = 0; i < 4; i++)
                g_dispColor[i] = g_origColor[i];
            g_keyPressed = FALSE;
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
        return 0;

    case WM_PAINT:                              // 화면 그리기
        hDC = BeginPaint(hWnd, &ps);

        // 1) 중앙 사각형 테두리
        DrawCenterBorder(hDC);

        // 2) 상하좌우 4개 도형
        DrawShapeAtPos(hDC, POS_TOP);
        DrawShapeAtPos(hDC, POS_RIGHT);
        DrawShapeAtPos(hDC, POS_BOTTOM);
        DrawShapeAtPos(hDC, POS_LEFT);

        // 3) 중앙에 선택된 도형의 변형 모양
        DrawCenterShape(hDC);

        EndPaint(hWnd, &ps);
        return 0;

    case WM_DESTROY:                            // 프로그램 종료
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, iMsg, wParam, lParam);
}