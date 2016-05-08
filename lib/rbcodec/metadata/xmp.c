/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2016 Hendrik Radke
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>
#include "platform.h"

#include "metadata.h"
#include <string-extra.h>
#include "metadata_common.h"
#include "metadata_parsers.h"
#include "rbunicode.h"
#include "codecs/libxmp/xmp.h"

bool get_xmp_metadata(int fd, struct mp3entry* id3)
{
    // TODO: ugly temporary metadata. see below.
    id3->title = "libxmp test";
    id3->bitrate = filesize(fd)/1024; // size in kb
    id3->frequency = 44100;
    id3->length = 10*1000;
    id3->vbr = false;
    id3->filesize = filesize(fd);
    
    // TODO: this would be the good variant, but does not work because of unsuitable makefiles
    //xmp_context ctx = xmp_create_context();
    /*
    // TODO: Write this function which is like xmp_load_module, but takes a file descriptor instead of a file name.
    if (xmp_load_module_fd(ctx, id3->path, fd) < 0) {
	return false;
    }
    struct xmp_module_info* modinfo = xmp_get_module_info(ctx, NULL);
    id3->title = modinfo->name;
    id3->bitrate = filesize(fd)/1024; // size in kb
    id3->frequency = 44100;
    id3->length = modinfo->time;
    id3->vbr = false;
    id3->filesize = filesize(fd);
    */

    /*
    xmp_deinit_formats(ctx);
    xmp_deinit(ctx);
    */
    return true;
}
