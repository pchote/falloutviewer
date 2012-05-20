//
//  palreader.c
//
//
//  Created by Paul Chote on 20/05/12.
//  Copyright (c) 2012. All rights reserved.
//

#include <stdlib.h>
#include <png.h>

#include "palreader.h"

palreader *palreader_from_data(uint8_t *data)
{
    palreader *reader = malloc(sizeof(palreader));
    if (!reader)
        return NULL;

    reader->brightness = PALTYPE_NOON;

    // Copy (r,g,b) triplets
    memcpy(reader->data, data, 256*3*sizeof(uint8_t));

    // Remaining data defines a cube to map arbitrary
    // RGB -> palette index, followed by up to 3
    // undefined additional tables
    // Ignore these for now
    return reader;
}

void palreader_free(palreader *reader)
{
    free(reader);
}

// Based on code from http://www.lemoda.net/c/write-png/
int palreader_export_png(palreader *reader, uint8_t *data, uint16_t width, uint16_t height, const char *path)
{
    int status = -1;
    FILE *fp = fopen(path, "wb");
    if (!fp)
        goto fopen_failed;

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
        goto png_create_write_struct_failed;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
        goto png_create_info_struct_failed;

    // Set up error handling
    if (setjmp(png_jmpbuf(png_ptr)))
        goto png_failure;

    // Set image attributes.
    png_set_IHDR(png_ptr, info_ptr, width, height, 8,
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // Initialize rows of PNG
    png_byte **row_pointers = png_malloc(png_ptr, height*sizeof(png_byte *));
    for (size_t y = 0; y < height; y++)
    {
        png_byte *row = png_malloc(png_ptr, 3*width*sizeof(uint8_t));
        row_pointers[y] = row;
        for (size_t x = 0; x < width; x++)
        {
            uint8_t index = data[y*width + x];
            for (size_t i = 0; i < 3; i++)
                *row++ = reader->data[3*index + i]*reader->brightness;
        }
    }

    // Write to file
    png_init_io(png_ptr, fp);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    // Cleanup
    status = 0;
    for (size_t y = 0; y < height; y++)
        png_free(png_ptr, row_pointers[y]);

    png_free(png_ptr, row_pointers);

png_failure:
png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
png_create_write_struct_failed:
    fclose (fp);
fopen_failed:
    return status;
}
