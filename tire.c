/*
 * Tire - Tiny Image Recoverer
 * Copyright (c) 2020 Danilo Lemos
 * License: https://github.com/danilolc/Tire/blob/master/LICENSE
 */

// Usage:
// ./Tire [start] [base] < FILE

// start - position to start looking for jpg images
// base - the positional base that start is written - default is base 10
// FILE - the file containing the raw data - use /dev/sdx on linux to search on disk

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bool _Bool

const long BUFFER_SIZE = 1 << 25; //~33 MB
char* image_buffer;
unsigned long buffer_start = 0;

unsigned long counter = 0;

void save_buffer() {

    char name[32];
    sprintf(name, "%lu", buffer_start);
    strcat(name, ".jpg");

    FILE *fp = fopen(name, "w+");
    if (!fp) {

        printf("Can't save file.");
        return;

    }

    fwrite(image_buffer, 1, counter - buffer_start, fp);
    
    fclose(fp);

}

bool get_image() {

    int c = fgetc(stdin);
    counter++;
    if (c != 0xFF)
        return 0;

    buffer_start = counter - 3;
    printf("Image at %f GB ---- %lu.jpg\n", (double)buffer_start / (1 << 30), buffer_start);

    bool ff = 1;
    
    while (1) {

        int c = fgetc(stdin);
        image_buffer[counter - buffer_start] = c;
        counter++;
        
        if ((counter - buffer_start) >= BUFFER_SIZE || c == EOF) {
            // Buffer or stdin has finnished

            return 0;

        }

        
        if (ff && c == 0xD9) {

            // Image must be at least 1 kB
            if (counter - buffer_start > 1 << 10)
                save_buffer();
            
            return 0;

        } else if (ff && c == 0xD8) {
            // Found a image starter, try again

            return 1;

        } else if (ff && c != 0x00 && c < 0xC0) {
            // Invalid marker

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
    
    if (argc > 1) {
        
        int base = 10;
        if (argc > 2)
            base = strtol(argv[2], NULL, 10);

        counter = strtol(argv[1], NULL, base);
        fseek(stdin, counter, SEEK_SET);
    
    }

    bool ff = 0;
    
    while(1) {

        int c = fgetc(stdin);
        counter++;    
        
        if (c == EOF) break;
        if (c == 0xD8 && ff)
            while(get_image());

        ff = (c == 0xFF);

    }

    printf("Total: %lu bytes.\n", counter);

    free(image_buffer);
    return 0;

}
