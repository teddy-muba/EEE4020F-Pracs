Prac4 MPI Sorting (EEE4120F)

Build:
    make

Run:
    mpirun -np 5 ./mpisort 100x100.csv result.csv

- Uses column-wise bubble sort.
- Implemented using MPI blocking communication.