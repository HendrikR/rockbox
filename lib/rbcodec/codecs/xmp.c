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

#include "codeclib.h"

CODEC_HEADER

/* this is the codec entry point */
enum codec_status codec_main(enum codec_entry_call_reason reason)
{
    LOGF("libxmp test: codec_main\n");
    switch(reason) {
    CODEC_LOAD: break;
    }
    return CODEC_OK;
}

/* this is called for each file to process */
enum codec_status codec_run(void)
{
    LOGF("libxmp test: codec_run\n");
/* todo: the loop should look something like that
uint32_t elapsedtime = 0;
ci->seek_buffer(ci->id3->first_frame_offset);
while (something) {
ci->set_elapsed(elapsedtime);
ci->yield();
ci->advance_buffer();
elapsedtime = ci->id3->elapsed;
}
*/
    
/* some helper functions: debug & logging. where does this go, anyway? */
//LOGF("other logging function\n");
//DEBUGF("read_packet error %d, errcount %d\n",wmares, errcount);
//return CODEC_ERROR;
    return CODEC_OK;
}
