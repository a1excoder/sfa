#include "sfa.h"

// #ifdef __linux__
// #ifdef _WIN32
// #endif

static int64_t get_file_size(int fd)
{
	// struct stat64 st;
	struct stat st;
	if (fstat(fd, &st) < 0) return -1;

	return (int64_t)st.st_size;
}

// SFA_TRUE = is utf-8 locale
inline static sfa_bool locale_is_utf8()
{
	setlocale(LC_ALL, "");
	if (strstr(setlocale(LC_CTYPE, NULL), "UTF-8") != NULL)
		return SFA_TRUE;
	
	return SFA_FALSE;
}

// SFA_FALSE = true extension
static sfa_bool check_file_extension(const char *path_to_file)
{
	assert(path_to_file != NULL);

	static const char *extension = "sfa";
	static const char c_dot = '.';

	// const char *cc = path_to_file;
	char *c = NULL;

	while((c = strchr(++path_to_file, c_dot)) != NULL)
		path_to_file = c;

	if (path_to_file == NULL || strcmp(path_to_file, extension))
		return SFA_TRUE;
		
	return SFA_FALSE;
}

// if SFA_FALSE = true signature
static sfa_bool check_signature(const sfa_file* file_hptr)
{
	assert(file_hptr != NULL);

	if (!memcmp(file_hptr->mapped_file, signature, SIGN_SIZE)) {
		return SFA_FALSE;
	}

	return SFA_TRUE;
}

sfa_file *open_sfa(const char *path_to_file, sfa_file* file_hptr)
{
	assert(path_to_file != NULL || file_hptr != NULL);

	if (check_file_extension(path_to_file)) return NULL;

	file_hptr->fd = open(path_to_file, O_RDONLY); // | O_NOATIME | O_LARGEFILE
	if (file_hptr->fd == -1) return NULL;

	int64_t fsize = get_file_size(file_hptr->fd);
	if (fsize == -1) {
		close(file_hptr->fd);
		return NULL;
	}

	// if (fcntl(file_hptr->fd, F_SETLEASE, F_RDLCK) == -1) {
	// 	close(file_hptr->fd);
	// 	return NULL;
	// }

	file_hptr->fsize = (size_t)fsize;
	file_hptr->mapped_file = mmap(NULL, file_hptr->fsize, 
				PROT_READ, MAP_PRIVATE, file_hptr->fd, 0);
	
	if (file_hptr->mapped_file == MAP_FAILED) {
		// fcntl(file_hptr->fd, F_SETLEASE, F_UNLCK);
		close(file_hptr->fd);
		return NULL;
	}

	if (check_signature(file_hptr)) {
		close(file_hptr->fd);
		return NULL;
	}

	return file_hptr;
}

sfa_header *read_header(sfa_file* file_hptr, sfa_header* header_ptr)
{
	assert(file_hptr != NULL || header_ptr != NULL);

	memcpy(header_ptr, (uint8_t*)file_hptr->mapped_file + SIGN_SIZE, sizeof(sfa_header));
	return header_ptr;
}


sfa_file *create_sfa(const char *path_to_file, sfa_file* file_hptr, uint8_t version[4])
{
	assert(path_to_file != NULL || file_hptr != NULL);

	if (check_file_extension(path_to_file)) return NULL;

	// file_hptr->fd = creat(path_to_file, S_IROTH | S_IWOTH);
	file_hptr->fd = open(path_to_file, O_CREAT | O_RDWR, S_IROTH | S_IWOTH);
	if (file_hptr->fd == -1) return NULL;

	file_hptr->fsize = (size_t)(SIGN_SIZE + sizeof(sfa_header));
	if (ftruncate(file_hptr->fd, (off_t)file_hptr->fsize) == -1) {
		close(file_hptr->fd);
		return NULL;
	}

	file_hptr->mapped_file = mmap(NULL, file_hptr->fsize, 
	PROT_READ | PROT_WRITE, MAP_PRIVATE, file_hptr->fd, 0);

	if (file_hptr->mapped_file == MAP_FAILED) {
		close(file_hptr->fd);
		return NULL;
	}

	memcpy(file_hptr->mapped_file, signature, SIGN_SIZE);

	sfa_header header = {
		.created_at = (int64_t)time(NULL), // unix time (from 00:00:00 UTC, January 1, 1970) measured in seconds
		.version[0] = version[0],
		.version[1] = version[1],
		.version[2] = version[2],
		.version[3] = version[3],
		.reserved = 0U
	};
	
	memcpy((uint8_t*)file_hptr->mapped_file + SIGN_SIZE, &header, sizeof(sfa_header));

	return file_hptr;
}

sfa_bool close_sfa(sfa_file *file_hptr)
{
	assert(file_hptr != NULL);

	if (munmap(file_hptr->mapped_file, file_hptr->fsize) == -1) {
		close(file_hptr->fd);
		return SFA_FALSE;
	}

	// if (fcntl(file_hptr->fd, F_SETLEASE, F_UNLCK) == -1) {
	// 	close(file_hptr->fd);
	// 	return SFA_FALSE;
	// }

	if (close(file_hptr->fd) == -1) {
		return SFA_FALSE;
	}

	return SFA_TRUE;
}
