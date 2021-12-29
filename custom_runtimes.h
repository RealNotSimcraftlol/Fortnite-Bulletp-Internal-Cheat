#pragma once

namespace crt {
	uintptr_t GetBaseAddress(const wchar_t* modName);

	//uint64_t rnd();
	float sqrtf(float _X);
	float expf(float _X);
	float sinf(float _X);
	float cosf(float _X);
	float tanf(float _X);
	float asinf(float _X);
	float acosf(float _X);
	float atan2f(float _X, float _Y);
	int toupper(int c);
	int tolower(int c);
	void* memcpy(void* dest, const void* src, unsigned __int64 count);
	void* memset(void* src, int val, unsigned __int64 count);
	void* memccpy(void* to, const void* from, int c, unsigned __int64 count);
	void* memchr(const void* s, int c, unsigned __int64 n);
	int  memcmp(const void* s1, const void* s2, unsigned __int64 n);
	int  memicmp(const void* s1, const void* s2, unsigned __int64 n);


	void* memmove(void* dest, const void* src, unsigned __int64 count);


	char* _cslwr(char* x);
	char* _csupr(char* x);



	int strlen(const char* string);
	int wcslen(const wchar_t* string);
	const char* strcpy(char* buffer, const char* string);
	int strcmp(const char* cs, const char* ct);
	int stricmp(const char* cs, const char* ct);
}