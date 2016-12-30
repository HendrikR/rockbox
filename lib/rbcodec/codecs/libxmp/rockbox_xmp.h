#ifndef LIBXMP_ROCKBOX_XMP_H
#define LIBXMP_ROCKBOX_XMP_H

#include "codeclib.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <sys/types.h>
#include <setjmp.h> // typedef jmp_buf
#include <time.h>   // typedef time_t


#define UNUSED(identifier) identifier __attribute__((unused))


#ifdef _MSC_VER
#define D_CRIT "  Error: "
#define D_WARN "Warning: "
#define D_INFO "   Info: "
#ifndef CLIB_DECL
#define CLIB_DECL
#endif

#ifdef DEBUG
#ifndef ATTR_PRINTF
#define ATTR_PRINTF(x,y)
#endif
void CLIB_DECL D_(const char *text, ...) ATTR_PRINTF(1,2);
#else
// VS prior to VC7.1 does not support variadic macros. VC8.0 does not optimize unused parameters passing
#if _MSC_VER < 1400
void __inline CLIB_DECL D_(const char *text, ...) { do {} while (0); }
#else
#define D_(args, ...)
#endif
#endif

#elif defined ANDROID

#ifdef DEBUG
#include <android/log.h>
#define D_CRIT "  Error: "
#define D_WARN "Warning: "
#define D_INFO "   Info: "
#define D_(args...) do { \
	__android_log_print(ANDROID_LOG_DEBUG, "libxmp", args); \
	} while (0)
#else
#define D_(args...) do {} while (0)
#endif

#elif defined ROCKBOX

#define D_INFO "info: "
#define D_CRIT "crit: "
#define D_WARN "warn: "
#ifdef ROCKBOX_HAS_LOGF
#define D_ ci->logf
#else
//#define D_(args...) do { printf(args); printf("\n"); } while(0)
#define D_(args...) do {} while (0)
#endif

#else

#ifdef DEBUG
#define D_INFO "\x1b[33m"
#define D_CRIT "\x1b[31m"
#define D_WARN "\x1b[36m"
#define D_(args...) do { \
	printf("\x1b[33m%s \x1b[37m[%s:%d] " D_INFO, __FUNCTION__, \
		__FILE__, __LINE__); printf (args); printf ("\x1b[0m\n"); \
	} while (0)
#else
#define D_(args...) do {} while (0)
#endif

#endif	/* !_MSC_VER */



//#ifndef M_LOG10E
double pow(double x, double y);
double sin(double x);
double cos(double x);
double log10(double x);
double log(double x);
double exp(double x);
double floor(double x);
//#endif

#if !defined __FILE_defined
#define PATH_MAX 4096

#define	BUFSIZ 1024
typedef int FILE;
typedef int pid_t;
#define getpid _getpid


#define S_ISDIR(st_mode) 0
struct stat {
    unsigned long st_size;
    unsigned short st_mode;
};

int stat(const char* filename, struct stat* stat);
int fstat(int fileno, struct stat* stat);

int getc (FILE* stream);

char* strncpy(char* dest, const char* src, size_t n);
char* strdup(const char* s);
int strncmp(const char* s1, const char* s2, size_t n);
char* strncat(char* dest, const char* src, size_t n);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
int sprintf(char *str, const char *format, ...);

FILE* fopen(const char* fpath, const char* fmode);
int fclose(FILE* stream);
int fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
int fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);
int fgetc(FILE* stream);
int fputc(int c, FILE* stream);
int fseek(FILE* stream, long offset, int whence);
long ftell(FILE* stream);
int fflush(FILE* stream);
int ferror(FILE* stream);
//int rand();
int ungetc(int c, FILE* stream);
double ldexp(double x, int exp);
int close(int fildes);
int unlink(const char *path);
int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);
int open(const char *path, int oflag, ...);
pid_t getpid(void);
char *getenv(const char *name);
int feof(FILE *stream);
int fileno(FILE *stream);
FILE *fdopen(int fd, const char *mode);
double round(double x);
FILE *popen(const char *command, const char *type);
int pclose(FILE *stream);
struct tm *localtime_r(const time_t *timep, struct tm *result);

#endif
#endif
