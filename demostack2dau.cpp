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

    void pushLeft(int x) {
        if (full()) { 
            setColor(12);
            cout << "Error: Stack full!\n"; 
            setColor(7);
            Sleep(1000);
            return; 
        }
        arr[++top1] = x;
    }

    void pushRight(int x) {
        if (full()) { 
            setColor(12);
            cout << "Error: Stack full!\n"; 
            setColor(7);
            Sleep(1000);
            return; 
        }
        arr[--top2] = x;
    }

    void popLeft() {
        if (top1 < 0) { 
            setColor(12);
            cout << "Error: Left stack empty!\n"; 
            setColor(7);
            Sleep(1000);
            return; 
        }
        cout << "Pop left: " << arr[top1--] << endl;
        system("pause");
    }

    void popRight() {
        if (top2 >= MAX) { 
            setColor(12);
            cout << "Error: Right stack empty!\n"; 
            setColor(7);
            Sleep(1000);
            return; 
        }
        cout << "Pop right: " << arr[top2++] << endl;
        system("pause");
    }

    void drawUI() {
        system("cls");
        cout << "\n                CHUONG TRINH DEMO STACK 2 DAU \n\n";

        int originX = 10;
        int originY = 5;

        gotoxy(originX, originY);
        cout << "+";
        for (int i = 0; i < MAX; i++) cout << "---+";

        gotoxy(originX, originY + 1);
        cout << "|";
        for (int i = 0; i < MAX; i++) {
            if (i <= top1) cout << setw(3) << arr[i] << "|";
            else if (i >= top2) cout << setw(3) << arr[i] << "|";
            else cout << "   |";
        }

        gotoxy(originX, originY + 2);
        cout << "+";
        for (int i = 0; i < MAX; i++) cout << "---+";

        int arrowY = originY + 3;
        int labelY = originY + 4;

        // pHead
        if (top1 == -1) {
            setColor(9);
            gotoxy(originX - 1, arrowY); cout << "^";
            gotoxy(originX - 5, labelY); cout << "pHead";
        } else {
            int arrowX = originX + 2 + top1 * 4;
            setColor(9);
            gotoxy(arrowX, arrowY); cout << "^";
            gotoxy(arrowX - 2, labelY); cout << "pHead";
        }

        // pTail
        if (top2 == MAX) {
            setColor(12);
            gotoxy(originX + MAX * 4 + 1, arrowY); cout << "^";
            gotoxy(originX + MAX * 4 - 1, labelY); cout << "pTail";
        } else {
            int arrowX = originX + 2 + top2 * 4;
            setColor(12);
            gotoxy(arrowX, arrowY); cout << "^";
            gotoxy(arrowX - 2, labelY); cout << "pTail";
        }

        setColor(7);
        gotoxy(0, labelY + 3);
        cout << "MENU:\n";
        cout << "1. Push left\n";
        cout << "2. Push right\n";
        cout << "3. Pop left\n";
        cout << "4. Pop right\n";
        cout << "5. Exit\n";
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
            case 1: {
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
            }
            case 2: {
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
            }
            case 3: s.popLeft(); break;
            case 4: s.popRight(); break;
            case 5: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice!\n"; Sleep(1000);
        }
    } while (choice != 5);

    return 0;
}

