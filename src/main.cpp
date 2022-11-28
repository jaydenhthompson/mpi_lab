#include "argparse.h"

#include <mpi.h>
#include <iostream>

int main(int argc, char **argv) {
  int id, numProcesses;

  options_t opts;
  get_opts(argc, argv, opts);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  std::cout << "HI: " << id << std::endl;

  if (id == 0) {
    // parent
  }
  else {
    // child
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}