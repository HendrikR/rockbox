#!/bin/sh
# Change the includes within libxmp point to the correct directory
find -name \*.c -or -name \*.h | xargs sed -i 's!#[ 	]*include "\(effects\|hio\|list\|period\|common\|tempfile\|format\|synth\|med_extras\|hmn_extras\|depackers/readlzw\|depacker\)\.h"!#include "libxmp/\1.h"!'
find loaders/ -name \*.c -or -name \*.h | xargs sed -i 's!#include "\(loader\|iff\)\.h"!#include "libxmp/loaders/\1.h"!'
### sed -i 's!#include <stdlib.h>!#include <stdlib.h>\n#include <stdio.h>!' loaders/prowizard/pp30.c
