#include <stdio.h>
#include <math.h>
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

void printField(char field[128][128]) {
    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 128; j++) {
            printf("%c", field[i][j]);
        }
        printf("\n");
    }
}

void makeMatrix(BMPFile* bmpf, char matrix[128][128]) {
    for (int i = 0; i < bmpf->dhdr.height; i++) {
        for (int j = 0; j < bmpf->dhdr.widht/8; j++) {
            int saved = bmpf->data[i * 16 + j];
            //0 - 8 бит черных
            //255 - 8 бит белых
            // 127 = 255-128 - 1 черный и 7 белых
            char bin[8] = {0};
            int a = 7;
            while (saved != 0) {
                bin[a] = saved % 2;
                saved /= 2;
                a--;
            }
            for (int u = 0; u < 8; u++) {
                if (bin[u] == 0) {
                    if (j == 0) {
                        matrix[i][j + u] = '@';
                    } else {
                        matrix[i][j * 8 - 8 + u] = '@';
                    }
                }
            }

        }
    }
}

void updateField(char matrix[128][128], BMPFile* bmpf) {
    char new_matrix[128][128];
    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 128; j++) {
            new_matrix[i][j] = matrix[i][j];
        }
    }
    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 128; j++) {
            //matrix[i][j]
            int count_alive_near = 0;
            if (i > 0 && j > 0 && matrix[i-1][j-1] == '@') {
                count_alive_near++;
            }
            if (i > 0 && matrix[i-1][j] == '@') {
                count_alive_near++;
            }
            if (i > 0 && j < 128 && matrix[i-1][j+1] == '@') {
                count_alive_near++;
            }
            if (j < 128 && matrix[i][j+1] == '@') {
                count_alive_near++;
            }
            if (i < 128 && j < 128 && matrix[i+1][j+1] == '@') {
                count_alive_near++;
            }
            if (i < 128 && matrix[i+1][j] == '@') {
                count_alive_near++;
            }
            if (i < 128 && j > 0 && matrix[i+1][j-1] == '@') {
                count_alive_near++;
            }
            if (j > 0 && matrix[i][j-1] == '@') {
                count_alive_near++;
            }
            if (matrix[i][j] == '@' && (count_alive_near < 2 || count_alive_near > 3)) {
                new_matrix[i][j] = ' ';
            }
            if (matrix[i][j] == ' ' && count_alive_near == 3) {
                new_matrix[i][j] = '@';
            }
        }
    }
    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 128; j++) {
            matrix[i][j] = new_matrix[i][j];
        }
    }
}

int main() {
    char field[128][128];
    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 128; j++) {
            field[i][j] = ' ';
        }
    }
    BMPFile* bmpf = load("field.bmp");
    printBMPHeaders(bmpf);
    printf("\n");
    makeMatrix(bmpf, field);
    printField(field);
    updateField(field, bmpf);
    printField(field);
    /*
    bmpf = save(2,bmpf,field);
     */
    updateField(field, bmpf);
    printField(field);
    freeBMPfile(bmpf);
    return 0;
}