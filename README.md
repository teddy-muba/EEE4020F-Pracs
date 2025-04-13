# EEE4020F-Pracs: MPI-Based Matrix Sorting
This repository contains the implementation of a parallel matrix sorting program using the Message Passing Interface (MPI). The primary goal of this project is to sort a matrix in parallel by performing bubble sort on each column, leveraging the power of distributed systems for faster processing.

The program reads a matrix from a CSV file, distributes the matrix to multiple processes, performs column-wise bubble sorting, and then writes the sorted matrix back to an output CSV file. This is done using the MPI framework to distribute work among multiple processes efficiently.

# Overview
This project demonstrates how MPI can be used to perform parallel processing on a matrix for sorting. Each process is responsible for sorting a subset of the matrix's columns, and after the sorting is completed, the results are gathered and written back to a file.

The matrix is randomly generated, and its size is specified in the code. The program can be easily extended or modified to handle different matrix sizes or sorting methods.

# Key Concepts:

- MPI: A parallel programming model used to distribute and coordinate tasks among multiple processes running on a single or multiple machines.

- Bubble Sort: A simple sorting algorithm, which in this case is applied column-wise to a matrix.

- Distributed Computation: The matrix is split across multiple processes, with each process sorting one or more columns independently.

# Features
- Random Matrix Generation: A random matrix is generated and saved to input.csv.

- Parallel Sorting: Each column is sorted independently in parallel using MPI, reducing the overall execution time.

- CSV I/O: Matrix data is read from and written to CSV files, making it easy to use with other tools.

- MPI Communication: The matrix is split and distributed to different processes, and the sorted results are collected and written back.

# Prerequisites
MPI: The project requires OpenMPI to run. You need to have OpenMPI installed and configured on your system.

# Run Program
The program is now ready to be executed. You can run it with the following command:
- mpirun -np 4 ./Prac4_MPI_Sorting input.csv output.csv
Here, input.csv is the input file, and output.csv is where the sorted matrix will be saved.

# Usage
Command Line Arguments
input.csv: The path to the input CSV file that contains the matrix.

output.csv: The path where the sorted matrix will be saved.

# Example:
mpirun -np 4 ./Prac4_MPI_Sorting input.csv output.csv
This will run the program with 4 processes, reading the matrix from input.csv, sorting it in parallel, and writing the result to output.csv.

# File Generation
The program generates a random matrix of size 8x6 (rows x columns) and saves it to input.csv by default. You can adjust the size of the matrix or modify the file generation behavior if needed.

# File Descriptions
main.c:
- The main program where the MPI-based parallel matrix sorting is implemented. It handles:

- Matrix generation (generate_random_matrix_csv function)

- Reading from the input CSV (read_matrix_from_csv)

- Sorting each column in parallel using MPI

- Writing the sorted matrix to the output CSV (write_matrix_to_csv)

CMakeLists.txt
- CMake configuration file to set up the build environment and link the necessary MPI libraries.

- input.csv
The input file containing the matrix to be sorted. The matrix is read from this file, sorted, and then written to output.csv.

- output.csv
The file where the sorted matrix is saved after the parallel sorting process completes.
