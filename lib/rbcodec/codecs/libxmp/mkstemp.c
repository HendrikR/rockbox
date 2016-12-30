#include "rockbox_xmp.h"
#ifdef __SUNPRO_C
#pragma error_messages (off,E_EMPTY_TRANSLATION_UNIT)
#endif

#ifndef HAVE_MKSTEMP

/*
 * Copyright (c) 1995, 1996, 1997 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <string.h>

#include <fcntl.h>
#include <errno.h>

#ifdef _MSC_VER
#include <process.h>
#define getpid _getpid
#define open _open
typedef int pid_t;
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

int mkstemp(char *template)
{
	int start, i;
	pid_t val;

	val = getpid();
	start = strlen(template) - 1;

	while (template[start] == 'X') {
		template[start] = '0' + val % 10;
		val /= 10;
		start--;
	}

	do {
		int fd;
		fd = open(template, O_RDWR | O_CREAT | O_EXCL | O_BINARY, 0600);
		if (fd >= 0 || errno != EEXIST)
			return fd;
		i = start + 1;
		do {
			if (template[i] == 0)
				return -1;
			template[i]++;
			if (template[i] == '9' + 1)
				template[i] = 'a';
			if (template[i] <= 'z')
				break;
			template[i] = 'a';
			i++;
		} while (1);
	} while (1);
}

#endif
