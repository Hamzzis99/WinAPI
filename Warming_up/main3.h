#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <iomanip>
#include <algorithm>
#include <ranges>

// 상수 정의: 10x10 좌석
constexpr int ROWS = 10;
constexpr int COLS = 10;

// STL std::array를 활용한 2차원 좌석표 (0: 공석, 그 외: 예약번호)
using SeatGrid = std::array<std::array<int, COLS>, ROWS>;

// 예약 취소 기능을 위한 단일 예약 정보 구조체
struct ReservationInfo {
    int theaterId;
    std::string movieTitle;
    std::string showtime;
    int row, col;
};

// 영화관 정보 구조체
struct Theater {
    int id;                                // 영화관 번호 (1~3)
    std::string title;                     // 영화 제목
    std::vector<std::string> showtimes;    // 3개의 상영시간
    std::map<std::string, SeatGrid> seats; // 상영시간별 좌석 상태 매핑
};

class MovieSystem {
private:
    std::vector<Theater> theaters;
    std::map<int, ReservationInfo> reservations; // 예약 번호로 O(log N) 빠른 검색 지원
    int nextResNum = 10; // 예약 번호는 2자리 숫자(10부터 시작)로 배당

    // STL 알고리즘을 활용한 영화관 검색 헬퍼 함수
    Theater* findTheater(const std::string& query);

    // 명령어 처리 함수들
    void cmdDisplay();     // d: 정보 출력
    void cmdPrintSeats();  // p: 좌석 상태 출력
    void cmdReserve();     // r: 예약하기
    void cmdCancel();      // c: 취소하기
    void cmdRate();        // h: 예약률 출력

public:
    void init();           // 3개의 영화관 및 상영시간 초기화
    void run();            // 메인 루프 실행
};