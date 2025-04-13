cmake_minimum_required(VERSION 3.30)
project(Prac4_MPI_Sorting C)

set(CMAKE_C_STANDARD 11)

# Tell CMake where to find MPI
set(MPI_HOME /opt/homebrew/opt/open-mpi)
set(CMAKE_C_COMPILER ${MPI_HOME}/bin/mpicc)

# Include and link MPI
include_directories(${MPI_HOME}/include)
link_directories(${MPI_HOME}/lib)

#add_executable(matrix_sort matrix_sort.c)
add_executable(Prac4_MPI_Sorting main.c)

# Link MPI libraries
target_link_libraries(Prac4_MPI_Sorting mpi)

