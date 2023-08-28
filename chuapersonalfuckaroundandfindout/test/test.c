#include <unistd.h>
#include <stdio.h>

int main()
{
	char *buff[256];

	int x = write(1, "12345\n\0", 7);
	printf("%d\n", x);
}