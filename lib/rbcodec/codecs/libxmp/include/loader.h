
#ifndef __XMP_LOADER_H
#define __XMP_LOADER_H

#include <stdio.h>
#include "../lib/rbcodec/codecs/libxmp/include/list.h"

struct xmp_loader_info {
	char *id;
	char *name;
	int (*test)(FILE *, char *, const int);
	int (*loader)(struct xmp_context *, FILE *, const int);
	int enable;
	struct list_head list;
};

#endif

