#include <stdio.h>
#include <math.h>
#include <string.h>
#include "bmp_reader.c"
#include <unistd.h>


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

void printField(char field[128][128]) {
    printf("\n");
    for (int i = 127; i >= 0; i--) {
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
            else if (matrix[i][j] == ' ' && count_alive_near == 3) {
                new_matrix[i][j] = '@';
                count++;
            }
        }
    }
    if (count == 0) {
        printf("Game generation is over in stable position");
        exit(0);
    }
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
    int make_gif = 0;
    char* name[100];
    BMPFile *bmpf;
    char field[128][128];
    char outputlink[1024];
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            printf("Game Life: \n");
            printf(" --input - start pos monochrome bmp 128x128\n"
                   " --output - saving library path with \n"
                   " --max-iter - how many generations will be created\n"
                   " --pump-freq - frequence beetwen saving generations (1 default)\n"
                   " --draw - make ascii picture\n"
                   " --make-gif - saves gif of the game (doesnt work yet\n"
                   " --print-data - print header data\n"
                   " uses: python gifmaker (gonna fix soon)\n"
                   " made by https://github.com/drlinggg\n"
                   );
            exit(0);
        }
        if (strcmp(argv[i], "--input") == 0) {
            bmpf = load(argv[i+1]);
            printBMPHeaders(bmpf);
            printf("\n");
            for (int i = 0; i < 128; i++) {
                for (int j = 0; j < 128; j++) {
                    field[i][j] = ' ';
                }
            }
            makeMatrix(bmpf, field);
        }
        if (strcmp(argv[i], "--make-gif") == 0) {
            make_gif = 1;
        }
        if (strcmp(argv[i], "--draw") == 0) {
            printField(field);
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
    if (make_gif) {
        //todo fix gif saving
        printf("Trying to save gif...\n");
        int count_len = 0;
        char temp[1000];
        for (int i = 0; i < count_max; i++) {
            count_len += sprintf(temp, "%d", i);
        }
        char makegifcommand[15] = "gifmaker -i ";
        for (int i = 0; i < count_max; i++) {
            char temp[10];
            snprintf(temp, sizeof(temp), "%d", i + 1);
            strcat(temp, ".bmp ");
            strcat(makegifcommand, temp);
        }
        int result = chdir(outputlink);
        if (result == 0) {
            printf("Current directory changed successfully\n");
        } else {
            printf("Error: directory changing\n");
            exit(1);
        }
        system(makegifcommand);
    }
    freeBMPfile(bmpf);
    return 0;
}
