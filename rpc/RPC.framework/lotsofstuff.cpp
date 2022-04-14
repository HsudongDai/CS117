#include <iostream>
#include <string>

using namespace std;
#include "lotsofstuff.idl"

void func1() {
    cout << "func1!";
}
void func2() {
    cout << "func2!";
};
void func3() {
    cout << "func3!";
};

int sqrt(int x, int y) {
    return x * y;
}

int takesTwoArrays(int x[24], int y[24]) {
    return x[1] + y[1];
}

int showsArraysofArrays(int x[24], int y[24][15], int z[24][15]) {
    return x[1] + y[1][1] + z[1][1];
}

string upcase(string s) {
    for (int i = 0; i < s.length(); i++) {
        s[i] = toupper(s[i]);
    }
    return s;
}

Person findPerson(ThreePeople tp) {
    return tp.p2;
}

float multiply(float x, float y) {
    return x * y;
}

int area(rectangle r) {
    return r.x * r.y;
}

void searchRectangles(rectangle rects[200]) {
    cout << "searchRectangles!";
};

