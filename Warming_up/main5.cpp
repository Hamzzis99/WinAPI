#include "main5.h"

using namespace std;

void MovieSystem::init() {
    theaters = {
        {1, "Jaws", {"1000", "1400", "1700"}},
        {2, "Graduation", {"1130", "1500", "2130"}},
        {3, "Big", {"0900", "1300", "1700"}}
    };

    for (auto& t : theaters) {
        for (const auto& time : t.showtimes) {
            t.seats[time] = SeatGrid{};
        }
    }
}

Theater* MovieSystem::findTheater(const string& query) {
    auto it = ranges::find_if(theaters, [&query](const Theater& t) {
        return to_string(t.id) == query || t.title == query;
        });
    return it != theaters.end() ? &(*it) : nullptr;
}

void MovieSystem::cmdDisplay() {
    for (const auto& t : theaters) {
        cout << t.id << " " << t.title;
        for (const auto& time : t.showtimes) cout << " " << time;
        cout << "\n";
    }
}

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
                if (grid[i][j] == 0) cout << ".  ";
                else cout << grid[i][j] << " ";
            }
            cout << "\n";
        }
    }
}

// [r 명령어] 예약하기 (요구사항 흐름 완벽 반영)
void MovieSystem::cmdReserve() {
    int numSeats;

    // 💡 [수정됨] r을 누르자마자 가장 먼저 좌석 개수를 물어보도록 맨 위로 배치
    cout << "how many seats?: (1 ~ 4) "; cin >> numSeats;
    if (numSeats < 1 || numSeats > 4) {
        cout << ">> You can only reserve 1 to 4 seats at a time.\n"; return;
    }

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

    // 💡 [예외처리 1] 예약 좌석의 개수가 한 열의 숫자인 10을 넘었을 경우
    if (col + numSeats > COLS) {
        cout << ">> Cannot reserve: Not enough seats in this row (Exceeds 10 columns).\n"; return;
    }

    // 💡 [예외처리 2 & 3] 처음부터 예약된 좌석이거나, 여러 자리 중 일부가 이미 예약된 경우
    for (int i = 0; i < numSeats; ++i) {
        if (t->seats[time][row][col + i] != 0) {
            cout << ">> Cannot reserve: Seat (" << row << ", " << col + i << ") is already reserved!\n";
            return; // 하나라도 이미 예약되어 있다면 전체 예약을 즉시 취소
        }
    }

    // 모든 예외 검사를 통과했으므로 좌석 배열에 동일한 예약 번호 배당
    int resNum = nextResNum++;
    for (int i = 0; i < numSeats; ++i) {
        t->seats[time][row][col + i] = resNum;
    }

    reservations[resNum] = { t->id, t->title, time, row, col, numSeats };

    cout << "\nReservation:\n"
        << "Movie name: " << t->title << "\n"
        << "Time: " << time << "\n"
        << "Seat numbers: ";
    for (int i = 0; i < numSeats; ++i) {
        cout << "(" << row << ", " << col + i << ") "; // 3x4, 3x5 형태로 출력
    }
    cout << "\nYour reservation number is " << resNum << ". Thank you for your reservation!\n";
}

// [c 명령어] 취소하기 (다중 좌석 일괄 취소)
void MovieSystem::cmdCancel() {
    int resNum;
    cout << "Enter the reservation number: "; cin >> resNum;

    if (!reservations.contains(resNum)) {
        cout << ">> Reservation not found.\n"; return;
    }

    ReservationInfo info = reservations[resNum];
    Theater* t = findTheater(to_string(info.theaterId));

    // 해당 예약 번호가 가진 좌석 개수(numSeats)만큼 반복하며 일괄 공석(0) 처리
    for (int i = 0; i < info.numSeats; ++i) {
        t->seats[info.showtime][info.row][info.col + i] = 0;
    }
    reservations.erase(resNum);

    cout << "\nYour reservation information is:\n"
        << "Movie name: " << info.movieTitle << "\n"
        << "Time: " << info.showtime << "\n"
        << "Seat numbers: ";
    for (int i = 0; i < info.numSeats; ++i) {
        cout << "(" << info.row << ", " << info.col + i << ") ";
    }
    cout << "\nReservation is cancelled. Please, come again!\n";
}

void MovieSystem::cmdRate() {
    int totalSeats = ROWS * COLS;

    for (const auto& t : theaters) {
        cout << t.id << " " << t.title << " ";
        for (const auto& time : t.showtimes) {
            const auto& grid = t.seats.at(time);

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