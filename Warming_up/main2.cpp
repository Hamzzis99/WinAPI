#include "main2.h"

using namespace std;

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void GameManager::init() {
    random_device rd;
    mt19937 gen(rd());

    for (int i = 0; i < 10; ++i) {
        vector<int> rowNums = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        shuffle(rowNums.begin(), rowNums.end(), gen);
        for (int j = 0; j < 10; ++j) {
            board[i][j] = rowNums[j];
        }
    }

    fill_n(isBlue, 10, false);
    curX = gen() % 10;
    curY = gen() % 10;
    isSumMode = false;
}

int GameManager::getBlueSum() {
    int sum = 0;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            if (isBlue[board[i][j]]) {
                sum += board[i][j];
            }
        }
    }
    return sum;
}

void GameManager::draw() {
    system("cls");

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            int currentNum = board[i][j];

            if (i == curY && j == curX) {
                setColor(COLOR_RED);
                if (isSumMode) {
                    cout << "[" << currentNum + getBlueSum() << "] ";
                }
                else {
                    cout << "[" << currentNum << "] ";
                }
            }
            else if (isBlue[currentNum]) {
                setColor(COLOR_BLUE);
                cout << " " << currentNum << "  ";
            }
            else {
                setColor(COLOR_DEFAULT);
                cout << " " << currentNum << "  ";
            }
            setColor(COLOR_DEFAULT);
        }
        cout << "\n";
    }
    cout << "\n[명령어] w/a/s/d: 이동 | 화살표: 행/열 스왑 | 0~9: 파랑 토글 | Enter: 합계\n";
}

void GameManager::handleInput() {
    while (true) {
        draw();

        // _getch()는 사용자가 키보드를 누르는 즉시 해당 키의 아스키코드 값을 읽어옵니다.
        // cin처럼 Enter 키 입력을 기다릴 필요가 없어 실시간 조작이 가능합니다.
        int key = _getch();
        
        // 방향키 같은 확장 키(특수 키)를 누르면 _getch()는 먼저 224(또는 0)를 반환합니다.
        if (key == 224) {
            // 확장 키임이 확인되면, _getch()를 한 번 더 호출해 실제 눌린 방향키 값을 가져옵니다.
            // (72: 위, 80: 아래, 75: 왼쪽, 77: 오른쪽)
            key = _getch();
            
            if (key == 72 && curY > 0) {
                for (int c = 0; c < 10; ++c) 
                    swap(board[curY][c], board[curY - 1][c]);
                curY--;
            }
            else if (key == 80 && curY < 9) {
                for (int c = 0; c < 10; ++c) 
                    swap(board[curY][c], board[curY + 1][c]);
                curY++;
            }
            else if (key == 75 && curX > 0) {
                for (int r = 0; r < 10; ++r) 
                    swap(board[r][curX], board[r][curX - 1]);
                curX--;
            }
            else if (key == 77 && curX < 9) {
                for (int r = 0; r < 10; ++r) 
                    swap(board[r][curX], board[r][curX + 1]);
                curX++;
            }
            continue;
        }

        switch (key) {
        case 'w': if (curY > 0) curY--; break;
        case 's': if (curY < 9) curY++; break;
        case 'a': if (curX > 0) curX--; break;
        case 'd': if (curX < 9) curX++; break;
        case 13: 
            isSumMode = !isSumMode;
            break;
        case 'r':
            init(); 
            break;
        case 'q':
            return; 
        default:
            if (key >= '0' && key <= '9') {
                int num = key - '0';
                isBlue[num] = !isBlue[num];
            }
            break;
        }
    }
}

int main() {
    GameManager game;
    game.init();
    game.handleInput();

    setColor(COLOR_DEFAULT);
    cout << "\n>> 프로그램이 종료되었습니다.\n";
    return 0;
}