#include "argparse.hpp"
#include "quadtree.hpp"

#include <mpi.h>
#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
  int id, numProcesses;

  options_t opts;
  get_opts(argc, argv, opts);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  std::ifstream in(opts.in_file);
  int num_bodies;
  in >> num_bodies;

  std::vector<body *> bodies(num_bodies);
  for(int i = 0; i < num_bodies; i++)
  {
    bodies[i] = new body();
    bodies[i]->readIn(in);
  }

  window * wind = new window(0, 4, 0, 4);
  quadtree tree(bodies, wind);

  if (id == 0) {
    // parent
  }
  else {
    // child
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}