#include <iostream>
#include <memory>
#include <mpi.h>
#include <random>
#include <unistd.h>

uint64_t g_numOfSamples = 100'000;

// Controller process
void controllerProcess(uint64_t id, uint64_t count) {

  const uint8_t WORKERS_NUM = count - 1;

  // Get hostname of the node running
  char hostname[256];
  gethostname(hostname, sizeof(hostname));
  std::cout << "Controller. Node: " << hostname << ", PID: " << id << ", Tasks: " << count << std::endl;

  // Broadcast value to each node
  // NUM_OF_SAMPLES/WORKERS_NUM doesn't need to sum up to NUM_OF_SAMPLES
  uint64_t sampleSize = g_numOfSamples / WORKERS_NUM;
  uint64_t realNumOfSamples = sampleSize * WORKERS_NUM;
  MPI_Bcast(&sampleSize, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);

  // Listen for results
  uint64_t buffer[WORKERS_NUM]{0};

  MPI_Request* requests = new MPI_Request[WORKERS_NUM];
  MPI_Status* statuses = new MPI_Status[WORKERS_NUM];

  for (size_t i = 0; i < WORKERS_NUM; ++i)
    MPI_Irecv(buffer + i, 1, MPI_UINT64_T, i + 1, 0, MPI_COMM_WORLD, &requests[i]);

  MPI_Waitall(WORKERS_NUM, requests, statuses);

  // Sum each result
  uint64_t sum = 0;
  for (size_t i = 0; i < WORKERS_NUM; ++i)
    sum += buffer[i];

  // Calculate final result
  double pi = 4.0 * sum / realNumOfSamples;
  std::cout << "Calculation result: " << pi << std::endl;

  delete[] requests;
  delete[] statuses;
}

// Worker processes
void workerProcess(uint64_t id, uint64_t count) {
  // Modern c++ random data generation
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_real_distribution<double> distribution(-1.0, 1.0);

  // Get hostname of the node running
  char hostname[256];
  gethostname(hostname, sizeof(hostname));

  // Get configuration from controller
  // MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
  uint64_t* sampleSize = new uint64_t[1];
  MPI_Bcast(sampleSize, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);

  // Print some data
  std::cout << "Task started. Node: " << hostname << ", PID: " << id << ", SAMPLE_SIZE: " << (*sampleSize) << std::endl;

  // Calculate PI with Monte Carlo
  uint64_t pointsInsideCircle = 0;
  for (size_t i = 0; i < (*sampleSize); ++i) {
    double x = distribution(generator);
    double y = distribution(generator);
    double distSquared = x * x + y * y;

    if (distSquared <= 1.0)
      pointsInsideCircle++;
  }

  // Return calculations
  MPI_Send(&pointsInsideCircle, 1, MPI_UINT64_T, 0, 0, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]) {

  if (argc > 1) {
    std::string sampleSizeStr = argv[1];
    try {
      g_numOfSamples = std::stoull(sampleSizeStr);
    } catch (const std::invalid_argument& e) {
      std::cout << "Invalid argument: " << e.what() << std::endl;
    } catch (const std::out_of_range& e) {
      std::cout << "Out of range: " << e.what() << std::endl;
    }
  }

  int PID, PCOUNT;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &PID);
  MPI_Comm_size(MPI_COMM_WORLD, &PCOUNT);

  if (PID == 0)
    controllerProcess(PID, PCOUNT);
  else
    workerProcess(PID, PCOUNT);

  MPI_Finalize();

  return 0;
}