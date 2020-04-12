/*
 * Tire - Tiny Image Recoverer
 * Copyright (c) 2020 Danilo Lemos
 * License: https://github.com/danilolc/Tire/blob/master/LICENSE
 */

// Usage:
// ./Tire FILE [start] [base]

// FILE - the file containing the raw data - use /dev/sdx on linux to search on disk
// start - position to start looking for jpg images
// base - the positional base that start is written - default is base 10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bool _Bool

FILE* raw;

const long BUFFER_SIZE = 1 << 25; //~33 MB
unsigned char* image_buffer;
unsigned long buffer_start = 0;

void save_buffer() {

    char name[32];
    sprintf(name, "%lu", buffer_start);
    strcat(name, ".jpg");

    FILE *fp = fopen(name, "wb");
    if (!fp) {

        printf("Can't save file.\n");
        return;

    }

    fwrite(image_buffer, 1, ftell(raw) - buffer_start, fp);
    
    fclose(fp);

}

bool get_image() {

    unsigned char c;

    if (fread(&c, 1, 1, raw) == 0)
        return 0;

    if (c != 0xFF)
        return 0;

    buffer_start = ftell(raw) - 3;
    printf("Image at %f GB ---- ", (double)buffer_start / (1 << 30));

    bool ff = 1;
    
    while (1) {

        if (fread(&c, 1, 1, raw) == 0) {

            printf("EOF\n");
            return 0;

        }

        if ((ftell(raw) - buffer_start) >= BUFFER_SIZE) {
            // Buffer has finnished

            printf("too big\n");
            return 0;

        }

        image_buffer[ftell(raw) - buffer_start - 1] = c;
        
        if (ff && c == 0xD9) {

            // Image must be at least 1 kB
            if (ftell(raw) - buffer_start > 1 << 10) {

                printf("%lu.jpg\n", buffer_start);
                save_buffer();

            } else {
                
                printf("too small\n");

            }
            
            return 0;

        } else if (ff && c == 0xD8) {
            // Found a image starter, try again

            printf("found 0xD8\n");
            return 1;

        } else if (ff && c != 0x00 && c < 0xC0) {
            // Invalid marker

            printf("marker 0x%x\n", c);
            return 0;

        }

        ff = (c == 0xFF);

    }    

}

int main(int argc, char** argv) {

    image_buffer = malloc(BUFFER_SIZE);
    image_buffer[0] = 0xFF;
    image_buffer[1] = 0xD8;
    image_buffer[2] = 0xFF;
    
    if (argc == 0) {

        printf("Usage:\n ./Tire FILE [start] [base]\n");
        return 1;

    }

    raw = fopen(argv[1], "rb");
    if (!raw) {

        printf("Can't open %s\n", argv[1]);
        return 1;        

    }

    if (argc > 2) {
        
        int base = 10;
        if (argc > 3)
            base = strtol(argv[2], NULL, 10);

        unsigned long pos = strtol(argv[1], NULL, base);
        fseek(raw, pos, SEEK_SET);
    
    }

    bool ff = 0;
    
    while(1) {

        unsigned char c;

        if (fread(&c, 1, 1, raw) == 0)
            break;
        
        if (c == 0xD8 && ff)
            while(get_image());

        ff = (c == 0xFF);

    }

    printf("Total: %lu bytes.\n", ftell(raw));

    free(image_buffer);
    return 0;

}
