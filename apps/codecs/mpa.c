/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2005 Dave Chapman
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#include "codeclib.h"
#include <codecs/libmad/mad.h>
#include <inttypes.h>

CODEC_HEADER

struct mad_stream stream IBSS_ATTR;
struct mad_frame frame IBSS_ATTR;
struct mad_synth synth IBSS_ATTR;

/* The following function is used inside libmad - let's hope it's never
   called. 
*/

void abort(void) {
}

#define INPUT_CHUNK_SIZE   8192

mad_fixed_t mad_frame_overlap[2][32][18] IBSS_ATTR;
unsigned char mad_main_data[MAD_BUFFER_MDLEN] IBSS_ATTR;
/* TODO: what latency does layer 1 have? */
int mpeg_latency[3] = { 0, 481, 529 };
int mpeg_framesize[3] = {384, 1152, 1152};

void init_mad(void)
{
    ci->memset(&stream, 0, sizeof(struct mad_stream));
    ci->memset(&frame, 0, sizeof(struct mad_frame));
    ci->memset(&synth, 0, sizeof(struct mad_synth));

    mad_stream_init(&stream);
    mad_frame_init(&frame);
    mad_synth_init(&synth);

    /* We do this so libmad doesn't try to call codec_calloc() */
    ci->memset(mad_frame_overlap, 0, sizeof(mad_frame_overlap));
    frame.overlap = &mad_frame_overlap;
    stream.main_data = &mad_main_data;
}

int get_file_pos(int newtime)
{
    int pos = -1;
    struct mp3entry *id3 = ci->id3;

    if (id3->vbr) {
        if (id3->has_toc) {
            /* Use the TOC to find the new position */
            unsigned int percent, remainder;
            int curtoc, nexttoc, plen;

            percent = (newtime*100) / id3->length;
            if (percent > 99)
                percent = 99;

            curtoc = id3->toc[percent];

            if (percent < 99) {
                nexttoc = id3->toc[percent+1];
            } else {
                nexttoc = 256;
            }

            pos = (id3->filesize/256)*curtoc;

            /* Use the remainder to get a more accurate position */
            remainder   = (newtime*100) % id3->length;
            remainder   = (remainder*100) / id3->length;
            plen        = (nexttoc - curtoc)*(id3->filesize/256);
            pos        += (plen/100)*remainder;
        } else {
            /* No TOC exists, estimate the new position */
            pos = (id3->filesize / (id3->length / 1000)) *
                (newtime / 1000);
        }
    } else if (id3->bitrate) {
        pos = newtime * (id3->bitrate / 8);
    } else {
        return -1;
    }

    pos += id3->first_frame_offset;

    /* Don't seek right to the end of the file so that we can
       transition properly to the next song */
    if (pos >= (int)(id3->filesize - id3->id3v1len))
        pos = id3->filesize - id3->id3v1len - 1;

    return pos;
}

/* this is the codec entry point */
enum codec_status codec_main(void)
{
    int status;
    size_t size;
    int file_end;
    int samples_to_skip; /* samples to skip in total for this file (at start) */
    char *inputbuffer;
    int64_t samplesdone;
    int stop_skip, start_skip;
    int current_stereo_mode = -1;
    unsigned long current_frequency = 0;
    int framelength;
    int padding = MAD_BUFFER_GUARD; /* to help mad decode the last frame */

    if (codec_init())
        return CODEC_ERROR;

    /* Create a decoder instance */

    ci->configure(DSP_SET_SAMPLE_DEPTH, MAD_F_FRACBITS);

next_track:
    status = CODEC_OK;

    /* Reinitializing seems to be necessary to avoid playback quircks when seeking. */
    init_mad();

    file_end = 0;
    while (!*ci->taginfo_ready && !ci->stop_codec)
        ci->sleep(1);

    ci->configure(DSP_SWITCH_FREQUENCY, ci->id3->frequency);
    current_frequency = ci->id3->frequency;
    codec_set_replaygain(ci->id3);
    
    if (ci->id3->offset)
        ci->seek_buffer(ci->id3->offset);
    else
        ci->seek_buffer(ci->id3->first_frame_offset);

    if (ci->id3->lead_trim >= 0 && ci->id3->tail_trim >= 0) {
        stop_skip = ci->id3->tail_trim - mpeg_latency[ci->id3->layer];
        if (stop_skip < 0) stop_skip = 0;
        start_skip = ci->id3->lead_trim + mpeg_latency[ci->id3->layer];
    } else {
        stop_skip = 0;
        /* We want to skip this amount anyway */
        start_skip = mpeg_latency[ci->id3->layer];
    }

