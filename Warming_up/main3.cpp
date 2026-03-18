#include "main3.h"

using namespace std;

// [초기화] 3개의 영화관 정보 세팅
void MovieSystem::init() {
    theaters = {
        {1, "Jaws", {"1000", "1400", "1700"}},
        {2, "Graduation", {"1130", "1500", "2130"}},
        {3, "Big", {"0900", "1300", "1700"}}
    };

    // 각 상영시간별 10x10 좌석을 0(공석)으로 자동 초기화
    for (auto& t : theaters) {
        for (const auto& time : t.showtimes) {
            t.seats[time] = SeatGrid{};
        }
    }
}

// [검색] 번호 또는 제목으로 영화관 찾기 (ranges 활용)
Theater* MovieSystem::findTheater(const string& query) {
    auto it = ranges::find_if(theaters, [&query](const Theater& t) {
        return to_string(t.id) == query || t.title == query;
        });
    return it != theaters.end() ? &(*it) : nullptr;
}

// [d 명령어] 영화관 정보 출력
void MovieSystem::cmdDisplay() {
    for (const auto& t : theaters) {
        cout << t.id << " " << t.title;
        for (const auto& time : t.showtimes) cout << " " << time;
        cout << "\n";
    }
}

// [p 명령어] 좌석 상태 출력 (빈 좌석과 예약 좌석 구분)
void MovieSystem::cmdPrintSeats() {
    string query;
    cin >> query;
    Theater* t = findTheater(query);

    if (!t) { cout << ">> Invalid theater or movie title.\n"; return; }

    for (const auto& time : t->showtimes) {
        cout << t->title << " " << time << ":\n";
        const auto& grid = t->seats[time];
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if (grid[i][j] == 0) cout << ".  "; // 빈 좌석
                else cout << grid[i][j] << " ";     // 2자리 예약 번호
            }
            cout << "\n";
        }
    }
}

// [r 명령어] 예약하기
void MovieSystem::cmdReserve() {
    string query, time;
    int row, col;

    cout << "Which Movie (ID or Title): "; cin >> query;
    Theater* t = findTheater(query);
    if (!t) { cout << ">> Movie not found.\n"; return; }

    cout << "What time: "; cin >> time;
    if (ranges::find(t->showtimes, time) == t->showtimes.end()) {
        cout << ">> Invalid time.\n"; return;
    }

    cout << "Seat you want (row col): "; cin >> row >> col;
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        cout << ">> Invalid seat range.\n"; return;
    }

    if (t->seats[time][row][col] != 0) {
        cout << ">> Seat already reserved!\n"; return;
    }

    // 2자리 숫자 배당 및 예약 처리
    int resNum = nextResNum++;
    t->seats[time][row][col] = resNum;
    reservations[resNum] = { t->id, t->title, time, row, col };

    cout << "\nReservation:\n"
        << "Movie name: " << t->title << "\n"
        << "Time: " << time << "\n"
        << "Seat number: (" << row << ", " << col << ")\n"
        << "Your reservation number is " << resNum << ". Thank you for your reservation!\n";
}

// [c 명령어] 취소하기 (예약 번호 기반)
void MovieSystem::cmdCancel() {
    int resNum;
    cout << "Enter the reservation number: "; cin >> resNum;

    // map 컨테이너의 특징을 활용해 반복문 없이 즉시 검색
    if (!reservations.contains(resNum)) { // C++20 contains
        cout << ">> Reservation not found.\n"; return;
    }

    ReservationInfo info = reservations[resNum];
    Theater* t = findTheater(to_string(info.theaterId));

    // 좌석 공석으로 원상복구 후 예약 기록 삭제
    t->seats[info.showtime][info.row][info.col] = 0;
    reservations.erase(resNum);

    cout << "\nYour reservation information is:\n"
        << "Movie name: " << info.movieTitle << "\n"
        << "Time: " << info.showtime << "\n"
        << "Seat number: (" << info.row << ", " << info.col << ")\n"
        << "Reservation is cancelled. Please, come again!\n";
}

// [h 명령어] 예약률 출력
void MovieSystem::cmdRate() {
    int totalSeats = ROWS * COLS;

    for (const auto& t : theaters) {
        cout << t.id << " " << t.title << " ";
        for (const auto& time : t.showtimes) {
            const auto& grid = t.seats.at(time);

            // ranges::count_if를 사용해 예약된(0이 아닌) 좌석 수 카운트
            int booked = 0;
            for (const auto& row : grid) {
                booked += ranges::count_if(row, [](int seat) { return seat != 0; });
            }

            double rate = (static_cast<double>(booked) / totalSeats) * 100.0;
            cout << time << ": " << fixed << setprecision(2) << rate << "%  ";
        }
        cout << "\n";
    }
}

void MovieSystem::run() {
    init();
    while (true) {
        cout << "\nCommand (d, p, r, c, h, q): ";
        string cmd;
        cin >> cmd;

        if (cmd == "q") break;
        if (cmd == "d") cmdDisplay();
        else if (cmd == "p") cmdPrintSeats();
        else if (cmd == "r") cmdReserve();
        else if (cmd == "c") cmdCancel();
        else if (cmd == "h") cmdRate();
        else cout << ">> Invalid Command.\n";
    }
}

int main() {
    MovieSystem system;
    system.run();
    return 0;
}