//
//  palreader.h
//
//
//  Created by Paul Chote on 20/05/12.
//  Copyright (c) 2012. All rights reserved.
//

#ifndef _palreader_h
#define _palreader_h

#include <stdint.h>

typedef enum
{
    PALTYPE_NIGHT = 1,
    PALTYPE_MORNING = 2,
    PALTYPE_AFTERNOON = 3,
    PALTYPE_NOON = 4,
} palreader_brightness;

typedef struct
{
    uint8_t data[768];
    palreader_brightness brightness;
} palreader;

palreader *palreader_from_data(uint8_t *data);
void palreader_free(palreader *reader);
int palreader_export_png(palreader *reader, uint8_t *data, uint16_t width, uint16_t height, const char *path);

#endif
