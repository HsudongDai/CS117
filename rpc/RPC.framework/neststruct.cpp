#include "neststruct.idl"
#include <string>

using namespace std;

Person findPerson(ThreePeople tp) {
    return tp.p1;
}

rectangle searchRectangles(rectangle rects[10]) {
    return rects[0];
}