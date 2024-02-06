#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include "sfa.h"

void print_datetime(const sfa_header* header_ptr)
{
	time_t file_time = (time_t)header_ptr->created_at;
	struct tm tm = *localtime(&file_time);
	printf("archive created at %d-%02d-%02d %02d:%02d:%02d\n", 
		tm.tm_year + 1900, tm.tm_mon + 1, 
		tm.tm_mday, tm.tm_hour, tm.tm_min, 
		tm.tm_sec);
}

void print_version(const sfa_header* header_ptr)
{
	printf("version is: %u.%u.%u.%u\n", 
		header_ptr->version[0],
		header_ptr->version[1],
		header_ptr->version[2],
		header_ptr->version[3]);
}

int main(int argc, char const *argv[])
{
	uint8_t version[4] = {0, 0, 1, 0};

	sfa_file file = {0};
	// if (create_sfa(argv[1], &file, version) == NULL) return -1;
	// close_sfa(&file);
	// return 0;
	if (open_sfa(argv[1], &file) == NULL)
		return 1;

	sfa_header header = {0};

	if (read_header(&file, &header) == NULL)
		return 2;

	print_datetime(&header);
	print_version(&header);

	fgetc(stdin);

	close_sfa(&file);

	return 0;
}