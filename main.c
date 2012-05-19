//
//  main.c
//
//
//  Created by Paul Chote on 19/05/12.
//  Copyright (c) 2012. All rights reserved.
//

#include <stdlib.h>
#include "dat2reader.h"
#include "frmreader.h"

void print_entry_table(dat2reader *reader)
{
    printf("Contains %d files.\n", reader->entry_count);
    printf("Filename                        Compressed   C-Size     Size   Offset\n");
    for (uint32_t i = 0; i < reader->entry_count; i++)
    {
        dat2entry *entry = &reader->entries[i];
        printf("%-40s %d %8d %8d %8d\n", entry->filename, entry->compressed, entry->compressed_size, entry->uncompressed_size, entry->offset);
    }
}

void extract_file(dat2reader *reader, char *entry_name, char *filename)
{
    dat2entry *e = dat2reader_find_entry(reader, entry_name);
    if (!e)
    {
        fprintf(stderr, "Unable to find file\n");
        return;
    }

    uint8_t *data = dat2entry_extract_data(e);
    if (!data)
        return;

    FILE *outfile = fopen(filename, "w+");
    if (!outfile)
    {
        free(data);
        return;
    }

    fwrite(data, sizeof(uint8_t), e->uncompressed_size, outfile);
    fclose(outfile);

    free(data);
}

extern int png_write_raw(char *filename, uint8_t *raw, uint16_t width, uint16_t height);
void dump_frm(dat2reader *reader, char *entry_name)
{
    dat2entry *e = dat2reader_find_entry(reader, entry_name);
    if (!e)
    {
        fprintf(stderr, "Unable to find file\n");
        return;
    }

    uint8_t *data = dat2entry_extract_data(e);
    frmreader *frm = frmreader_from_data(data);
    free(data);

    png_write_raw("test.png", frm_get_framedata(frm, 0, 0), frm->width, frm->height);
    frmreader_free(frm);
}

int main(int argc, char **argv)
{
    dat2reader *reader = dat2reader_open("master.dat");
    if (!reader)
        return 1;

    //print_entry_table(reader);
    extract_file(reader, "art\\scenery\\verti01.frm", "verti01.frm");
    dump_frm(reader, "art\\scenery\\verti01.frm");
    dat2reader_close(reader);

    return 0;
}
