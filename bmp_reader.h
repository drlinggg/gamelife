/*
#ifndef BMP_READER_H
#define BMP_READER_H

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
    unsigned int BI_RGB;
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

BMPFile* load(char* fname);
void freeBMPfile(BMPFile* bmp_file);

#endif //BMP_READER_H
 */