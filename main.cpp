#include <fstream>
#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    // Controller process
    int numWorkers;
    MPI_Comm_size(MPI_COMM_WORLD, &numWorkers);

    for (int i = 1; i < numWorkers; ++i) {
      // Send data to each worker
      int data = i * 10;
      MPI_Send(&data, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
  } else {
    // Worker processes
    int data;
    MPI_Status status;

    while (true) {
      // Receive data from the controller
      MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

      // Process the received data
      std::cout << "Worker " << rank << " received data: " << data << std::endl;

      // Break the loop if a termination condition is met
      if (data < 0) {
        break;
      }
    }
  }

  MPI_Finalize();

  return 0;
}