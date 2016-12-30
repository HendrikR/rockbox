#ifndef XMP_PLATFORM_H
#define XMP_PLATFORM_H

#include "libxmp/common.h"

FILE *make_temp_file(char **);
void unlink_temp_file(char *);

#endif
