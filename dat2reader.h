/*
 * dat2reader.h
 * Reads Fallout 2 .dat files
 *
 * Copyright (c) 2012, Paul Chote
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _dat2reader_h
#define _dat2reader_h

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

struct dat2reader;

typedef struct
{
    struct dat2reader *reader;
    char *filename;
    bool compressed;
    uint32_t uncompressed_size;
    uint32_t compressed_size;
    uint32_t offset;
} dat2entry;

typedef struct dat2reader
{
    FILE *file;
    uint32_t filesize;

    uint32_t entry_count;
    dat2entry *entries;
} dat2reader;

dat2reader *dat2reader_open(char *path);
void dat2reader_close(dat2reader *reader);
dat2entry *dat2reader_find_entry(dat2reader *reader, char *filename);


uint8_t *dat2entry_extract_data(dat2entry *entry);
#endif
