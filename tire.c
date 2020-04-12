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

#define BUFFER_SIZE (1 << 25) //~33 MB

FILE* raw;
unsigned char* image_buffer;
unsigned long long buffer_start = 0;
unsigned long long current_position = 0;

void set_position(unsigned long long pos) {

    // TODO - accept long long
    if (fseek(raw, pos, SEEK_SET) == 0)
        current_position = pos;
    else
        printf("Coudn't set position to %llu\n", pos);

}

bool read_char(unsigned char* c) {

    int n = fread(c, 1, 1, raw);
    current_position += n;
    return n == 1;

}

void save_buffer() {

    char name[32];
    sprintf(name, "%llu", buffer_start);
    strcat(name, ".jpg");

    FILE *fp = fopen(name, "wb");
    if (!fp) {

        printf("Can't save file.\n");
        return;

    }

    fwrite(image_buffer, 1, current_position - buffer_start, fp);
    
    fclose(fp);

}

bool get_image() {

    unsigned char c;

    if (!read_char(&c))
        return 0;

    if (c != 0xFF)
        return 0;

    buffer_start = current_position - 3;
    printf("Image at %f GB ---- ", (double)buffer_start / (1 << 30));

    bool ff = 1;
    
    while (1) {

        if (!read_char(&c)) {

            printf("EOF\n");
            return 0;

        }

        if ((current_position - buffer_start) >= BUFFER_SIZE) {
            // Buffer has finnished

            printf("too big\n");
            return 0;

        }

        image_buffer[current_position - buffer_start - 1] = c;
        
        if (ff && c == 0xD9) {

            // Image must be at least 1 kB
            if (current_position - buffer_start > 1 << 10) {

                printf("%llu.jpg\n", buffer_start);
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
            base = strtol(argv[3], NULL, 10);

        set_position( strtoll(argv[2], NULL, base) );
    
    }

    bool ff = 0;
    
    while(1) {

        unsigned char c;

        if (!read_char(&c))
            break;
        
        if (c == 0xD8 && ff)
            while(get_image());

        ff = (c == 0xFF);

    }

    printf("Total: %llu bytes.\n", current_position);

    free(image_buffer);
    return 0;

}
