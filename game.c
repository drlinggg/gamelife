#include <stdio.h>
#include <stdbool.h>
#include "bmp_reader.c"

void printBMPHeaders(BMPFile* bmpf) {
    printf("ID=%c%c\n"
           "file_size=%d\n"
           "pixel_offset=%d\n"
           "header_size=%d\n"
           "widht=%d\n"
           "height=%d\n"
           "unsigned short color_planes=%d\n"
           "unsigned short bits_per_pixel=%d\n"
           "comp=%d\n"
           "data_size=%d\n"
           "pwidht=%d\n"
           "pheight=%d\n"
           "colors_count=%d\n"
           "imp_colors_count=%d\n",
           bmpf->bhdr.ID[0],bmpf->bhdr.ID[1],
           bmpf->bhdr.file_size,
           bmpf->bhdr.pixel_offset,
           bmpf->dhdr.header_size,
           bmpf->dhdr.widht,
           bmpf->dhdr.height,
           bmpf->dhdr.color_planes,
           bmpf->dhdr.bits_per_pixel,
           bmpf->dhdr.comp,
           bmpf->dhdr.data_size,
           bmpf->dhdr.pwidht,
           bmpf->dhdr.pheight,
           bmpf->dhdr.colors_count,
           bmpf->dhdr.imp_colors_count);
}

void printBMPPixels(BMPFile* bmpf) {
    int data_size = bmpf->dhdr.widht*bmpf->dhdr.height*bmpf->dhdr.bits_per_pixel/8;
    for (int i = 0; i < data_size; i++) {
        if (i % 16 == 0) {
            printf("\n");
        }
        printf("%02x ", bmpf->data[i]);
    }
}

int main() {
    bool field[100][100];
    bool newfield[100][100];
    BMPFile* bmpf = load("field.bmp");
    printBMPHeaders(bmpf);
    printf("\n");
    printBMPPixels(bmpf);
    freeBMPfile(bmpf);
    return 0;
}