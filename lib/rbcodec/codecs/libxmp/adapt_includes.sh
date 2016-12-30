#!/bin/sh
# Change the includes within libxmp to point to the correct directory
find -name \*.c -or -name \*.h | xargs sed -i 's!#[ 	]*include "\(effects\|hio\|list\|period\|common\|xmp\|tempfile\|format\|synth\|med_extras\|hmn_extras\|depackers/readlzw\|depacker\)\.h"!#include "libxmp/\1.h"!'
find loaders/ -name \*.c -or -name \*.h | xargs sed -i 's!#include "\(loader\|iff\)\.h"!#include "libxmp/loaders/\1.h"!'
### sed -i 's!#include <stdlib.h>!#include <stdlib.h>\n#include <stdio.h>!' loaders/prowizard/pp30.c

# sys/stat.h and unistd.h are not part of rockbox's firmware
find -name \*.c -or -name \*.h | xargs sed -i 's!#include <\(sys/stat\|unistd\).h>!!'

# use debug.error output instead of stderr
find -name \*.c | xargs sed -i 's!fprintf(stderr,!D_(D_CRIT!'

# Variable "uint8 nop" ("number of patterns") collides with ASM "nop" command in these files. Replace:
sed -i 's!nop!no_p!g' loaders/669_load.c loaders/prowizard/pha.c
# HRTODO: FILE* and the FILE*-based operations (fopen, fread, fclose ...) don't work on
# ARM architecture. replace them.
# find -name \*.c -or -name \*.h | xargs sed -i 's!FILE *\*!int !'
