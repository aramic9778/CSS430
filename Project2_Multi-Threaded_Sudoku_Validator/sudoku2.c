#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// array of all threads
int thread_process_number[11];
// index counter for all square threads (thats why starts from 2)
int thread_counter = 2;
// struct for parameters
typedef struct
{    
    int row;    
    int column;
} parameters;

// all threads variables
pthread_t rows, col, one, two, three, four, five, six, seven, eight, nine;

// all return values
void * r_rows;
void * r_col;
void * r_square1;
void * r_square2;
void * r_square3;
void * r_square4;
void * r_square5;
void * r_square6;
void * r_square7;
void * r_square8;
void * r_square9;

// declaration of worker functions for rows
void* rows_valid(void* p);
// declaration of worker functions for columns
void* col_valid(void* p);
// declaration of worker functions for squares
void* square_valid(void* p);

// sudoku for checking 
int sudoku_board[9][9] = {
        {6, 2, 4, 5, 3, 9, 1, 8, 7},
        {5, 1, 9, 7, 2, 8, 6, 3, 4},
        {8, 3, 7, 6, 1, 4, 2, 9, 5},
        {1, 4, 3, 8, 6, 5, 7, 2, 9},
        {9, 5, 8, 2, 4, 7, 3, 6, 1},
        {7, 6, 2, 3, 9, 1, 4, 5, 8},
        {3, 7, 1, 9, 5, 6, 8, 4, 2},
        {4, 9, 6, 1, 8, 2, 5, 7, 3},
        {2, 8, 5, 4, 7, 3, 9, 1, 6},
    };

// main function for creating and joining all threads and output the result
int main() {
    
// ============================================================================
// declaring all start locations for each thread
// ============================================================================

// for rows
parameters *row_check = (parameters *) malloc(sizeof(parameters));
row_check->row = 0;
row_check->column = 0;

// for columns
parameters *col_check = (parameters *) malloc(sizeof(parameters));
col_check->row = 0;
col_check->column = 0;

// ============================================================================
// X * *
// * * * - first square ( each X or * is 3x3 square)
// * * *
// ============================================================================
parameters *square1_check = (parameters *) malloc(sizeof(parameters));
square1_check->row = 0;
square1_check->column = 0;

// ============================================================================
// * X *
// * * * - second square ( each X or * is 3x3 square)
// * * *
// ============================================================================
parameters *square2_check = (parameters *) malloc(sizeof(parameters));
square2_check->row = 0;
square2_check->column = 3;

// ============================================================================
// * * X
// * * * - third square ( each X or * is 3x3 square)
// * * *
// ============================================================================
parameters *square3_check = (parameters *) malloc(sizeof(parameters));
square3_check->row = 0;
square3_check->column = 6;

// ============================================================================
// * * *
// X * * - fourth square ( each X or * is 3x3 square)
// * * *
// ============================================================================
parameters *square4_check = (parameters *) malloc(sizeof(parameters));
square4_check->row = 3;
square4_check->column = 0;

// ============================================================================
// * * *
// * X * - fifth square ( each X or * is 3x3 square)
// * * *
// ============================================================================
parameters *square5_check = (parameters *) malloc(sizeof(parameters));
square5_check->row = 3;
square5_check->column = 3;

// ============================================================================
// * * *
// * * X - sixth square ( each X or * is 3x3 square)
// * * *
// ============================================================================
parameters *square6_check = (parameters *) malloc(sizeof(parameters));
square6_check->row = 3;
square6_check->column = 6;

// ============================================================================
// * * *
// * * * - seventh square ( each X or * is 3x3 square)
// X * *
// ============================================================================
parameters *square7_check = (parameters *) malloc(sizeof(parameters));
square7_check->row = 6;
square7_check->column = 0;

// ============================================================================
// * * *
// * * * - eighth square ( each X or * is 3x3 square)
// * X *
// ============================================================================
parameters *square8_check = (parameters *) malloc(sizeof(parameters));
square8_check->row = 6;
square8_check->column = 3;

// ============================================================================
// * * *
// * * * - ninth square ( each X or * is 3x3 square)
// * * X
// ============================================================================
parameters *square9_check = (parameters *) malloc(sizeof(parameters));
square9_check->row = 6;
square9_check->column = 6;

// creating all threads where pthread_create(address of thread, default attributes, worker function, parameters)
pthread_create(&rows, NULL, *rows_valid, (void *) row_check);
pthread_create(&col, NULL, *col_valid, (void *) col_check);
pthread_create(&one, NULL, *square_valid, (void *) square1_check);
pthread_create(&two, NULL, *square_valid, (void *) square2_check);
pthread_create(&three, NULL, *square_valid, (void *) square3_check);
pthread_create(&four, NULL, *square_valid, (void *) square4_check);
pthread_create(&five, NULL, *square_valid, (void *) square5_check);
pthread_create(&six, NULL, *square_valid, (void *) square6_check);
pthread_create(&seven, NULL, *square_valid, (void *) square7_check);
pthread_create(&eight, NULL, *square_valid, (void *) square8_check);
pthread_create(&nine, NULL, *square_valid, (void *) square9_check);

// join all threads where pthread_join(thread name, return value)
pthread_join(rows, &r_rows);
pthread_join(col, &r_col);
pthread_join(one, &r_square1);
pthread_join(two, &r_square2);
pthread_join(three, &r_square3);
pthread_join(four, &r_square4);
pthread_join(five, &r_square5);
pthread_join(six, &r_square6);
pthread_join(seven, &r_square7);
pthread_join(eight, &r_square8);
pthread_join(nine, &r_square9);

// checking that all 11 threads gave us right output
for (int i = 0; i < 11; i++)
{
            if(thread_process_number[i] != 1){              
                printf("Sudoku is NOT VALID. \n");
                exit(0);
            }
        }
        printf("Sudoku is VALID. \n");
    return 0;
}

