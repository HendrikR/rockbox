#!/bin/sh
# Change the includes within libxmp point to the correct directory
find -name \*.c -or -name \*.h | xargs sed -i 's!#include "\(common\|convert\|driver\|effects\|list\|loader\|mixer\|period\|readhuff\|readlzw\|readrle\|spectrum\|synth\|xmp\|xxm\)\.h"!#include "../lib/rbcodec/codecs/libxmp/include/\1.h"!'
