#include "main.h"

// 예외처리를 많이 할 것이다 (잘못된 명령어 또는 이미 입력한 번호)

using namespace std;

// 3번 qsort 비교 함수 구현. (중복 제거)
int compareAlpha(const void* a, const void* b) {
    unsigned char charA = std::tolower(*(const unsigned char*)a);
    unsigned char charB = std::tolower(*(const unsigned char*)b);
    if (charA < charB) return -1;
    if (charA > charB) return 1;
    return 0;
}

// 4번 qsort 비교 함수 구현 (오름차순)
int compareAsc(const void* a, const void* b) {
    return (*(const char*)a - *(const char*)b);
}

// 솔트 번호로부터 정렬 해주는 기능.
string soltNumber(std::string_view text) {
    std::map<char, int> countMap;
    for (char c : text) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            countMap[c]++;
        }
    }
    std::string result;
    for (const auto& [alphabet, count] : countMap) {
        result += alphabet;
        result += std::to_string(count);
    }
    return result;
}

// 본격적인 정렬.
void sortInPlace(string& text) {
    ranges::sort(text);
}

int main()
{
	string mySentence; // 문장을 입력할 수 있는 공간이며 이곳은 메모리에 저장해둘 것이다.
	string command; // 명령어 입력 칸.
	string inputBuffer; // 명령어 입력 버퍼

	cout << "Enter the sentence : ";
	getline(cin, mySentence); // 문장을 입력받는다. (띄어쓰기 포함)

    string currentSentence = mySentence;
    bool isTransformed = false; // [추가] 3, 4번 변형 상태를 추적하는 플래그
	cout << currentSentence << endl; // 입력받은 문장을 출력한다.
		

    while (true) {
        std::cout << "\n--------------------------------------";
        std::cout << "\n[Current Sentence]: " << currentSentence;
        std::cout << "\n(a~z, 0, 3, 5) ";

        if (!std::getline(std::cin, inputBuffer) || inputBuffer.empty()) continue;

        char command = inputBuffer[0];

        if (command == '0') break;
        if (command == '5') {
            currentSentence = mySentence;
            std::cout << ">> Default.\n";
            continue;
        }
        // 3: 알파벳순 정렬 및 개수 표현
        if (command == '3') {
            if (isTransformed) {
                currentSentence = mySentence; // 원래대로 복원
                isTransformed = false;
                cout << ">> Restored to Original.\n";
            }
            else {
                currentSentence = soltNumber(currentSentence); // 3번 실행
                isTransformed = true;
                cout << ">> Processed with soltNumber.\n";
            }
            continue;
        }

        // 4번: 정렬
        if (command == '4') {
            if (isTransformed) {
                currentSentence = mySentence; // 원래대로 복원
                isTransformed = false;
                cout << ">> Restored to Original.\n";
            }
            else {
                sortInPlace(currentSentence); // 4번 실행
                isTransformed = true;
                cout << ">> Sorted In-Place.\n";
            }
            continue;
        }

        // a~z 반전 코드
        if (std::isalpha(static_cast<unsigned char>(command))) {
            // C++26: ranges::to와 views::transform의 조합
            currentSentence = currentSentence
                | std::views::transform([command](char c) {
                // 입력한 문자와 일치하는지 확인 (대소문자 구분 없이)
                if (std::tolower(static_cast<unsigned char>(c)) ==
                    std::tolower(static_cast<unsigned char>(command))) {

                    // 현재가 소문자면 대문자로, 대문자면 소문자로 반전(Toggle)
                    return std::islower(static_cast<unsigned char>(c))
                        ? (char)std::toupper(static_cast<unsigned char>(c))
                        : (char)std::tolower(static_cast<unsigned char>(c));
                }
                return c; // 일치하지 않는 문자는 그대로 통과
                    })
                | std::ranges::to<std::string>();

            std::cout << ">> '" << command << "' Changed.\n";
            std::cout << "\n[Current Sentence]: " << currentSentence;
        }
    }
	
	return 0;
}