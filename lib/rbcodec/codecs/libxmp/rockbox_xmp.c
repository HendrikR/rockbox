#ifndef LIBXMP_ROCKBOX_XMP_H
#define LIBXMP_ROCKBOX_XMP_H

#include "rockbox_xmp.h"
#include "stdio.h"
#include "string.h"
#include <time.h>
#include <stdarg.h> // for va_list / va_start
#include <setjmp.h>
#include "fixedpoint.h" // for fixed-point arithmetics
#include "string-extra.h"
#include "codeclib.h"

#define UNUSED(identifier) identifier __attribute__((unused))


//#ifndef M_LOG10
double sin(double x) {
    // HRTODO: Umstellen auf fp14_sin/fp14_cos?
    long fix, cos;
    double FULL = 6.283185307179586;
    while (x <    0) x += FULL;
    while (x > FULL) x -= FULL;
    fix = (x / FULL) * 0xFFFFFFFF;
    fix = fp_sincos(fix, &cos);
    return (fix / (double)0xFFFFFFFF) * FULL;
}

double cos(double x) {
    long fix, cos;
    double FULL = 6.283185307179586;
    while (x <    0) x += FULL;
    while (x > FULL) x -= FULL;
    fix = (x / FULL) * 0xFFFFFFFF;
    fp_sincos(fix, &cos);
    return (cos / (double)0xFFFFFFFF) * FULL;
}

double log10(double x) {
    long fix = x * 0xFFFF;
    fix = fp_log10(fix,16);
    return fix / (double)0xFFFF;
}

double log(double x) {
    long fix = x * 0xFFFF;
    fix = fp16_log(fix);
    return fix / (double)0xFFFF;
}

double exp(double x) {
    long fix = x * 0xFFFF;
    fix = fp16_exp(fix);
    return fix / (double)0xFFFF;
}

double floor(double x) {
    long fix = x;
    return x - (double)fix;
}

double pow(double x, double y) {
    return exp(log(x)*y);
}

double ldexp(double x, int exp) {
    return x * pow(2, exp);
}

double round(double x) {
    double ix = floor(x);
    if (x-ix <= 0.5) return ix;
    else return ix+1.0;
}
//#endif


#if !defined __FILE_defined
#define UNUSED(identifier) identifier __attribute__((unused))


const unsigned char _ctype_[257];
// HRTODO: The following types are actually declared in rockbox_xmp.h. Why do I have to redeclare here?
typedef int FILE;
typedef int pid_t;
#define getpid _getpid

#define S_ISDIR(st_mode) 0
struct stat {
    unsigned long st_size;
    unsigned short st_mode;
};

int* __errno(void) {
    return 0;
}


int fstat(int UNUSED(fileno), struct stat* stat) {
    //size_t filesize;
    //ci->request_buffer(&filesize, 0xFFFF);
    stat->st_size = ci->id3->filesize;
    stat->st_mode = 1;
    return 0;
}

int stat(const char* UNUSED(filename), struct stat* stat) {
    return fstat(0, stat);
}

char* strncpy(char* dest, const char* src, size_t UNUSED(n)) {
    //strlcpy(dest, src, n);
    strcpy(dest,src);
    return dest;
}

char* strdup(const char* s) {
    char* ret = (char*)malloc(strlen(s)*sizeof(char));
    return strcpy(ret, s);
}

int strncmp(const char* s1, const char* s2, size_t UNUSED(n)) {
    //return strlcmp(s1, s2, n);
    return strcmp(s1, s2);
}

char* strncat(char* dest, const char* src, UNUSED(size_t) n) {
    //strlcat(dest, src, n);
    strcat(dest, src);
    return dest;
}

char* strchr(const char* UNUSED(s), int UNUSED(c)) {
    return 0;//HRTODO
    //return index(s, c);
}

char* strrchr(const char* UNUSED(s), int UNUSED(c)) {
    //HRTODO
    return 0;
    //return rindex(s, c);
}

// HRTODO: This def of (v)snprintf is rather convoluted. Is there a better way?
size_t write(int fd, const void* letter, size_t n) {
    return 1;
}
#include "common/format.c"
#include "libc/sprintf.c"

/*
int vsnprintf(char* UNUSED(str), size_t size, const char* UNUSED(format), __gnuc_va_list UNUSED(ap)) {
    // HRTODO
    return size;
}

int snprintf(char* str, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vsnprintf(str, size, format, args);
    va_end(args);
    LOGF("snprintf yields '%s'", str);
    return ret;
}
*/

int printf(char* UNUSED(str), const char* UNUSED(format), ...) {
        return 0;//HRTODO
}

