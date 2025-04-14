#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

#define MASTER 0
#define HEADER_TAG 1
#define DATA_TAG 2
#define RESULT_TAG 3


typedef struct {
    int num_rows;
    int num_cols_assigned;
} MatrixHeader;

void generate_random_matrix_csv(const char *filename, int rows, int cols) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error creating file %s\n", filename);
        exit(1);
    }

    srand((unsigned int) time(NULL));  // Seed RNG

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            double val = ((double)rand() / RAND_MAX) * 100.0;  // [0, 100]
            fprintf(fp, "%.2f", val);
            if (j < cols - 1)
                fprintf(fp, ",");
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

void bubble_sort_column(double *matrix, int rows, int col_index, int total_cols) {
    for (int i = 0; i < rows - 1; i++) {
        for (int j = 0; j < rows - i - 1; j++) {
            int idx1 = j + col_index * rows;
            int idx2 = j + 1 + col_index * rows;
            if (matrix[idx1] > matrix[idx2]) {
                double temp = matrix[idx1];
                matrix[idx1] = matrix[idx2];
                matrix[idx2] = temp;
            }
        }
    }
}

void read_matrix_from_csv(const char *filename, double **matrix, int *rows, int *cols) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error opening file %s\n", filename);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    char line[10000];
    int row = 0, current_cols = 0;

    while (fgets(line, sizeof(line), fp)) {
        current_cols = 0;
        char *token = strtok(line, ",");
        while (token) {
            current_cols++;
            token = strtok(NULL, ",");
        }
        row++;
    }

    *rows = row;
    *cols = current_cols;
    rewind(fp);

    *matrix = (double *)malloc((*rows) * (*cols) * sizeof(double));
    if (!(*matrix)) {
        fprintf(stderr, "Memory allocation failed.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int index = 0;
    while (fgets(line, sizeof(line), fp)) {
        char *token = strtok(line, ",");
        while (token) {
            (*matrix)[index++] = atof(token);
            token = strtok(NULL, ",");
        }
    }

    fclose(fp);
}

void write_matrix_to_csv(const char *filename, double *matrix, int rows, int cols) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error writing file %s\n", filename);
        return;
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(fp, "%.2f", matrix[j * rows + i]);
            if (j < cols - 1) fprintf(fp, ",");
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 3) {
        if (rank == MASTER)
            fprintf(stderr, "Usage: %s input.csv output.csv [rows cols]\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int gen_rows = 8, gen_cols = 6;  // Default size
    if (argc >= 5) {
        gen_rows = atoi(argv[3]);
        gen_cols = atoi(argv[4]);
    }

    if (rank == MASTER) {
        printf("Generating matrix: %dx%d -> %s\n", gen_rows, gen_cols, argv[1]);
        generate_random_matrix_csv(argv[1], gen_rows, gen_cols);
    }

    MPI_Barrier(MPI_COMM_WORLD);  // Ensure file is ready before read

    // --- Start timing right before master starts computation ---
    double start_time = MPI_Wtime();

    if (rank == MASTER) {
        double *matrix = NULL;
        int rows, cols;
        read_matrix_from_csv(argv[1], &matrix, &rows, &cols);

        int workers = size - 1;
        int cols_per_proc = cols / workers;
        int extra_cols = cols % workers;
        int offset = 0;

        for (int i = 1; i <= workers; i++) {
            int num_cols = cols_per_proc + (i <= extra_cols ? 1 : 0);
            MatrixHeader header = {rows, num_cols};
            MPI_Send(&header, sizeof(header), MPI_BYTE, i, HEADER_TAG, MPI_COMM_WORLD);
            MPI_Send(&matrix[offset * rows], rows * num_cols, MPI_DOUBLE, i, DATA_TAG, MPI_COMM_WORLD);
            offset += num_cols;
        }

        double *result = (double *)malloc(rows * cols * sizeof(double));
        if (!result) {
            fprintf(stderr, "Result memory allocation failed.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        offset = 0;
        for (int i = 1; i <= workers; i++) {
            int num_cols = cols_per_proc + (i <= extra_cols ? 1 : 0);
            MPI_Recv(&result[offset * rows], rows * num_cols, MPI_DOUBLE, i, RESULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            offset += num_cols;
        }

        write_matrix_to_csv(argv[2], result, rows, cols);
        printf("Sorted matrix written to %s\n", argv[2]);

        free(matrix);
        free(result);
    } else {
        MatrixHeader header;
        MPI_Recv(&header, sizeof(header), MPI_BYTE, MASTER, HEADER_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        double *submatrix = (double *)malloc(header.num_rows * header.num_cols_assigned * sizeof(double));
        if (!submatrix) {
            fprintf(stderr, "Rank %d: Allocation failed.\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        MPI_Recv(submatrix, header.num_rows * header.num_cols_assigned, MPI_DOUBLE, MASTER, DATA_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int j = 0; j < header.num_cols_assigned; j++) {
            bubble_sort_column(submatrix, header.num_rows, j, header.num_cols_assigned);
        }

        MPI_Send(submatrix, header.num_rows * header.num_cols_assigned, MPI_DOUBLE, MASTER, RESULT_TAG, MPI_COMM_WORLD);
        free(submatrix);
    }

    // --- End timing after final result written ---
    double end_time = MPI_Wtime();

    if (rank == MASTER) {
        printf("Matrix Size: %dx%d, Total Execution Time: %.3f seconds\n", gen_rows, gen_cols, end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
