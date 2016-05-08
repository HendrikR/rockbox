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
#include "libxmp/xmp.h"

CODEC_HEADER

extern struct xmp_drv_info drv_smix;

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
enum codec_status codec_run(void) {
    struct xmp_module_info mod_info;
    struct xmp_frame_info info;
    xmp_context ctx;
    char* filename;
    
    if (codec_init()) {
        DEBUGF("codec init failed\n");
        return CODEC_ERROR;
    }

    ctx = xmp_create_context();

    filename = ci->id3->path;
    if (xmp_load_module(ctx, filename) < 0) {
	LOGF("loading file %s failed.\n", filename);
	return CODEC_ERROR;
    }
    
    xmp_get_module_info(ctx, &mod_info);
    xmp_get_frame_info(ctx, &info);
    DEBUGF("now playing: %s, type %s, length %d seconds\n",
	   mod_info.mod->name, mod_info.mod->type, info.time / 1000);


    xmp_start_player(ctx, 44100, XMP_FORMAT_MONO);
    xmp_set_player(ctx, XMP_PLAYER_MIX, 100);
    xmp_set_player(ctx, XMP_PLAYER_INTERP, XMP_INTERP_SPLINE);

    ci->configure(DSP_SET_FREQUENCY, 44100);
    ci->configure(DSP_SET_SAMPLE_DEPTH, 16);
    ci->configure(DSP_SET_STEREO_MODE, STEREO_MONO);
    ci->id3->length = info.time;
    ci->set_elapsed(0);

    ci->seek_buffer(0);
    uint32_t elapsedtime = 0;

    while (xmp_play_frame(ctx) == 0) {
	xmp_get_frame_info(ctx, &info);
	ci->pcmbuf_insert(info.buffer, NULL, info.buffer_size / 2);

	elapsedtime += info.frame_time;
	ci->set_elapsed(elapsedtime);
    }
    xmp_end_player(ctx);
    xmp_free_context(ctx);

    return CODEC_OK;
}
