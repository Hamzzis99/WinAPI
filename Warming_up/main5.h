#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <iomanip>
#include <algorithm>
#include <ranges>

constexpr int ROWS = 10;
constexpr int COLS = 10;

using SeatGrid = std::array<std::array<int, COLS>, ROWS>;

struct ReservationInfo {
    int theaterId;
    std::string movieTitle;
    std::string showtime;
    int row, col;
    int numSeats;
};

struct Theater {
    int id;
    std::string title;
    std::vector<std::string> showtimes;
    std::map<std::string, SeatGrid> seats;
};

class MovieSystem {
private:
    std::vector<Theater> theaters;
    std::map<int, ReservationInfo> reservations;
    int nextResNum = 10;

    Theater* findTheater(const std::string& query);

    void cmdDisplay();
    void cmdPrintSeats();
    void cmdReserve();
    void cmdCancel();
    void cmdRate();

public:
    void init();
    void run();
};