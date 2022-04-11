#include "structwitharray.idl"

int getStructSum(s arr) {
    int sum = 0;
    for (int i = 0; i < 4; i++) {
        sum += arr.m1[i];
        for (int j = 0; j < 10; j++) {
            sum += arr.m2[i][j];
            for (int k = 0; k < 100; k++) {
                sum += arr.m3[i][j][k];
            }
        }
    }
    return sum;
}