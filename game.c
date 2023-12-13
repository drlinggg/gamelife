#include <stdio.h>
#include <math.h>
#include <string.h>
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
    printf("\n");
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
    int count = 0;
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
                count++;
            }
            if (matrix[i][j] == ' ' && count_alive_near == 3) {
                new_matrix[i][j] = '@';
                count++;
            }
        }
    }
    if (count == 0) {
        printf("game over");
        exit(0);
    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 128; j++) {
            matrix[i][j] = new_matrix[i][j];
        }
    }
}
// game --input birds.bmp --output C:\Users\User\Desktop\ucheba\lab_op\14lab\gamegenerations1\ --max_iter 400 --pump_freq 1
// C:\Users\User\Desktop\ucheba\lab_op\14lab\gamegenerations1\

int main(int argc, char *argv[]) {
    int frequency = 1;
    int count_max = -1;
    char* name[100];
    BMPFile *bmpf;
    char field[128][128];
    char outputlink[1024];
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--input") == 0) {
            bmpf = load(argv[i+1]);
            //printBMPHeaders(bmpf);
            printf("\n");
            for (int i = 0; i < 128; i++) {
                for (int j = 0; j < 128; j++) {
                    field[i][j] = ' ';
                }
            }
            makeMatrix(bmpf, field);
        }
        if (strcmp(argv[i], "--output") == 0) {
            strcpy(outputlink,argv[i+1]);
            //printf(outputlink);
            //todo
        }
        if (strcmp(argv[i], "--max_iter") == 0) {
            count_max = strtol(argv[i+1], NULL, 10);
        }
        if (strcmp(argv[i], "--dump_freq") == 0) {
            frequency = strtol(argv[i+1], NULL, 10);
        }
    }
    if (count_max == -1) {
        count_max = 15;
    }

    for (int i = 0; i < count_max; i++) {
        updateField(field,bmpf);
        if (i % frequency == 0) {
            save(i+1,bmpf,field, outputlink);
        }
    }
    printf("game is ended");
    freeBMPfile(bmpf);
    return 0;
}
