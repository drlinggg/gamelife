#include <stdio.h>
#include <stdlib.h>
#ifndef BMP_READER_C
#define BMP_READER_C

#pragma pack(1)
typedef struct BMPHeader {
    unsigned char ID[2];
    unsigned int file_size;
    unsigned char unused[4];
    unsigned int pixel_offset;
} BMPHeader;

typedef struct DIBHeader {
    unsigned int header_size;
    unsigned int widht;
    unsigned int height;
    unsigned short color_planes;
    unsigned short bits_per_pixel;
    unsigned int comp;
    unsigned int data_size;
    unsigned int pwidht;
    unsigned int pheight;
    unsigned int colors_count;
    unsigned int imp_colors_count;
} DIBHeader;

typedef struct BMPFile {
    BMPHeader bhdr;
    DIBHeader dhdr;
    unsigned char* data;
} BMPFile;
#pragma pop

BMPFile* load(char* fname) {
    FILE* fp = fopen(fname, "r");
    if (!fp) {
        printf("Load failed");
        exit(0);
    }
    BMPFile* bmp_file = (BMPFile*)malloc(sizeof(BMPFile));
    fread(&bmp_file->bhdr, sizeof(BMPHeader),1,fp);
    fread(&bmp_file->dhdr, sizeof(DIBHeader),1,fp);
    bmp_file->data = (unsigned char*)malloc(bmp_file->dhdr.data_size);
    fseek(fp,bmp_file->bhdr.pixel_offset, SEEK_SET);
    fread(bmp_file->data,bmp_file->dhdr.data_size,1,fp);
    fclose(fp);
    return bmp_file;
}

void freeBMPfile(BMPFile* bmp_file) {
    if (bmp_file) {
        free(bmp_file);
    }
}

#endif