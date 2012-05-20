//
//  frmreader.h
//
//
//  Created by Paul Chote on 20/05/12.
//  Copyright (c) 2012. All rights reserved.
//

#ifndef _frmreader_h
#define _frmreader_h
typedef struct
{
    uint32_t version;
    uint16_t fps;
    uint16_t action_frame;
    uint16_t animation_length;
    int16_t  x_origin[6];
    int16_t  y_origin[6];
    uint32_t animation_start[6];
    uint32_t data_length;
    uint16_t width;
    uint16_t height;
    uint16_t x;
    uint16_t y;
    uint8_t *data;
} frmreader;

frmreader *frmreader_from_data(uint8_t *data);
void frmreader_free(frmreader *reader);
uint8_t *frm_get_framedata(frmreader *reader, uint8_t facing, uint8_t index);


#endif
