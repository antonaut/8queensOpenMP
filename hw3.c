/**
 * hw3.c - a.k.a. the 8-queens problem in parallel.
 * @author: Anton Erholt - <aerholt@kth.se>
 *
 * Implemented using OpenMP.
 *
 * A board is represented with the solution struct,
 * which contains an array of ints for each queen position on a row;
 *
 * NOTE: The terminology is a bit unintuitive, a board with placed queens on it
 * is called a 'solution' and the program determines whether the solution is
 * correct or not.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <omp.h>


#define NSOLUTIONS 92


#define QUEEN_SYMBOL 'X'
#define BOARD_SYMBOL '.'

#undef PRINT_SOLUTIONS

/*
  Data structs
 */

// Positions of queens on rows
// 0 indexed

struct solution
{
    unsigned char r[8];
};


/*
  Variables
 */

int nsolutions,
ncorrect,
num_threads;

// t1 and t2 are test solutions used when developing is_correct(...).
struct solution correct_solutions[NSOLUTIONS],
        t1 = {3, 6, 2, 7, 1, 4, 0, 5}, // correct
t2 = {3, 6, 2, 7, 1, 4, 0, 6}; // incorrect

void init_solution(struct solution * sp)
{
    sp->r[0] = 0; // Bottom of board (doesn't really matter)
    sp->r[1] = 0;
    sp->r[2] = 0;
    sp->r[3] = 0;
    sp->r[4] = 0;
    sp->r[5] = 0;
    sp->r[6] = 0;
    sp->r[7] = 0; // Top of board (doesn't really matter)
    return;
}

void copy_solution(struct solution * from, struct solution * to)
{
    to->r[0] = from->r[0];
    to->r[1] = from->r[1];
    to->r[2] = from->r[2];
    to->r[3] = from->r[3];
    to->r[4] = from->r[4];
    to->r[5] = from->r[5];
    to->r[6] = from->r[6];
    to->r[7] = from->r[7];
    return;
}

/**
 * Used to print a solution.
 * @param sol pointer to the solution to be printed.
 */
void print_solution(struct solution * sol)
{
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < sol->r[i]; j++)
            putchar(BOARD_SYMBOL);
        putchar(QUEEN_SYMBOL);
        for (j = sol->r[i] + 1; j < 8; j++)
            putchar(BOARD_SYMBOL);
        putchar('\n');
    }

}


/**
 * Determines if a solution is correct or not.
 * @param sol The solution to be checked.
 * @return true iff the solution is correct,
 *         false otherwise
 */
bool is_correct(struct solution * sol)
{
    int q, i, j, idiag, q2, offset;

    // For every queen
    for (i = 0; i < 8; i++)
    {
        q = sol->r[i]; // q = col, i = row

        // Check diagonals

        // Upwards
        idiag = i - 1;
        offset = 1;
        while (idiag > 0)
        {
            q2 = sol->r[idiag];
            if (q + offset == q2 || q - offset == q2)
            {

#ifdef DEBUG
                printf("updiags false!\n");
#endif
                return false;
            }
            offset++;
            idiag--;
        }

        // Downwards
        idiag = i + 1;
        offset = 1;
        while (idiag < 8)
        {
            q2 = sol->r[idiag];
            if (q + offset == q2 || q - offset == q2)
            {
#ifdef DEBUG
                printf("downdiags false!\n");
#endif
                return false;
            }
            offset++;
            idiag++;
        }

        // Check cols
        for (j = 0; j < 8; j++)
        {
            q2 = sol->r[j];
            if (j == i)
            {
                continue;
            }
            if (q2 == q)
            {
#ifdef DEBUG
                printf("cols false!\n");
#endif
                return false;
            }
        }

    }

    return true;
}


// Some simple testing
void tests(void)
{
    if (is_correct(&t1))
        printf("t1 correct!\n");
    if (is_correct(&t2))
        printf("t2 correct!");
    return;
}

void solve()
{
    struct solution tmp;
    init_solution(&tmp);

    // 8^8 = 16777216 // Max possible combinations of solutions

    // Execute this region in parallel without tasks
#pragma omp parallel for private(tmp, nsolutions)
    for (nsolutions = 0; nsolutions < 16777216 + 1; nsolutions++)
    {
        // Update board by using octal notation :-D
        int p = nsolutions;
        tmp.r[0] = p % 010;
        tmp.r[1] = (p / 010) % 010;
        tmp.r[2] = (p / 0100) % 010;
        tmp.r[3] = (p / 01000) % 010;
        tmp.r[4] = (p / 010000) % 010;
        tmp.r[5] = (p / 0100000) % 010;
        tmp.r[6] = (p / 01000000) % 010;
        tmp.r[7] = (p / 010000000) % 010;

        if (is_correct(&tmp))
        {

            // This might go faster if we use #pragma omp atomic
#pragma omp critical
            {
                copy_solution(&tmp, &correct_solutions[ncorrect]);
                ncorrect++;
            }
        }
    }
}

int main( void )
{
    int nsolved, iter;

    double total_time[5], start_time, end_time;

    /* Set number of threads */
    for (num_threads = 1; num_threads < 5; num_threads++)
    {
        omp_set_num_threads(num_threads);
        printf("*** num_threads : %d ***\n", num_threads);

        /* Iterate 5 times to get several times */
        for (iter = 0; iter < 5; iter++)
        {
            printf("Iteration %d\n", iter);

            total_time[iter] = 0;

            // Find all 92 solutions 4 times to make the program take about
            // 5 seconds to solve sequentially
            nsolved = 0;

            while (nsolved < 4)
            {
                ncorrect = 0; // Reset this global
                start_time = omp_get_wtime();
                solve();
                end_time = omp_get_wtime();

                total_time[iter] += end_time - start_time;
                nsolved++;
            }
            if (iter == 4)
                printf("Total time(s): %f, %f, %f, %f, %f\n", total_time[0], total_time[1], total_time[2], total_time[3], total_time[4]);
        }
    }

    return EXIT_SUCCESS;
}
