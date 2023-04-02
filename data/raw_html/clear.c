#include <stdio.h>
int main() {
    FILE *pf = fopen("raw.txt", "w");
    fclose(pf);
    return 0;
}