#include "main.h"

using namespace std;

// 문장 입력 칸
void Sentence(string& text) {
    while (!text.empty() && text.back() == '.') text.pop_back();
    if (text.length() >= 40) text.resize(39);
    text += '.';
}

// [1, 2번] 공백 개수 증감 (이제 vector를 직접 조작합니다!)
void SpacesManager(vector<int>& spaces, int delta) {
    for (int& s : spaces) {
        s = std::clamp(s + delta, 0, 5);
    }
}

// [3번] 알파벳 빈도 분석 (CharCmp 적용)
string SortNumber(string_view text) {
    map<char, int, CharCmp> countMap;
    for (char c : text) {
        if (isalpha(static_cast<unsigned char>(c))) countMap[c]++;
    }
    string result;
    for (const auto& [alphabet, count] : countMap) {
        result += alphabet;
        result += to_string(count);
    }
    Sentence(result);
    return result;
}

// [4번] 단어 길이순 오름차순 정렬
string SortByWordLength(string_view text) {
    string temp(text);
    while (!temp.empty() && temp.back() == '.') temp.pop_back();

    stringstream ss(temp);
    vector<string> words;
    string w;
    while (ss >> w) words.push_back(w);

    ranges::stable_sort(words, ranges::less{}, &string::length);

    string result;
    for (size_t i = 0; i < words.size(); ++i) {
        result += words[i];
        if (i + 1 < words.size()) result += " ";
    }
    Sentence(result);
    return result;
}

int main() {
    string mySentence;
    cout << "Enter the sentence : ";
    getline(cin, mySentence);

    Sentence(mySentence);

    // 단어와 공백을 분리해서 기억하는 창고
    vector<string> words;
    vector<int> spaces;

    auto parseOriginal = [&]() {
        words.clear();
        spaces.clear();
        string tempWord = "";
        int tempSpace = 0;
        for (char c : mySentence) {
            if (c == ' ') {
                if (!tempWord.empty()) {
                    words.push_back(tempWord);
                    tempWord = "";
                }
                tempSpace++;
            }
            else if (c == '.') continue;
            else {
                if (tempSpace > 0 && !words.empty()) {
                    spaces.push_back(tempSpace);
                    tempSpace = 0;
                }
                tempWord += c;
            }
        }
        if (!tempWord.empty()) words.push_back(tempWord);
        };

    auto buildSentence = [&]() {
        string result = "";
        for (size_t i = 0; i < words.size(); ++i) {
            result += words[i];
            if (i < spaces.size()) {
                result.append(spaces[i], ' ');
            }
        }
        Sentence(result);
        return result;
        };

    parseOriginal();
    string currentSentence = buildSentence();
    bool isTransformed = false;

    while (true) {
        cout << "\n--------------------------------------";
        cout << "\n[Current Sentence]: " << currentSentence;
        cout << "\n(a~z, 1, 2, 3, 4, 5, 0): ";

        string inputBuffer;
        if (!getline(cin, inputBuffer) || inputBuffer.empty()) continue;
        char command = inputBuffer[0];

        if (command == '0') break;

        if (command == '5') {
            parseOriginal();
            currentSentence = buildSentence();
            isTransformed = false;
            cout << ">> Default.\n";
            continue;
        }

        // 1번: 공백 줄이기 (SpacesManager 사용)
        if (command == '1') {
            SpacesManager(spaces, -1);
            currentSentence = buildSentence();
            isTransformed = false;
            continue;
        }

        // 2번: 공백 늘리기 (SpacesManager 사용)
        if (command == '2') {
            SpacesManager(spaces, 1);
            currentSentence = buildSentence();
            isTransformed = false;
            continue;
        }

        // 3번: 알파벳 빈도 분석
        if (command == '3') {
            if (isTransformed) {
                currentSentence = buildSentence();
                isTransformed = false;
            }
            else {
                currentSentence = SortNumber(buildSentence());
                isTransformed = true;
            }
            continue;
        }

        // 4번: 단어 길이순 정렬
        if (command == '4') {
            if (isTransformed) {
                currentSentence = buildSentence();
                isTransformed = false;
            }
            else {
                currentSentence = SortByWordLength(buildSentence());
                isTransformed = true;
            }
            continue;
        }

        // a~z: 대소문자 반전
        if (isalpha(static_cast<unsigned char>(command))) {
            char targetCmd = tolower(static_cast<unsigned char>(command));

            for (auto& w : words) {
                w = w | views::transform([targetCmd](char c) {
                    if (tolower(static_cast<unsigned char>(c)) == targetCmd) {
                        return islower(static_cast<unsigned char>(c))
                            ? (char)toupper(static_cast<unsigned char>(c))
                            : (char)tolower(static_cast<unsigned char>(c));
                    }
                    return c;
                    }) | ranges::to<string>();
            }

            currentSentence = buildSentence();
            isTransformed = false;
        }
    }
    return 0;
}