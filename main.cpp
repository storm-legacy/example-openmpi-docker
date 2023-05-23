#include <iostream>
#include <memory>
#include <mpi.h>
#include <random>
#include <unistd.h>
#include <vector>

// Controller process
void controllerProcess(uint64_t id) {

  const uint64_t NUM_OF_SAMPLES = 250000;
  const uint8_t WORKERS_NUM = 8;

  // Broadcast value to each node
  // NUM_OF_SAMPLES/WORKERS_NUM doesn't need to sum to NUM_OF_SAMPLES
  uint64_t sampleSize = NUM_OF_SAMPLES / WORKERS_NUM;
  MPI_Bcast(&sampleSize, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);

  // Listen for results
  uint64_t buffer[WORKERS_NUM]{0};

  MPI_Request* requests = new MPI_Request[WORKERS_NUM];
  MPI_Status* statuses = new MPI_Status[WORKERS_NUM];

  for (auto i = 0; i < WORKERS_NUM; ++i)
    MPI_Irecv(buffer + i, 1, MPI_UINT64_T, i + 1, 0, MPI_COMM_WORLD, &requests[i]);

  MPI_Waitall(WORKERS_NUM, requests, statuses);

  delete[] requests;
  delete[] statuses;
}

// Worker processes
void workerProcess(uint64_t id, uint64_t count) {
  // Modern c++ random data generation
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_real_distribution<double> distribution(0.0, 1.0);

  // Get hostname of the node running
  char hostname[256];
  gethostname(hostname, sizeof(hostname));

  // Print some data
  std::cout << "Task started. Node: " << hostname << " PID: " << id << std::endl;

  // Get configuration from controller
  // MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
  uint64_t* sampleSize = new uint64_t[1];
  MPI_Bcast(sampleSize, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);

  // Return calculations
  uint64_t* result = new uint64_t[1];
  MPI_Send(result, 1, MPI_UINT64_T, 0, 0, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]) {

  int PID, PCOUNT;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &PID);
  MPI_Comm_size(MPI_COMM_WORLD, &PCOUNT);

  if (PID == 0)
    controllerProcess(PID);
  else
    workerProcess(PID, PCOUNT);

  MPI_Finalize();

  return 0;
}