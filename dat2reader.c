//
//  dat2reader.c
//  Reads Fallout 2 dat files
//
//  Created by Paul Chote on 19/05/12.
//  Copyright (c) 2012. All rights reserved.
//

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "dat2reader.h"

//
// Open a Fallout 2 dat file and cache the file entries
// Returns NULL if there is an error
//
dat2reader *dat2reader_open(char *path)
{
    dat2reader *reader = malloc(sizeof(dat2reader));
    if (!reader)
        return NULL;
    
    reader->file = fopen(path, "r");
    if (!reader->file)
    {
        fprintf(stderr, "Malloc error: %s\n", strerror(errno));
        goto malloc_error;
    }

    // Find and seek to the filename block
    if (fseeko(reader->file, -8, SEEK_END))
    {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        goto seek_error;
    }

    uint32_t entry_offset;
    fread(&entry_offset, sizeof(uint32_t), 1, reader->file);
    fread(&reader->filesize, sizeof(uint32_t), 1, reader->file);
    
    // Jump to entry count, 4 bytes before the file data offset
    if (fseek(reader->file, reader->filesize - entry_offset - 8, SEEK_SET))
    {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        goto seek_error;
    }

    // Read entry count
    fread(&reader->entry_count, sizeof(uint32_t), 1, reader->file);
    
    // Read file entries
    reader->entries = malloc(reader->entry_count*sizeof(dat2entry));
    if (!reader->entries)
        goto entry_malloc_error;

    for (uint32_t i = 0; i < reader->entry_count; i++)
    {
        dat2entry *entry = &reader->entries[i];
        entry->reader = reader;
        uint32_t name_length;
        fread(&name_length, sizeof(uint32_t), 1, reader->file);

        entry->filename = malloc((name_length + 1)*sizeof(char));
        if (!entry->filename)
        {
            for (uint32_t j = 0; j < i; j++)
                free(reader->entries[j].filename);
            free(reader->entries);
            goto entry_error;
        }

        fread(entry->filename, sizeof(char), name_length, reader->file);
        entry->filename[name_length] = '\0';        
        fread(&entry->compressed, sizeof(uint8_t), 1, reader->file);
        fread(&entry->uncompressed_size, sizeof(uint32_t), 1, reader->file);
        fread(&entry->compressed_size, sizeof(uint32_t), 1, reader->file);
        fread(&entry->offset, sizeof(uint32_t), 1, reader->file);
    }

    return reader;

entry_error:
    free(reader->entries);
entry_malloc_error:
seek_error:
    fclose(reader->file);
malloc_error:
    free(reader);
    return NULL;
}

//
// Release resources associated with a dat2reader
//
void dat2reader_close(dat2reader *reader)
{
    fclose(reader->file);
    for (uint32_t i = 0; i < reader->entry_count; i++)
    {
        free(reader->entries[i].filename);
        reader->entries[i].reader = NULL;
    }
    free(reader->entries);
    free(reader);
}

//
// Find an entry with a given filename
// Returns a pointer to the entry, or NULL if not found
//
dat2entry *dat2reader_find_entry(dat2reader *reader, char *filename)
{
    // TODO: This could be smarter
    for (uint32_t i = 0; i < reader->entry_count; i++)
    {
        dat2entry *entry = &reader->entries[i];
        if (strcmp(filename, entry->filename) == 0)
            return entry;
    }
    return NULL;
}

//
// Extract (and if necessary, decompress) the data for a given entry
// Returns an allocated byte array, or NULL on error
//
uint8_t *dat2entry_extract_data(dat2entry *entry)
{
    if (!entry->reader)
        return NULL;

    // TODO: support compressed entries
    if (entry->compressed)
    {
        fprintf(stderr, "Compressed entries are currently unsupported\n");
        return NULL;
    }

    if (fseek(entry->reader->file, entry->offset, SEEK_SET))
    {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return NULL;
    }

    uint8_t *data = malloc(entry->uncompressed_size*sizeof(uint8_t));
    if (!data)
    {
        fprintf(stderr, "Malloc error: %s\n", strerror(errno));
        return NULL;
    }

    size_t read = fread(data, sizeof(uint8_t), entry->uncompressed_size, entry->reader->file);
    if (read != entry->uncompressed_size)
    {
        fprintf(stderr, "Extracted file length mismatch\n");
        free(data);
        return NULL;
    }

    return data;
}