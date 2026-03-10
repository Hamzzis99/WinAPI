#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <ranges>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <map>

std::string soltNumber(std::string_view text);

//3번 알파뱃 개수끼리 정렬
int compareAlpha(const void* a, const void* b);

// 4번 qsort용 비교 함수 (오름차순)
int compareAsc(const void* a, const void* b);
