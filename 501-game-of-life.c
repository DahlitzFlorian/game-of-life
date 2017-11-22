/* Game of life
The "game of life" is an old and very simple approach of simulating evolution 
A 2-dimensional field of cells is regarded with a cell being either empty/dead (0) or occupied/alive (1)
The intial state can be chosen e.g. manually or using some random operations.

The further evolution is governed by the following rules:

Every cell interacts with its eight neighbours, which are the cells that are directly horizontally, vertically, or diagonally adjacent. 
At each step in time, the following transitions occur:
   1. Any live cell with fewer than two live neighbours dies, as if by needs caused by underpopulation.
   2. Any live cell with more than three live neighbours dies, as if by overcrowding.
   3. Any live cell with two or three live neighbours lives, unchanged, to the next generation.
   4. Any empty/dead cell with exactly three live neighbours cells will be populated with a living cell.

The initial pattern constitutes the 'seed' of the system. The first generation is created by applying the above rules
 simultaneously to every cell in the seed � births and deaths happen simultaneously, and the discrete moment at which this
 happens is sometimes called a tick. (In other words, each generation is a pure function of the one before.)
 The rules continue to be applied repeatedly to create further generations.

see: http://en.wikipedia.org/wiki/Conway's_Game_of_Life
*/


// TO DO: Complete the programm such that it simulates the game of life.
//        Do this as teamwork (e.g. in teams with 2 or 3)
//        and benefit from the fact the functions can developed separately and then integrated into the final program.
// TO DO optional 1: extend the program, such that it detects 'stable states', i.e. the system is oscillating between a few states.
// TO DO optional 2: let the program find a start state such that the system stays alive and unstable for as long as possible
// TO DO optional 3: Create a flicker-free output: Do not print each character separately, but write the output into a string, which is printed all at once
// TO DO optional 4: extend the program such that the content of the cells can be edited by the user.

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <unistd.h>

#define XDIM 30
#define YDIM 50

#define DEAD  0
#define ALIVE 1

int count_alive_cells(int x, int y);
void display_cells();
void evolution_step();
int count_all_alive_cells();
void initialize_cells();

int cells[XDIM][YDIM];
int generation = 1;

int main() {
    setlocale(LC_ALL, "");
    initialize_cells();

    while(1) {
        display_cells();

        printf("Anzahl lebender Zellen: %4d\n", count_all_alive_cells());
        printf("Generation %5d\n", generation);

        // Leave loop if there are no more occupied cells
        if (count_all_alive_cells() == 0)
            break;

        usleep(100000);


        evolution_step();
        generation++;
    }
}

void initialize_cells() {
    srand(time(0));

    for (int a = 0; a < XDIM; a++)
        for (int b = 0; b < YDIM; b++)
            cells[a][b] = rand() % 2;
}

void display_cells() {
	system("clear");

    for (int x = 0; x < XDIM; x++) {
        for (int y = 0; y < YDIM; y++) {
            if (cells[x][y] == ALIVE) {
                printf("%lc", 0x1F47B);
            } else {
                printf(" .");
            }
        }
        printf("\n");
    }
    printf("\n");
}

int count_alive_cells(int x, int y) {
    int count = 0, tx = 0, ty = 0;
    for (int a = -1; a <= 1; a++)
        for (int b = -1; b <= 1; b++) {
            tx = x+a < 0 ? XDIM-1 : (x+a >= XDIM ? 0 : x+a);
            ty = y+b < 0 ? YDIM-1 : (y+b >= YDIM ? 0 : y+b);
            if (!(a == 0 && b == 0))
                count += cells[tx][ty];
        }
    return count;
}

void evolution_step() {
    int t_cells[XDIM][YDIM];
    int * c;
    int * tc;
    for (int a = 0; a < XDIM; a++)
        for (int b = 0; b < YDIM; b++) {
            c = &cells[a][b];
            tc = &t_cells[a][b];
            switch(count_alive_cells(a, b)) {
                case 0:
                case 1:
                    *tc = DEAD;
                    break;
                case 2:
                    *tc = *c;
                    break;
                case 3:
                    *tc = ALIVE;
                     break;
                default:
                    *tc = DEAD;
            }
        }

    for (int a = 0; a < XDIM; a++)
        for (int b = 0; b < YDIM; b++)
            cells[a][b] = t_cells[a][b];
}

int count_all_alive_cells() {
    int count_alive = 0;
    for (int x = 0; x < XDIM; x++)
        for (int y = 0; y < YDIM; y++)
            if (cells[x][y] == ALIVE)
                count_alive++;

    return count_alive;
}