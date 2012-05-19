//
//  dat2reader.h
//  Reads Fallout 2 dat files
//
//  Created by Paul Chote on 19/05/12.
//  Copyright (c) 2012. All rights reserved.
//


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

    uint8_t *data;
} dat2reader;

dat2reader *dat2reader_open(char *path);
void dat2reader_close(dat2reader *reader);
dat2entry *dat2reader_find_entry(dat2reader *reader, char *filename);


uint8_t *dat2entry_extract_data(dat2entry *entry);
#endif
