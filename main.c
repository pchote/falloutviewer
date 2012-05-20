//
//  main.c
//
//
//  Created by Paul Chote on 19/05/12.
//  Copyright (c) 2012. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "dat2reader.h"
#include "frmreader.h"
#include "palreader.h"

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

void dump_frm(dat2reader *reader, char *frm_name, char *pal_name, char *filename)
{
    dat2entry *frm_entry = dat2reader_find_entry(reader, frm_name);
    if (!frm_entry)
    {
        fprintf(stderr, "Unable to find file\n");
        return;
    }

    uint8_t *frm_data = dat2entry_extract_data(frm_entry);
    frmreader *frm = frmreader_from_data(frm_data);
    free(frm_data);

    dat2entry *pal_entry = dat2reader_find_entry(reader, pal_name);
    if (!pal_entry)
    {
        fprintf(stderr, "Unable to find file\n");
        return;
    }
    uint8_t *pal_data = dat2entry_extract_data(pal_entry);
    palreader *pal = palreader_from_data(pal_data);
    free(pal_data);

    palreader_export_png(pal, frm_get_framedata(frm, 0, 0), frm->width, frm->height, filename);
    palreader_free(pal);
    frmreader_free(frm);
}

void dump_artwork(dat2reader *reader)
{
    dat2entry *pal_entry = dat2reader_find_entry(reader, "color.pal");
    if (!pal_entry)
    {
        fprintf(stderr, "Unable to find file\n");
        return;
    }
    uint8_t *pal_data = dat2entry_extract_data(pal_entry);
    palreader *pal = palreader_from_data(pal_data);
    free(pal_data);

    for (size_t i = 0; i < reader->entry_count; i++)
    {
        if (strcasestr(reader->entries[i].filename, ".frm") ||
            strcasestr(reader->entries[i].filename, ".FRM"))
        {
            // Take the file component and replace frm -> png
            char *c = strrchr(reader->entries[i].filename, '\\');
            char *png = strdup(++c);
            size_t end = strlen(png);
            strcpy(&png[end-3], "png");
            printf("%s\n", png);

            uint8_t *frm_data = dat2entry_extract_data(&reader->entries[i]);
            frmreader *frm = frmreader_from_data(frm_data);
            palreader_export_png(pal, frm_get_framedata(frm, 0, 0), frm->width, frm->height, png);
            frmreader_free(frm);
            free(frm_data);

            free(png);
        }
    }
    palreader_free(pal);
}

int main(int argc, char **argv)
{
    dat2reader *reader = dat2reader_open("master.dat");
    if (!reader)
        return 1;

    //print_entry_table(reader);
    //extract_file(reader, "art\\scenery\\verti01.frm", "verti01.frm");
    //dump_frm(reader, "art\\scenery\\verti01.frm", "color.pal", "0.png");
    dump_artwork(reader);

    dat2reader_close(reader);
    return 0;
}
