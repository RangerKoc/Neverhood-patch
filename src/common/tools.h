// ----------------------------------------------------------------------------
// tools.h
// ----------------------------------------------------------------------------

#ifndef __tools_h__
#define __tools_h__

// ----------------------------------------------------------------------------

void debug_print(const char* format, ...);

int str_len(const char* str);

int str_cpy(char* dst, int size, const char* src);
int str_cat(char* dst, int size, const char* src);

int str_pos(const char* str1, const char* str2);

//int str_cmp(const char* str1, const char* str2);

int get_app_dir(char* dst, int size, bool trail_slash = true);

//int get_file_name(char* dst, bool ext = true);
int get_file_name(char* dst, int len, const char* src, bool ext = true);

bool file_exists(const char* filename);
bool dir_exists(const char* dirname);

int  read_file(const char* filename, void* buf, int maxsize);
bool write_file(const char* filename, const void* buf, int size);

void mem_read_buf(unsigned long addr, void* buf, int len);
void mem_write_buf(unsigned long addr, void* buf, int len);
void mem_write_byte(unsigned long addr, unsigned char value);

// ----------------------------------------------------------------------------

#endif /* __tools_h__ */
