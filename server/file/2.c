# include <stdio.h>
# include <stdlib.h>
void main (void)
{
int num = 100;
char str[25];
itoa(num, str, 10);
printf("The number 'num' is %d and the string 'str' is %s. \n" ,
num, str);
}