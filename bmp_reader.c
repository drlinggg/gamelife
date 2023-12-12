#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

BMPFile* save(int n, BMPFile* bmpf, char matrix[128][128]) {
    FILE *fptr;
    BMPFile new_bmpf;
    new_bmpf.bhdr = bmpf->bhdr;
    new_bmpf.dhdr = bmpf->dhdr;
    new_bmpf.data = (unsigned char*)malloc(bmpf->dhdr.data_size);
    //надо перенести значение матрицы в бмпшку и сохранить ее;
    //биты слева направо снизу вверх
    int pos = 0;
    for (int i = 0; i < new_bmpf.dhdr.height; i++) {
        for (int j = 0; j < new_bmpf.dhdr.widht; j+=8) {
            int count = 0;
            if (matrix[i][j] == '@') {
                count += 128;
            }
            if (matrix[i][j+1] == '@') {
                count += 64;
            }
            if (matrix[i][j+2] == '@') {
                count += 32;
            }
            if (matrix[i][j+3] == '@') {
                count += 16;
            }
            if (matrix[i][j+4] == '@') {
                count += 8;
            }
            if (matrix[i][j+5] == '@') {
                count += 4;
            }
            if (matrix[i][j+6] == '@') {
                count += 2;
            }
            if (matrix[i][j+7] == '@') {
                count++;
            }

            //data[i] - 1 byte
            //получить число бит в сумме на 8 пикселей вперед добавить потом увеличить пос
            new_bmpf.data[pos] = 255-count;
            pos++;
        }
    }
    fptr = fopen("2.bmp", "w");
    fwrite(&new_bmpf,sizeof (new_bmpf),1,fptr);
    fclose(fptr);
    BMPFile* link = &new_bmpf;
    return link;
}

void freeBMPfile(BMPFile* bmp_file) {
    if (bmp_file) {
        free(bmp_file);
    }
}

#endif