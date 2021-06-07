#include "files.h"

#define R_(ptr) ptr BASE_RESTRICT

int Base_get_file_size (Base_File_t file, R_(size_t*) size_p) {
#if    defined(BASE_OS_UNIXLIKE)
	struct stat s;
	if (fstat(file, &s))
		return -1;
	*size_p = (size_t)s.st_size;
#elif  defined(BASE_OS_WINDOWS)
	LARGE_INTEGER li;
	if (!GetFileSizeEx(file, &li))
		return -1;
	*size_p = (size_t)li.QuadPart;
#else
#	error "Unsupported operating system."
#endif
	return 0;
}

size_t Base_get_file_size_or_die (Base_File_t file) {
	size_t size;
	Base_assert_msg(!Base_get_file_size(file, &size), "Error: Base_enforce_get_file_size failed!\n");
	return size;
}

int Base_get_filepath_size (R_(const char*) fpath, R_(size_t*) size_p) {
#ifdef BASE_OS_UNIXLIKE
	struct stat s;
	if (stat(fpath, &s))
		return -1;
	*size_p = (size_t)s.st_size;
	return 0;
#else /* Any other OS. */
	Base_File_t f;
	if (Base_open_filepath(fpath, true, &f))
		return -1;
	if (Base_get_file_size(f, size_p)) {
		Base_close_file(f);
		return -1;
	}
	return Base_close_file(f);
#endif
}

size_t Base_get_filepath_size_or_die (const char* filepath) {
	size_t size;
	Base_assert_msg(!Base_get_filepath_size(filepath, &size), "Error: Base_get_filepath_size failed!\n");
	return size;
}

bool Base_filepath_exists (const char* filepath) {
	bool exists = false;
	FILE* test = fopen(filepath, "r");
	if (test) {
		fclose(test);
		exists = true;
	}
	return exists;
}

void Base_force_filepath_existence_or_die (R_(const char*) filepath, bool force_to_exist) {
	if (force_to_exist)
		Base_assert_msg(Base_filepath_exists(filepath) , "Error: The filepath %s does not seem to exist.\n", filepath);
	else
		Base_assert_msg(!Base_filepath_exists(filepath), "Error: The filepath %s seems to already exist.\n", filepath);
}

int Base_open_filepath (R_(const char*) filepath, bool readonly, R_(Base_File_t*) file) {
#if    defined(BASE_OS_UNIXLIKE)
	int read_write_rights = readonly ? O_RDONLY : O_RDWR;
	*file = open(filepath, read_write_rights, (mode_t)0600);
#elif  defined(BASE_OS_WINDOWS)
	DWORD read_write_rights = readonly ? GENERIC_READ : (GENERIC_READ|GENERIC_WRITE);
	*file = CreateFileA(filepath, read_write_rights, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
#	error "Unsupported operating system."
#endif
	if (*file == BASE_NULL_FILE)
		return -1;
	return 0;
}

Base_File_t Base_open_filepath_or_die (R_(const char*) filepath, bool readonly) {
	Base_File_t file;
	Base_assert_msg(!Base_open_filepath(filepath, readonly, &file), "Error: Base_enforce_open_filepath failed with filepath '%s'.\n", filepath);
	return file;
}

int Base_create_filepath (R_(const char*) filepath, R_(Base_File_t*) file) {
#if    defined(BASE_OS_UNIXLIKE)
	*file = open(filepath, (O_RDWR|O_TRUNC|O_CREAT), (mode_t)0600);
#elif  defined(BASE_OS_WINDOWS)
	*file = CreateFileA(filepath, (GENERIC_READ|GENERIC_WRITE), 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
#else
#	error "Unsupported operating system."
#endif
	if (*file == BASE_NULL_FILE) return -1;
	return 0;
}

Base_File_t Base_create_filepath_or_die (const char* filepath) {
	Base_File_t file;
	Base_assert_msg(!Base_create_filepath(filepath, &file), "Error: Base_enforce_create_filepath failed with filepath '%s'.\n", filepath);
	return file;
}

#ifndef BASE_CLOSE_FILE_INLINE
int Base_close_file (Base_File_t file) BASE_CLOSE_FILE_IMPL(file)
#endif /* ~ BASE_INLINE_CLOSE_FILE */

#ifdef BASE_OS_UNIXLIKE
#	define ERROR_ "Error: Base_enforce_close_file failed with fd: %d\n.", file
#else
#	define ERROR_ "Error: Base_enforce_close_file failed!\n"
#endif /* ~ BASE_OS_UNIXLIKE */
void Base_close_file_or_die (Base_File_t file) { Base_assert_msg(!Base_close_file(file), ERROR_); }
#undef ERROR_

#ifndef BASE_SET_FILE_SIZE_INLINE
int Base_set_file_size (Base_File_t file, size_t size) BASE_SET_FILE_SIZE_IMPL(file, size)
#endif /* ~ BASE_INLINE_SET_FILE_SIZE */

#ifdef BASE_OS_UNIXLIKE
#	define ERROR_	"Error: Base_enforce_set_file_size failed with fd %d; size %zd.\n", file, size
#else
#	define ERROR_	"Error: Base_enforce_set_file_size failed with size %zd.\n", size
#endif
void Base_set_file_size_or_die (Base_File_t file, size_t size) {
	Base_assert_msg(!Base_set_file_size(file, size), ERROR_);
}
