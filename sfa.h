#ifndef SFA_H
#define SFA_H

#ifdef __cplusplus
extern "C" {
#endif


// #define NDEBUG

#ifdef __linux__
#define _GNU_SOURCE

#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#endif

#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <locale.h>


/*

FILE STRUCTURE

*/

#define SFA_TRUE 1
#define SFA_FALSE 0

typedef uint8_t sfa_bool;

// first six bytes in file
#define SIGN_SIZE 6
static const uint8_t signature[] = {0x73, 0x66, 0x61, 0x34, 0x75, 0x73}; // sfa4us

// after signature bytes
typedef struct {
	int64_t created_at; // unix time format
	uint8_t version[4]; // [major, minor, patch, reserved] or ..
	uint32_t reserved; // ..
} sfa_header;

typedef struct {
	// int64_t modificated_at; // size 24 with | 16 without
	uint32_t parent_dir_id; // directory id
	uint32_t file_name_size; // bytes
	uint32_t file_size; // bytes
} sfa_file_info; 


// 0 = root dir
typedef struct {
	uint32_t parent_id; // directory id
	uint32_t dir_name_size; // bytes
	uint32_t files_count; // how many files in dir
} sfa_directory_info;

typedef struct {
	void *mapped_file;
	size_t fsize;
	int fd;
} sfa_file;

sfa_file *open_sfa(const char *path_to_file, sfa_file* file_hptr);
sfa_bool close_sfa(sfa_file *file_hptr);

sfa_file *create_sfa(const char *path_to_file, sfa_file* file_hptr, uint8_t version[4]);
sfa_header *read_header(sfa_file* file_hptr, sfa_header* header_ptr);


#ifdef __cplusplus
}
#endif

#endif // SFA_H