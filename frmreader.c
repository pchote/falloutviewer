//
//  frmreader.c
//
//
//  Created by Paul Chote on 20/05/12.
//  Copyright (c) 2012. All rights reserved.
//

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "frmreader.h"

uint8_t read_u8(uint8_t **data)
{
    return *(*data)++;
}

// frm Data is big-endian
static uint16_t read_u16(uint8_t **data)
{
    uint8_t hi = *(*data)++;
    uint8_t lo = *(*data)++;
    return (hi << 8) | lo;
}

static int16_t read_s16(uint8_t **data)
{
    int8_t **sdata = (int8_t **)data;
    int8_t hi = *(*sdata)++;
    int8_t lo = *(*sdata)++;
    return (hi << 8) | lo;
}

static uint32_t read_u32(uint8_t **data)
{
    uint32_t ret = 0;
    for (int8_t i = 3; i >= 0; i--)
        ret |= *(*data)++ << 8*i;
    return ret;
}

frmreader *frmreader_from_data(uint8_t *data)
{
    frmreader *reader = malloc(sizeof(frmreader));
    if (!reader)
        return NULL;

    uint8_t *dp = data;
    reader->version = read_u32(&dp);
    reader->fps = read_u16(&dp);
    reader->action_frame = read_u16(&dp);
    reader->animation_length = read_u16(&dp);
    for (uint8_t i = 0; i < 6; i++)
        reader->x_origin[i] = read_s16(&dp);
    for (uint8_t i = 0; i < 6; i++)
        reader->y_origin[i] = read_s16(&dp);
    for (uint8_t i = 0; i < 6; i++)
        reader->animation_start[i] = read_u32(&dp);
    reader->data_length = read_u32(&dp);
    reader->width = read_u16(&dp);
    reader->height = read_u16(&dp);
    read_u32(&dp); // Skip size field
    reader->x = read_u16(&dp);
    reader->y = read_u16(&dp);

    // Take a local copy of the image data
    reader->data = malloc(reader->data_length*sizeof(uint8_t));
    memcpy(reader->data, dp, reader->data_length);

    return reader;
}

uint8_t *frm_get_framedata(frmreader *reader, uint8_t facing, uint8_t index)
{
    if (facing >= 6 || index >= reader->animation_length)
        return NULL;

    uint8_t offset = reader->animation_start[facing] + index*reader->width*reader->height;
    return &reader->data[offset];
}

void frmreader_free(frmreader *reader)
{
    free(reader->data);
    free(reader);
}