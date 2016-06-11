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

/* this is the codec entry point */
enum codec_status codec_main(enum codec_entry_call_reason reason)
{
    switch(reason) {
    case CODEC_LOAD:
	break;
    case CODEC_UNLOAD:
	break;
    }
    return CODEC_OK;
}

/* this is called for each file to process */
enum codec_status codec_run(void) {
    if (codec_init()) {
        DEBUGF("codec init failed\n");
        return CODEC_ERROR;
    }

    struct xmp_module_info mod_info;
    struct xmp_frame_info info;
    xmp_context ctx;
    char* filename;
    ctx = xmp_create_context();
    
    filename = ci->id3->path;
    if (xmp_load_module(ctx, filename) < 0) {
	LOGF("loading file %s failed.\n", filename);
	return CODEC_ERROR;
    }
    
    xmp_get_module_info(ctx, &mod_info);
    xmp_get_frame_info(ctx, &info);
    DEBUGF("now playing: %s, type %s, length %d seconds\n",
	   mod_info.mod->name, mod_info.mod->type, info.total_time / 1000);
    /* TODO: set metadata in metadata/xmp.c*/
    
    
    xmp_start_player(ctx, 44100, 0);
    /* TODO: set stereo separation from rockbox config. other things might also be configurable. */
    xmp_set_player(ctx, XMP_PLAYER_MIX, 70);
    xmp_set_player(ctx, XMP_PLAYER_INTERP, XMP_INTERP_SPLINE);
    
    /* struct codec_api* ci, see codecs.h */
    ci->id3->title = mod_info.mod->name;
    ci->id3->artist = "";
    ci->id3->album = "";
    ci->id3->comment = mod_info.comment;
    ci->id3->frequency = 44100;
    ci->id3->length = info.total_time;
    /* TODO: ci->id3->bitrate = 8000*ci->id3->filesize / ci->id3->length; or something like that */
    ci->id3->elapsed = 0;
    /* TODO: id3->samples, id3->frame_count, id3->bytesperframe */
    ci->id3->vbr = false;
    ci->id3->channels = 2;
    /* TODO: id3->offset and id3->index for resuming */
    
    ci->configure(DSP_SET_FREQUENCY, 44100);
    ci->configure(DSP_SET_SAMPLE_DEPTH, 16);
    ci->configure(DSP_SET_STEREO_MODE, STEREO_INTERLEAVED);
    
    ci->seek_buffer(0);

    uint32_t elapsedtime = 0;
    intptr_t param;
    enum codec_command_action action;
    while (1) {
	action = ci->get_command(&param);

	if (action == CODEC_ACTION_HALT) /* stopping */
            break;

	if (action == CODEC_ACTION_SEEK_TIME) { /* seeking */
	    elapsedtime = param;
	    xmp_seek_time(ctx, elapsedtime);
	    ci->seek_complete();
	}

	/* playing */
	if (xmp_play_frame(ctx) != 0)
	    break;
	xmp_get_frame_info(ctx, &info);
	ci->pcmbuf_insert(info.buffer, NULL, info.buffer_size / 4);

	elapsedtime += info.frame_time / 1000;
	ci->set_elapsed(elapsedtime);
	ci->yield();
    }

    xmp_end_player(ctx);
    xmp_free_context(ctx);

    return CODEC_OK;
}
