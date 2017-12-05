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
#define YDIM 85

#define DEAD  0
#define ALIVE 1

#define TRUE  1
#define FALSE 0

#define HISTORY_SIZE 5
#define DIFFERENCE_HISTORY_SIZE 52
#define GRAPH_LINES 11

/*
 * --------------------------------------------------------
 * prototypes
 * --------------------------------------------------------
 */
void get_user_input(int * cells, const char* path, int max);
int count_alive_cells(const int *cells, int x, int y);
void display_cells(const int *cells);
void display_graph(const int *, int, int, int, char[64]);
void evolution_step(int *cells);
int count_all_alive_cells(const int *cells);
void initialize_cells(int *cells);
void copy(const int *source, int *target, int offset, int size);
int equals(const int *state_a, const int *state_b);

/*
 * --------------------------------------------------------
 * start of the main-function
 * --------------------------------------------------------
 */
int main(int argc, char * argv[]) {
    setlocale(LC_ALL, "");

	int history[HISTORY_SIZE][XDIM][YDIM] = {};
	int *cells = malloc(XDIM * YDIM * sizeof(int));
	int difference_history[DIFFERENCE_HISTORY_SIZE];
	int generation = 1;
    int oscillating_after = 0;
    int oscillating_steps = 0;
    int oscillating = FALSE;

    if(cells == NULL)
        return EXIT_FAILURE;

	for (int x = 0; x < DIFFERENCE_HISTORY_SIZE; x++)
		*(difference_history + x) = GRAPH_LINES + 1;

    if (argc > 1)
	get_user_input((int *) cells, argv[1], XDIM*YDIM);
    else
	initialize_cells((int *) &cells);

    while(TRUE) {
        display_cells((int *) &cells);


        printf("Anzahl lebender Zellen: %4d\n", count_all_alive_cells((int *) &cells));
        printf("Generation: %5d\n", generation);

        // generating generation statistics
	    if (generation > 1) {
		    int last_generation[XDIM][YDIM] = {};

		    copy((int *) &(history) + (HISTORY_SIZE - 1) * XDIM * YDIM, (int *) &last_generation, 0, XDIM * YDIM);

		    int difference = count_all_alive_cells((int *) &last_generation) - count_all_alive_cells((int *) &cells);

		    if (generation < DIFFERENCE_HISTORY_SIZE)
			    *((int *) &difference_history + generation - 1) = difference;
		    else {
			    for (int x = 1; x < DIFFERENCE_HISTORY_SIZE; x++)
				    copy((int *) &difference_history + x, (int *) &difference_history, x - 1, 1);

			    *((int *) &difference_history + DIFFERENCE_HISTORY_SIZE - 1) = difference;
		    }


		    printf("Unterschied zur letzten Generation: %5d\n", difference);


            if (!oscillating) {
                for (int x = 0; x < HISTORY_SIZE - 1; x++) {
                    copy((int *) &(history) + (HISTORY_SIZE - 1 - x) * XDIM * YDIM, (int *) &last_generation, 0,
                         XDIM * YDIM);

                    if (equals((int *) &last_generation, (int *) &cells)) {
                        oscillating = TRUE;
                        oscillating_steps = x;
                        oscillating_after = generation;
                        break;
                    }
                }
            }

            // display oscilliating data
            if (oscillating) {
                char *steps = malloc(5 * sizeof(char));
                sprintf(steps, "mit %d", oscillating_steps);

                printf("Zustand osziliert %s Schritt%s nach %d Generationen\n",
                       oscillating_steps == 1 ?  "mit einem" : oscillating_steps == 0 ? "ohne" : steps,
                       oscillating_steps > 1 ? "en" : "", oscillating_after);
            } else
	            printf("\n");
	    }

	    display_graph(difference_history, DIFFERENCE_HISTORY_SIZE, GRAPH_LINES, -5, "Entwicklung der absoluten Anzahl der lebenden Zellen");


        if (count_all_alive_cells((int *) &cells) == 0)
            break;

        // set application asleep to see the progress
        usleep(100000);

        for (int x = 1; x < HISTORY_SIZE; x++)
            copy((int *) &history + x * XDIM * YDIM, (int *) &history, (x -1) * XDIM * YDIM, XDIM * YDIM);

        copy((int *) &cells, (int *) &history, (HISTORY_SIZE -1) * XDIM * YDIM, XDIM * YDIM);

        evolution_step((int *) &cells);
        generation++;
    }

    free(cells);
}

/*
 * --------------------------------------------------------
 * gets a set of 0s and 1s from a .txt-file
 * --------------------------------------------------------
 */
