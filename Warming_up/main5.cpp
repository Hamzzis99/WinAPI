#include "main.h"

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

void MovieSystem::cmdReserve() {
    int numSeats;

    cout << "how many seats?: (1 ~ 4) ";

    //추가 된 부분 number만 입력하는 게 아닌이상 에러.
    if (!(cin >> numSeats)) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << ">> Invalid input. Please enter a number.\n";
        return;
    }

    if (numSeats < 1 || numSeats > 4) {
        cout << ">> You can only reserve 1 to 4 seats at a time.\n";
        return;
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

    cout << "Seat you want (row col): ";
    if (!(cin >> row >> col)) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << ">> Invalid input. Please enter row and col numbers.\n";
        return;
    }

    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        cout << ">> Invalid seat range.\n"; return;
    }

    if (col + numSeats > COLS) {
        cout << ">> Cannot reserve: Not enough seats in this row (Exceeds 10 columns).\n"; return;
    }

    for (int i = 0; i < numSeats; ++i) {
        if (t->seats[time][row][col + i] != 0) {
            cout << ">> Cannot reserve: Seat (" << row << ", " << col + i << ") is already reserved!\n";
            return;
        }
    }

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
        cout << "(" << row << ", " << col + i << ") ";
    }
    cout << "\nYour reservation number is " << resNum << ". Thank you for your reservation!\n";
}

void MovieSystem::cmdCancel() {
    int resNum;
    cout << "Enter the reservation number: ";

    if (!(cin >> resNum)) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << ">> Invalid input. Please enter a number.\n";
        return;
    }

    if (!reservations.contains(resNum)) {
        cout << ">> Reservation not found.\n"; return;
    }

    ReservationInfo info = reservations[resNum];
    Theater* t = findTheater(to_string(info.theaterId));

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