    /* Libmad will not decode the last frame without 8 bytes of extra padding
       in the buffer. So, we can trick libmad into not decoding the last frame
       if we are to skip it entirely and then cut the appropriate samples from
       final frame that we did decode. Note, if all tags (ID3, APE) are not
       properly stripped from the end of the file, this trick will not work. */
    if (stop_skip >= mpeg_framesize[ci->id3->layer]) {
        padding = 0;
        stop_skip -= mpeg_framesize[ci->id3->layer];
    } else {
        padding = MAD_BUFFER_GUARD;
    }

    samplesdone = ((int64_t)ci->id3->elapsed) * current_frequency / 1000;

    /* Don't skip any samples unless we start at the beginning. */
    if (samplesdone > 0)
        samples_to_skip = 0;
    else
        samples_to_skip = start_skip;

    framelength = 0;

    /* This is the decoding loop. */
    while (1) {
        ci->yield();
        if (ci->stop_codec || ci->new_track)
            break;
    
        if (ci->seek_time) {
            int newpos;

            samplesdone = ((int64_t)(ci->seek_time-1))*current_frequency/1000;

            if (ci->seek_time-1 == 0) {
                newpos = ci->id3->first_frame_offset;
                samples_to_skip = start_skip;
            } else {
                newpos = get_file_pos(ci->seek_time-1);
                samples_to_skip = 0;
            }

            if (!ci->seek_buffer(newpos))
                break;
            ci->seek_complete();
            init_mad();
            framelength = 0;
        }

        /* Lock buffers */
        if (stream.error == 0) {
            inputbuffer = ci->request_buffer(&size, INPUT_CHUNK_SIZE);
            if (size == 0 || inputbuffer == NULL)
                break;
            mad_stream_buffer(&stream, (unsigned char *)inputbuffer,
                              size + padding);
        }

        if (mad_frame_decode(&frame, &stream)) {
            if (stream.error == MAD_FLAG_INCOMPLETE 
                || stream.error == MAD_ERROR_BUFLEN) {
                /* This makes the codec support partially corrupted files */
                if (file_end == 30)
                    break;

                /* Fill the buffer */
                if (stream.next_frame)
                    ci->advance_buffer_loc((void *)stream.next_frame);
                else
                    ci->advance_buffer(size);
                stream.error = 0;
                file_end++;
                continue;
            } else if (MAD_RECOVERABLE(stream.error)) {
                continue;
            } else {
                /* Some other unrecoverable error */
                status = CODEC_ERROR;
                break;
            }
            break;
        }

        file_end = 0;

        /* Do the pcmbuf insert here. Note, this is the PREVIOUS frame's pcm
           data (not the one just decoded above). When we exit the decoding
           loop we will need to process the final frame that was decoded. */
        if (framelength > 0) {
            /* In case of a mono file, the second array will be ignored. */
            ci->pcmbuf_insert(&synth.pcm.samples[0][samples_to_skip],
                              &synth.pcm.samples[1][samples_to_skip],
                              framelength);

            /* Only skip samples for the first frame added. */
            samples_to_skip = 0;
        }

        mad_synth_frame(&synth, &frame);

        /* Check if sample rate and stereo settings changed in this frame. */
        if (frame.header.samplerate != current_frequency) {
            current_frequency = frame.header.samplerate;
            ci->configure(DSP_SWITCH_FREQUENCY, current_frequency);
        }
        if (MAD_NCHANNELS(&frame.header) == 2) {
            if (current_stereo_mode != STEREO_NONINTERLEAVED) {
                ci->configure(DSP_SET_STEREO_MODE, STEREO_NONINTERLEAVED);
                current_stereo_mode = STEREO_NONINTERLEAVED;
            }
        } else {
            if (current_stereo_mode != STEREO_MONO) {
                ci->configure(DSP_SET_STEREO_MODE, STEREO_MONO);
                current_stereo_mode = STEREO_MONO;
            }
        }

        if (stream.next_frame)
            ci->advance_buffer_loc((void *)stream.next_frame);
        else
            ci->advance_buffer(size);

        framelength = synth.pcm.length - samples_to_skip;
        if (framelength < 0) {
            framelength = 0;
            samples_to_skip -= synth.pcm.length;
        }

        samplesdone += framelength;
        ci->set_elapsed(samplesdone / (current_frequency / 1000));
    }

    /* Finish the remaining decoded frame.
       Cut the required samples from the end. */
    if (framelength > stop_skip)
        ci->pcmbuf_insert(synth.pcm.samples[0], synth.pcm.samples[1],
                          framelength - stop_skip);

    stream.error = 0;

    if (ci->request_next_track())
        goto next_track;

    return status;
}