FILE* fopen(const char* UNUSED(fpath), const char* UNUSED(fmode)) {
    // there is only one file, which is already open (ci->request_buffer).
    FILE* x = (FILE*)malloc(sizeof(FILE));
    *x = 5;
    return x;
}

FILE* fdopen(int UNUSED(fd), const char* UNUSED(mode)) {
    // there is only one file, which is already open (ci->request_buffer).
    FILE* x = (FILE*)malloc(sizeof(FILE));
    *x = 5;
    return x;
}

int open(const char* UNUSED(path), int UNUSED(oflag), ...) {
    // there is only one file, which is already open (ci->request_buffer).
    return 5;
}

FILE* popen(const char* UNUSED(command), const char* UNUSED(type)) {
    // we can't execute external commands or open pipes.
    FILE* x = (FILE*)malloc(sizeof(FILE));
    *x = 5;
    return x;
}

int pclose(FILE* UNUSED(stream)) {
    // we can't execute external commands or open pipes.
    return 0;
}

int close(int UNUSED(fildes)) {
    // there is only one file, rockbox cares about opening/closing it.
    return 0;
}

int fclose(FILE* UNUSED(stream)) {
    // there is only one file, rockbox cares about opening/closing it.
    return 0;
}

int fileno(FILE* UNUSED(stream)) {
    // there is only one file, which is already open (ci->request_buffer).
    return 5;
}

int fread(void* ptr, size_t size, size_t nmemb, FILE* UNUSED(stream)) {
    return ci->read_filebuf(ptr, size*nmemb) / size;
}

int fwrite(const void* UNUSED(ptr), size_t UNUSED(size), size_t UNUSED(nmemb), FILE* UNUSED(stream)) {
    // writing is not allowed
    return nmemb;
}

int fgetc(FILE* UNUSED(stream)) {
    char ret;
    ci->read_filebuf(&ret, 1);
    return ret;
}

int getc (FILE* UNUSED(stream)) {
    return fgetc(stream);
}

int fputc(int c, FILE* UNUSED(stream)) {
    // writing is not allowed, just return the char.
    return c;
}

int fseek(FILE* UNUSED(stream), long offset, int whence) {
    if (whence == SEEK_SET) {
	offset += 0;
    } else if (whence == SEEK_CUR) {
	offset += ci->curpos;
    } else { // SEEK_END
	offset += ci->filesize;
    }
    if (ci->seek_buffer(offset)) {
	return 0;
    } else {
	return -1;
    }
}

long ftell(FILE* UNUSED(stream)) {
    return ci->curpos;
}

int ferror(FILE* UNUSED(stream)) {
    return 0;//HRTODO
}

int fflush(FILE* UNUSED(stream)) {
    // we do not write to the buffer, so flushing is pointless.
    return 0;
}


int unlink(const char* UNUSED(path)) {
    // the codec does not create or delete files.
    return 0;
}

int rand() {
    return 4;//HRTODO: choose by fair dice roll
}

int feof(FILE* UNUSED(stream)) {
    size_t realsize;
    ci->request_buffer(&realsize, 4);
    if (realsize == 0) return 1;
    else return 0;
}

char* getenv(const char* name) {
    if (strcmp(name, "XMP_INSTRUMENT_PATH") == 0) {
	return "";
    } else if (strcmp(name, "TEMP") == 0) {
	return "";
    } else if (strcmp(name, "TMPDIR") == 0) {
	return "";
    } else {
	DEBUGF("libxmp getenv: unknown variable %s", name);
	return "";
    }
}

int ungetc(int c, FILE* UNUSED(stream)) {
    size_t realsize;
    ci->advance_buffer(-1);
    char* buf = (char*)ci->request_buffer(&realsize, 1);
    buf[0] = c;
    return c;
}

int setjmp(jmp_buf UNUSED(env)) {
    return 0;//HRTODO
}

void longjmp(jmp_buf UNUSED(env), int UNUSED(val)) {
    return ;//HRTODO
}

struct tm* localtime_r(const time_t* UNUSED(timep), struct tm* result) {
    // HRTODO
    // localtime_r is only needed to construct a version number of Schism Tracker for .IT files.
    // Just returning zero should be enough for this.
    result->tm_sec  = result->tm_min  = result->tm_hour  = 0;
    result->tm_mday = result->tm_mon  = result->tm_year  = 0;
    result->tm_wday = result->tm_yday = result->tm_isdst = 0;
    return result;
}

pid_t _getpid(void) {
    // this is only needed to name temp files, so "0" is an okay pid.
    return 0;
}

#endif
#endif