void get_user_input(int * cells, const char* path, int max)
{
	FILE * f;
	int c, t = 0;
	f = fopen(path, "r");
	while (((c = fgetc(f)) != EOF) && t < max)
	{
		if (c == 48)
		{
			*(cells + t) = 0;
			t++;
		}
		if (c == 49)
		{
			*(cells + t) = 1;
			t++;
		}
	}
	fclose(f);
}

/*
 * --------------------------------------------------------
 * copies a whole state to a given target
 * --------------------------------------------------------
 */
void copy(const int *source, int *target, int offset, int size) {
	for (int x = 0; x < size; x++)
		*(target + offset + x) = *(source + x);
}

/*
 * --------------------------------------------------------
 * compares two states / successive states
 * --------------------------------------------------------
 */
int equals(const int *state_a, const int *state_b) {
    for (int x = 0; x < XDIM * YDIM; x++)
        if (*(state_a + x) != *(state_b + x))
            return FALSE;
    return TRUE;
}

/*
 * --------------------------------------------------------
 * creates the cells randomly
 * --------------------------------------------------------
 */
void initialize_cells(int *cells) {
    srand(time(0));

    for (int a = 0; a < XDIM; a++)
        for (int b = 0; b < YDIM; b++)
            *(cells + a * YDIM + b) = rand() % 2;
}

/*
 * --------------------------------------------------------
 * displays the cells on the screen
 * --------------------------------------------------------
 */
void display_cells(const int *cells) {
	system("clear");

    for (int x = 0; x < XDIM; x++) {
        for (int y = 0; y < YDIM; y++) {
            if (*(cells + x * YDIM + y)== ALIVE) {
                printf("%lc", 0x1F47B);
            } else {
                printf(" .");
            }
        }
        printf("\n");
    }
    printf("\n");
}

/*
 * --------------------------------------------------------
 * displays a graph revealing the alive cells difference
 * between the current step and the previous one
 * --------------------------------------------------------
 */
void display_graph(const int * p, int dim, int lines, int bottom_border, char caption[64]) {
	char line[dim];

    // print caption
	printf("\n::: %s :::\n", caption);
	for(int d = 0; d < dim; d++)
		for (int g = 0; g < 3; g++) 
			printf("-");
	printf("--------\n");

    // print the graph based on data
	for (int a = lines - 1; a >= 0; a--)
	{
		printf("%4d :: ", bottom_border+a);
		for (int b = 0; b < dim; b++)
		{
			if (*(p+b) == bottom_border + a)
				*(line+b) = 'x';
			else if (bottom_border + a == 0)
				*(line + b) = '-';
			else
				*(line+b) = ' ';
		}
		for(int d = 0; d < dim; d++)
			for (int g = 0; g < 3; g++) 
				printf("%c", *(line+d));
		printf("\n");
	}
	for(int d = 0; d < dim; d++)
		for (int g = 0; g < 3; g++) 
			printf("-");
	printf("--------\n");
}

/*
 * --------------------------------------------------------
 * returns the number of living cells around a given one
 * --------------------------------------------------------
 */
int count_alive_cells(const int *cells, int x, int y) {
    int count = 0, tx = 0, ty = 0;
    for (int a = -1; a <= 1; a++)
        for (int b = -1; b <= 1; b++) {
            tx = x+a < 0 ? XDIM-1 : (x+a >= XDIM ? 0 : x+a);
            ty = y+b < 0 ? YDIM-1 : (y+b >= YDIM ? 0 : y+b);
            if (!(a == 0 && b == 0))
	            count += *(cells + tx * YDIM + ty);
        }
    return count;
}

/*
 * --------------------------------------------------------
 * goes one evolution step forward based on the set rules
 * --------------------------------------------------------
 */
void evolution_step(int *cells) {
    int t_cells[XDIM][YDIM];
    int * c;
    int * tc;
    for (int a = 0; a < XDIM; a++)
        for (int b = 0; b < YDIM; b++) {
            c = cells + a * YDIM + b;
            tc = &t_cells[a][b];

            // apply game rules
            switch(count_alive_cells(cells, a, b)) {
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
            *(cells + a * YDIM + b) = t_cells[a][b];
}

/*
 * --------------------------------------------------------
 * counts all alive cells existing in the field
 * --------------------------------------------------------
 */
int count_all_alive_cells(const int *cells) {
    int count_alive = 0;
    for (int x = 0; x < XDIM; x++)
        for (int y = 0; y < YDIM; y++)
            if (*(cells + x * YDIM + y) == ALIVE)
                count_alive++;

    return count_alive;
}
