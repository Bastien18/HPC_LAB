#include<stdio.h>
int main() {
    // sizeof evaluates the size of a variable
    printf("Size of int: %zu bytes\n", sizeof(int));
    printf("Size of float: %zu bytes\n", sizeof(float));
    printf("Size of double: %zu bytes\n", sizeof(double));
    printf("Size of char: %zu byte\n", sizeof(char));
    printf("Size of size_t: %zu byte\n", sizeof(size_t));
    
    return 0;
}