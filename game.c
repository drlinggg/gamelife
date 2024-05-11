#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include "bmp_reader.c"


void printBMPHeaders(BMPFile* bmpf) {
    printf("Headers data: \n");
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

void printField(char field[512][512]) {
    printf("\n");
    for (int i = 511; i >= 0; i--) {
        for (int j = 0; j < 512; j++) {
            printf("%c", field[i][j]);
        }
        printf("\n");
    }
}

void makeMatrix(BMPFile* bmpf, char matrix[512][512]) {
    for (int i = 0; i < bmpf->dhdr.height; i++) {
        for (int j = 0; j < bmpf->dhdr.widht/8; j++) {
            int saved = bmpf->data[i * 32 + j];
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

void updateField(char matrix[512][512], BMPFile* bmpf) {
    char new_matrix[512][512];
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            new_matrix[i][j] = matrix[i][j];
        }
    }
    int count = 0;
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            //matrix[i][j]
            int count_alive_near = 0;
            if (i > 0 && j > 0 && matrix[i-1][j-1] == '@') {
                count_alive_near++;
            }
            if (i > 0 && matrix[i-1][j] == '@') {
                count_alive_near++;
            }
            if (i > 0 && j < 512 && matrix[i-1][j+1] == '@') {
                count_alive_near++;
            }
            if (j < 512 && matrix[i][j+1] == '@') {
                count_alive_near++;
            }
            if (i < 512 && j < 512 && matrix[i+1][j+1] == '@') {
                count_alive_near++;
            }
            if (i < 512 && matrix[i+1][j] == '@') {
                count_alive_near++;
            }
            if (i < 512 && j > 0 && matrix[i+1][j-1] == '@') {
                count_alive_near++;
            }
            if (j > 0 && matrix[i][j-1] == '@') {
                count_alive_near++;
            }
            if (matrix[i][j] == '@' && (count_alive_near < 2 || count_alive_near > 3)) {
                new_matrix[i][j] = ' ';
                count++;
            }
            else if (matrix[i][j] == ' ' && count_alive_near == 3) {
                new_matrix[i][j] = '@';
                count++;
            }
        }
    }
    if (count == 0) {
        printf("Game generation is over in stable position");
    }
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            matrix[i][j] = new_matrix[i][j];
        }
    }
}

int main() {
    int frequency = 1;
    int count_max = -1;
    char* name[100];
    BMPFile *bmpf;
    char field[512][512];
    char outputlink[512];
    while (count_max != -1) {
        char* command[100];
        scanf("%s", command);
            if (strcmp(command, "--help") == 0) {
                printf("Game Life: \n");
                printf(" --input - start pos monochrome bmp 512x512\n"
                       " --output - saving library path with \n"
                       " --max-iter - how many generations will be created\n"
                       " --pump-freq - frequence beetwen saving generations (1 default)\n"
                       " --draw - make ascii picture\n"
                       " --print-data - print header data\n"
                       " made by https://github.com/drlinggg\n"
                );
                exit(0);
            }
            if (strcmp(command, "--input") == 0) {
                scanf("%s", command);
                bmpf = load(command);
                printBMPHeaders(bmpf);
                printf("\n");
                for (int i = 0; i < 512; i++) {
                    for (int j = 0; j < 512; j++) {
                        field[i][j] = ' ';
                    }
                }
                makeMatrix(bmpf, field);
            }
            if (strcmp(command, "--output") == 0) {
                scanf("%s", command);
                strcpy(outputlink, command);
            }
            if (strcmp(command, "--max-iter") == 0) {
                scanf("%s", command);
                count_max = strtol(command, NULL, 10);
            }
            if (strcmp(command, "--dump-freq") == 0) {
                scanf("%s", command);
                frequency = strtol(command, NULL, 10);
                break;
            }
        }
        if (count_max != -1) {
            printf("Trying to generate & save...\n");
            for (int i = 0; i < count_max; i++) {
                updateField(field, bmpf);
                if (i % frequency == 0) {
                    save(i + 1, bmpf, field, outputlink);
                }
            }
            printf("Game generation is over\n");
        }
    freeBMPfile(bmpf);
    return 0;
}