// rows validation function
void* rows_valid(void* p) {
    parameters * data_rows = (parameters *) p; // taking begin parameters
    int begin_row = data_rows->row; // declaring begin values
    int begin_column = data_rows->column;
    for (int i = begin_row; i < 9; i++) 
    {
        // array for maing sure that all numbers from 1 to 9 are available 
        int validator[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
        for (int j = begin_column; j < 9; j++)
        {
         validator[sudoku_board[i][j]]++;
        }
        // checking array from 1 to 10 index
        for (int k = 1; k < 10; k++)
        {    
             // if any digit does not exist in array exit from thread
             if (validator[k]==0)
             {
                 thread_process_number[0]=0;
                 pthread_exit(0);
             }
        }
    }
    // if passed all tests thread is valid 
    thread_process_number[0]=1;
    pthread_exit(0);
}

// columns validation function 
void* col_valid(void* p) {
    parameters * data_cols = (parameters *) p; // taking begin parameters
    int begin_row = data_cols->row; // declaring begin values
    int begin_column = data_cols->column;
    for (int i = begin_column; i < 9; i++)
    {
        // array for maing sure that all numbers from 1 to 9 are available 
        int validator[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        for (int j = begin_row; j < 9; j++)
        {
         validator[sudoku_board[i][j]]++; 
        }
        // checking array from 1 to 10 index
        for (int k = 1; k < 10; k++)
        {
            // if any digit does not exist in array exit from thread
            if (validator[k]==0)
            {
                thread_process_number[1]=0;
                pthread_exit(0);
            }
        }
    }
    // if passed all tests thread is valid 
    thread_process_number[1]=1;
    pthread_exit(0);
}

// 3x3 square validation function 
void* square_valid(void* p) {
    parameters * data_sqr = (parameters *) p; // taking begin parameters
    int begin_row = data_sqr->row; // declaring begin values
    int begin_column = data_sqr->column;
    // array for maing sure that all numbers from 1 to 9 are available 
    int validator[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = begin_row; i < begin_row + 3; i++)
    {
        for (int j = begin_column; j < begin_column + 3; j++)
        {
         validator[sudoku_board[i][j]]++; 
        } 
    }
    // checking array from 1 to 10 index
    for (int k = 1; k < 10; k++)
        {
            // if any digit does not exist in array exit from thread
            if (validator[k]==0)
            {
                thread_process_number[thread_counter]=0;
                pthread_exit(0);
            }
        }

// if passed all tests thread is valid 
thread_process_number[thread_counter]=1;
thread_counter++;
pthread_exit(0);
}