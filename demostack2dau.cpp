#include <iostream>
#include <iomanip>
#include <limits>
#include <windows.h>
using namespace std;

#define MAX 10

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

class DeStack {
private:
    int arr[MAX];
    int top1, top2;

public:
    DeStack() {
        top1 = -1;
        top2 = MAX;
    }

    bool full() { return (top1 + 1 == top2); }

    void showError(const string& msg) {
    int errorX = 0;      // bên trái console
    int errorY = 25;     // dòng cuối console

    setColor(12);
    gotoxy(errorX, errorY);
    cout << msg;
    setColor(7);
    Sleep(2000);         // hiệnhiện 2 giây
    gotoxy(errorX, errorY);
    cout << "                                    "; // xóa thông báo
}

    void pushLeft(int x) {
        if (full()) {
            showError("Error: Stack full!");
            return;
        }
        arr[++top1] = x;
    }

    void pushRight(int x) {
        if (full()) {
            showError("Error: Stack full!");
            return;
        }
        arr[--top2] = x;
    }

    void popLeft() {
        if (top1 < 0) {
            showError("Error: Left stack empty!");
            return;
        }
        gotoxy(0, 15); cout << "Pop left: " << arr[top1--] << "           ";
    }

    void popRight() {
        if (top2 >= MAX) {
            showError("Error: Right stack empty!");
            return;
        }
        gotoxy(0, 15); cout << "Pop right: " << arr[top2++] << "          ";
    }

    void drawUI() {
        system("cls");
        cout << "\n              CHUONG TRINH DEMO STACK 2 DAU \n\n";

        int originX = 10;
        int originY = 5;

        gotoxy(originX + 4, originY - 1); cout << "S1";
        gotoxy(originX + 24, originY - 1); cout << "S2";

        gotoxy(originX, originY); cout << "+";
        for (int i = 0; i < MAX; i++) {
            if (i < 5) setColor(9);
            else setColor(12);
            cout << "---+";
        }
        setColor(7);

        gotoxy(originX, originY + 1); cout << "|";
        for (int i = 0; i < MAX; i++) {
            if (i < 5) setColor(9);
            else setColor(12);
            if (i <= top1 || i >= top2) cout << setw(3) << arr[i] << "|";
            else cout << "   |";
        }
        setColor(7);

        gotoxy(originX, originY + 2); cout << "+";
        for (int i = 0; i < MAX; i++) {
            if (i < 5) setColor(9);
            else setColor(12);
            cout << "---+";
        }
        setColor(7);

        int arrowY = originY + 3;
        int labelY = originY + 4;

        int headX = originX - 2;
        int tailX = originX + 2 + MAX * 4;

        setColor(9);
        gotoxy(headX, arrowY); cout << "^";
        gotoxy(headX - 2, labelY); cout << "pHead";

        setColor(12);
        gotoxy(tailX, arrowY); cout << "^";
        gotoxy(tailX - 2, labelY); cout << "pTail";

        setColor(7);

        gotoxy(0, labelY + 3);
        cout << "MENU:\n";
        cout << "1. Push left\n2. Push right\n3. Pop left\n4. Pop right\n5. Exit\n";
    }
};

int main() {
    DeStack s;
    int choice, val;

    do {
        s.drawUI();
        cout << "\nChoice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input!\n";
            Sleep(1000);
            continue;
        }

        switch (choice) {
            case 1:
                while (true) {
                    cout << "Enter number: ";
                    cin >> val;
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Please enter a number!\n";
                    } else {
                        s.pushLeft(val);
                        break;
                    }
                }
                break;
            case 2:
                while (true) {
                    cout << "Enter number: ";
                    cin >> val;
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Please enter a number!\n";
                    } else {
                        s.pushRight(val);
                        break;
                    }
                }
                break;
            case 3: s.popLeft(); break;
            case 4: s.popRight(); break;
            case 5: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice!\n"; Sleep(1000);
        }
    } while (choice != 5);

    return 0;
}

