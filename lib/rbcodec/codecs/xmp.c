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
#include "libxmp/include/common.h"
#include "codecs/libxmp/include/xmp.h"
#include "codecs/libxmp/include/loader.h"

CODEC_HEADER

// normally in player/readrc.c
int _xmp_read_rc(struct xmp_context *ctx) {
    ctx->o.drv_id = "smix";
    //ctx->o->amplify = int???;
    ctx->o.resol = 16;
    //ctx->o->instrument_path = "???";
    //ctx->o->freq = xmp_cfg.mixing_freq;
}
void _xmp_read_modconf(struct xmp_context *ctx, uint32 crc, long size) {
}

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
    LOGF("libxmp test: codec_run\n");
    // code is oriented at libxmp's winamp plugin
    int n, dsp;
    int numch = 2;// opt->outfmt & XMP_FMT_MONO ? 1 : 2;
    int ssize = 2;// opt->resol / 8;
    int t, todo;
    char *data;
    int size;
    char* module;

    if (codec_init()) {
        DEBUGF("codec init failed\n");
        return CODEC_ERROR;
    }

    /*
    xmp_cfg.loop = 0;
    xmp_cfg.fixloops = 0;
    xmp_cfg.force_mono = 0;
    xmp_cfg.interpolation = 1;
    xmp_cfg.filter = 1;*/

    xmp_context ctx = xmp_create_context();
    xmp_init(ctx, 0, NULL);
    xmp_init_formats(ctx);

    struct xmp_options* opt = xmp_get_options(ctx);
    opt->resol = 16;
    opt->amplify = 0;
    opt->verbosity = 0;
    opt->drv_id = "smix";
    opt->freq = 44100;
    opt->mix = 80;
    opt->flags = XMP_CTL_FILTER | XMP_CTL_ITPT; // filtering and interpolation, no looping
    opt->outfmt = XMP_FMT_MONO; // no mono
    
    xmp_drv_register(&drv_smix);
    printf("smix driver registered\n");
    int ret = xmp_open_audio(ctx);
    if (ret != 0) {
	LOGF("error %d opening audio driver\n", ret);
	return CODEC_ERROR;
    }

    struct xmp_context* con = (struct xmp_context*) ctx;
    char* filename = ci->id3->path;
    ret = xmp_load_module(ctx, filename);
    if (ret < 0) {
	LOGF("loading file %s failed.\n", filename);
	return CODEC_ERROR;
    }

    struct xmp_module_info mod_info;
    xmp_get_module_info(ctx, &mod_info);
    DEBUGF("now playing: %s, type %s, length %d seconds\n",
	   mod_info.name, mod_info.type, mod_info.time / 1000);


    xmp_player_start(ctx);
    ci->configure(DSP_SET_FREQUENCY, 44100);
    ci->configure(DSP_SET_SAMPLE_DEPTH, 16);
    ci->configure(DSP_SET_STEREO_MODE, STEREO_MONO);
    ci->id3->length = mod_info.time;
    ci->set_elapsed(0);

    ci->seek_buffer(0);
    uint32_t elapsedtime = 0;
    while (xmp_player_frame(ctx) == 0) {
	xmp_get_buffer(ctx, (void*)&data, &size);
	ci->pcmbuf_insert(data, NULL, size / 2);
    }

    return CODEC_OK;
}
