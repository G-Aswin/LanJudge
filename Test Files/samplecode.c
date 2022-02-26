#include <stdio.h>

int main()
{
    char msg[50];
    scanf("%s", msg);

    printf("Message was %s\n", msg);
    int *p = (int*)malloc(sizeof(int));
    return 0;
}