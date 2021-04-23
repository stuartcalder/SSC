#include "files.h"

int
shim_get_file_size (Shim_File_t const file, ssize_t * SHIM_RESTRICT size_p) {
#if    defined(SHIM_OS_UNIXLIKE)
	struct stat s;
	if (fstat(file, &s))
		return -1;
	*size_p = (ssize_t)s.st_size;
#elif  defined(SHIM_OS_WINDOWS)
	LARGE_INTEGER li;
	if (!GetFileSizeEx(file, &li))
		return -1;
	*size_p = (ssize_t)li.QuadPart;
#else
#	error "Unsupported operating system."
#endif
	return 0;
}

ssize_t
shim_enforce_get_file_size (Shim_File_t const file) {
	ssize_t size;
	shim_assert_msg(!shim_get_file_size(file, &size), "Error: shim_enforce_get_file_size failed!\n");
	return size;
}

int
shim_get_filepath_size (char const * SHIM_RESTRICT fpath,
		   	ssize_t *    SHIM_RESTRICT size_p)
{
#ifdef SHIM_OS_UNIXLIKE
	struct stat s;
	if (stat(fpath, &s))
		return -1;
	*size_p = (ssize_t)s.st_size;
	return 0;
#else /* Any other OS. */
	Shim_File_t f;
	if (shim_open_filepath(fpath, true, &f))
		return -1;
	if (shim_get_file_size(f, size_p)) {
		shim_close_file(f);
		return -1;
	}
	return shim_close_file(f);
#endif
}

ssize_t
shim_enforce_get_filepath_size (char const * filepath) {
	ssize_t size;
	shim_assert_msg(!shim_get_filepath_size(filepath, &size), "Error: shim_get_filepath_size failed!\n");
	return size;
}

bool
shim_filepath_exists (char const * filepath) {
	bool exists = false;
	FILE * test = fopen(filepath, "r");
	if (test) {
		fclose(test);
		exists = true;
	}
	return exists;
}

void
shim_enforce_filepath_existence (char const * SHIM_RESTRICT filepath,
				 bool const                 force_to_exist)
{
	if (force_to_exist)
		shim_assert_msg(shim_filepath_exists(filepath), "Error: The filepath %s does not seem to exist.\n", filepath);
	else
		shim_assert_msg(!shim_filepath_exists(filepath), "Error: The filepath %s seems to already exist.\n", filepath);
}

int
shim_open_filepath (char const *  SHIM_RESTRICT filepath,
		    bool const                  readonly,
		    Shim_File_t * SHIM_RESTRICT file)
{
#if    defined(SHIM_OS_UNIXLIKE)
	int const read_write_rights = readonly ? O_RDONLY : O_RDWR;
	*file = open(filepath, read_write_rights, (mode_t)0600);
#elif  defined(SHIM_OS_WINDOWS)
	DWORD read_write_rights = readonly ? GENERIC_READ : (GENERIC_READ|GENERIC_WRITE);
	*file = CreateFileA(filepath, read_write_rights, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
#	error "Unsupported operating system."
#endif
	if (*file == SHIM_NULL_FILE)
		return -1;
	return 0;
}

Shim_File_t
shim_enforce_open_filepath (char const * SHIM_RESTRICT filepath,
			    bool const                 readonly)
{
	Shim_File_t file;
	if (shim_open_filepath(filepath, readonly, &file))
		shim_errx("Error: shim_enforce_open_filepath failed with filepath '%s'.\n", filepath);
	return file;
}

int
shim_create_filepath (char const *  SHIM_RESTRICT filepath,
		      Shim_File_t * SHIM_RESTRICT file)
{
#if    defined(SHIM_OS_UNIXLIKE)
	*file = open(filepath, O_RDWR|O_TRUNC|O_CREAT, (mode_t)0600);
#elif  defined(SHIM_OS_WINDOWS)
	*file = CreateFileA(filepath, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
#else
#	error "Unsupported operating system."
#endif
	if (*file == SHIM_NULL_FILE)
		return -1;
	return 0;
}

Shim_File_t
shim_enforce_create_filepath (char const * filepath) {
	Shim_File_t file;
	if (shim_create_filepath(filepath, &file))
		shim_errx("Error: shim_enforce_create_filepath failed with filepath '%s'.\n", filepath);
	return file;
}

#ifndef SHIM_FILES_INLINE_CLOSE_FILE
int
shim_close_file (Shim_File_t const file)
	SHIM_FILES_CLOSE_FILE_IMPL(file)
#endif /* ~ SHIM_FILES_INLINE_CLOSE_FILE */

#ifdef SHIM_OS_UNIXLIKE
#	define ERROR_ "Error: shim_enforce_close_file failed with fd: %d\n.", file
#else
#	define ERROR_ "Error: shim_enforce_close_file failed!\n"
#endif /* ~ SHIM_OS_UNIXLIKE */
void
shim_enforce_close_file (Shim_File_t const file) {
	if (shim_close_file(file))
		shim_errx(ERROR_);
}
#undef ERROR_

#ifndef SHIM_FILES_INLINE_SET_FILE_SIZE
int
shim_set_file_size (Shim_File_t const file, ssize_t const new_size)
	SHIM_FILES_SET_FILE_SIZE_IMPL(file, new_size)
#endif /* ~ SHIM_FILES_INLINE_SET_FILE_SIZE */

#ifdef SHIM_OS_UNIXLIKE
#	define ERROR_	"Error: shim_enforce_set_file_size failed with fd %d; size %zu.\n", file, new_size
#else
#	define ERROR_	"Error: shim_enforce_set_file_size failed with size %zu.\n", new_size
#endif
void
shim_enforce_set_file_size (Shim_File_t const file, ssize_t const new_size) {
	if (shim_set_file_size(file, new_size))
		shim_errx(ERROR_);
}
#undef ERROR_
