#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define GRID_SIZE 9
#define NUM_SUBGRIDS 9
#define NUM_THREADS 11  // 1 for rows, 1 for columns, 9 for subgrids

int sudoku[GRID_SIZE][GRID_SIZE];
int valid[NUM_THREADS] = {0};

typedef struct {
    int start_row;
    int start_col;
    int index;
} parameters;

// Check all rows
void *check_rows(void *param) {
    for (int i = 0; i < GRID_SIZE; i++) {
        int seen[GRID_SIZE+1] = {0};
        for (int j = 0; j < GRID_SIZE; j++) {
            int num = sudoku[i][j];
            if (num < 1 || num > 9 || seen[num]) {
                pthread_exit(NULL);
            }
            seen[num] = 1;
        }
    }
    valid[0] = 1;
    pthread_exit(NULL);
}

void *check_columns(void *param) {
    for (int j = 0; j < GRID_SIZE; j++) {
        int seen[GRID_SIZE+1] = {0};
        for (int i = 0; i < GRID_SIZE; i++) {
            int num = sudoku[i][j];
            if (num < 1 || num > 9 || seen[num]) {
                pthread_exit(NULL);
            }
            seen[num] = 1;
        }
    }
    valid[1] = 1;
    pthread_exit(NULL);
}

void *check_subgrid(void *param) {
    parameters *p = (parameters *) param;
    int seen[GRID_SIZE+1] = {0};
    for (int i = p->start_row; i < p->start_row + 3; i++) {
        for (int j = p->start_col; j < p->start_col + 3; j++) {
            int num = sudoku[i][j];
            if (num < 1 || num > 9 || seen[num]) {
                free(param);
                pthread_exit(NULL);
            }
            seen[num] = 1;
        }
    }
    valid[p->index] = 1;
    free(param);
    pthread_exit(NULL);
}

int main() {
    printf("Enter the Sudoku board (9 rows of 9 numbers, space-separated):\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (scanf("%d", &sudoku[i][j]) != 1) {
                fprintf(stderr, "Invalid input at row %d, column %d\n", i+1, j+1);
                return EXIT_FAILURE;
            }
        }
    }

    pthread_t threads[NUM_THREADS];
    int thread_count = 0;

    // Thread to check rows
    pthread_create(&threads[thread_count++], NULL, check_rows, NULL);

    // Thread to check columns
    pthread_create(&threads[thread_count++], NULL, check_columns, NULL);

    // Threads to check 3x3 subgrids
    int idx = 2;
    for (int row = 0; row < GRID_SIZE; row += 3) {
        for (int col = 0; col < GRID_SIZE; col += 3) {
            parameters *data = (parameters *) malloc(sizeof(parameters));
            data->start_row = row;
            data->start_col = col;
            data->index = idx;
            pthread_create(&threads[thread_count++], NULL, check_subgrid, (void *)data);
            idx++;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (valid[i] == 0) {
            printf("Sudoku solution is invalid.\n");
            return EXIT_SUCCESS;
        }
    }

    printf("Sudoku solution is valid!\n");
    return EXIT_SUCCESS;
}
