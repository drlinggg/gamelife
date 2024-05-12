#include <stdio.h>
#include <string.h>
#include "bmp_reader.c"
#include "gif.h"


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
            int saved = bmpf->data[i * 64 + j];
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
                else {
                }
            }
        }
    }
}

void updateField(char matrix[512][512], unsigned int fieldforgif[512*512]) {
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
            if (i > 0 && j < 511 && matrix[i-1][j+1] == '@') {
                count_alive_near++;
            }
            if (j < 511 && matrix[i][j+1] == '@') {
                count_alive_near++;
            }
            if (i < 511 && j < 511 && matrix[i+1][j+1] == '@') {
                count_alive_near++;
            }
            if (i < 511 && matrix[i+1][j] == '@') {
                count_alive_near++;
            }
            if (i < 511 && j > 0 && matrix[i+1][j-1] == '@') {
                count_alive_near++;
            }
            if (j > 0 && matrix[i][j-1] == '@') {
                count_alive_near++;
            }


            if (matrix[i][j] == '@' && (count_alive_near < 2 || count_alive_near > 3)) {
                new_matrix[i][j] = ' ';
                fieldforgif[(511-i)*512+j] = 0;
                count++;
            }
            else if (matrix[i][j] == ' ' && count_alive_near == 3) {
                new_matrix[i][j] = '@';
                fieldforgif[(511-i)*512+j] = 255;
                count++;
            }
        }
    }
    if (count == 0) {
        printf("%s","Game generation is over in stable position");
        exit(0);
    }
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            matrix[i][j] = new_matrix[i][j];
        }
    }
}

int main(int argc, char *argv[]) {
    unsigned int frequency = 1;
    unsigned int count_max = 0;
    unsigned int delay = 5;
    bool make_gif = 0;
    bool bmp = 0;
    char* name;
    GifWriter g;
    BMPFile *bmpf;
    char field[512][512];
    unsigned int fieldforgif[512*512];
    char outputlink[1024];
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            printf("Game Life: \n");
            printf(" --input - start pos monochrome bmp 512x512\n"
                       " --max-iter - how many generations will be created\n"
                       " --pump-freq - frequence beetwen saving generations (1 default)\n"
                       " --gif pastename.gif\n"
                       " --delay (gif delay attribute)"
                       " --bmp - save generations in dir save as bmps (but no gif)\n"
                       " --output - saving library path (for --bmp)  \n"
                       " made by https://github.com/drlinggg\n"
                );
            exit(0);
        }
        if (strcmp(argv[i], "--input") == 0) {
            bmpf = load(argv[i + 1]);
            printBMPHeaders(bmpf);
            printf("\n");
            for (int i = 0; i < 512; i++) {
                for (int j = 0; j < 512; j++) {
                    field[i][j] = ' ';
                }
            }
            makeMatrix(bmpf, field);
        }
        if (strcmp(argv[i], "--output") == 0) {
            strcpy(outputlink,argv[i+1]);
        }
        if (strcmp(argv[i], "--max-iter") == 0) {
            count_max = strtol(argv[i+1], NULL, 10);
        }
        if (strcmp(argv[i], "--dump_freq") == 0) {
            frequency = strtol(argv[i+1], NULL, 10);
        }
        if (strcmp(argv[i], "--gif") == 0) {
            make_gif = 1;
            name = argv[i+1];
        }
        if (strcmp(argv[i], "--delay") == 0) {
            delay = strtol(argv[i+1], NULL, 10);
        }
        if (strcmp(argv[i], "--bmp") == 0) {
            bmp = 1;
        }
    }
    int count = 0;
    if (count_max != -1 && bmp) {
        printf("Trying to generate & save bmps...\n");
        for (int i = 0; i < count_max; i++) {
            if (count == count_max/10) {
                count = 0;
                printf("%s", "*");
            }
            count++;
            updateField(field, fieldforgif);
            if (i % frequency == 0) {
                save(i + 1, bmpf, field, outputlink);
            }
        }
    }
    else if (count_max != -1 && make_gif) {
        printf("Trying to generate & save gif...\n");
        GifBegin(&g,name,512,512, delay,8,8);
        for (int i = 0; i < 512; i++) {
            for (int j = 0; j < 512; j++) {
                if (field[i][j] == ' ') {
                    fieldforgif[(511-i)*512+j] = 0;
                }
                else {
                    fieldforgif[(511-i)*512+j] = 255;
                }
            }
        }
        GifWriteFrame(&g, (const uint8_t *) fieldforgif, 512, 512, delay, 8, 8);
        for (int i = 0; i < count_max; i++) {
            if (count == count_max/10) {
                count = 0;
                printf("%s", "*");
            }
            count++;
            updateField(field, fieldforgif);
            if (i % frequency == 0) {
                GifWriteFrame(&g, (const uint8_t *) fieldforgif, 512, 512, delay, 8, 8);
            }
        }
        GifEnd(&g);
    }
    freeBMPfile(bmpf);
    printf("\nGame generation is over\n");
    return 0;
}
