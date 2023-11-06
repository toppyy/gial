#include <stdio.h>

int main() {

    int sum = 0;
    while (sum < 214745088) { // 2147450880
        sum = sum + 1;
    }

    printf("%d\n", sum);

}