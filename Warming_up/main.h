#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <ranges>
#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include <vector>

std::string SortNumber(std::string_view text);
std::string SortByWordLength(std::string_view text);
void Sentence(std::string& text);
void SpacesManager(std::string& text, int delta);

struct CharCmp {
    bool operator()(char a, char b) const {
        bool aIsLower = islower(static_cast<unsigned char>(a));
        bool bIsLower = islower(static_cast<unsigned char>(b));

        // 둘 중 하나만 소문자라면, 소문자인 쪽을 앞으로 보냄
        if (aIsLower != bIsLower) return aIsLower;

        // 둘 다 소문자이거나 둘 다 대문자라면 기존 알파벳순 정렬
        return a < b;
    }
};