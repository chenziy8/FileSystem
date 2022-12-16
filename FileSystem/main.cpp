#include <stdlib.h>
#include <stdio.h>
#include "FileHeader.h"



int main()
{
	create_fs();
	write_fs();
	string name = "first";
	int n = name.length() + 1;
	char name_char[8];
	strcpy_s(name_char, name.c_str());
	int res = allocate_file(name_char);


	char test = 'b';
	set_filesize(0, 5000);
	for (int i = 0; i < 49; i++) {
		write_byte(0, i * 100, &test);
	}
	
	write_fs();
	print_fs();
	printf("done\n");
	
	return 0;
}

