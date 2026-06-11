#ifndef STRING_H
#define STRING_H

void memcpy(void* dst, void* src, int len);
void memset(void* ptr, int val, int num);
int strlen(const char* s);
int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, unsigned int n);
void strcpy(char* dst, const char* src);
char* strcat(char* dst, const char* src);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
char* strstr(const char* haystack, const char* needle);
void itoa(int value, char* str, int base);

#endif
