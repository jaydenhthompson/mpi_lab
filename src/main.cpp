#include "argparse.hpp"
#include "quadtree.hpp"

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char **argv) {
  int id, num_processes;

  options_t opts;
  get_opts(argc, argv, opts);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
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

  auto work_size = num_bodies < num_processes ? num_processes : num_bodies % num_processes == 0 ? num_bodies
                                                                                                : num_bodies + (num_processes - (num_bodies % num_processes));
  std::vector<int> work(work_size, -1);
  for(int i = 0; i < num_bodies; i++)
  {
    work[i] = i;
  }

  std::cout << "work size" << work_size << std::endl;

  int per_proc = work_size / num_processes;
  stc::vector<int> my_work(per_proc);
  MPI_Scatter(work.data(), per_proc, MPI_INT, my_work.data(), per_proc, MPI_INT, 0, MPI_COMM_WORLD);

  // TODO : MPI_Gather(my_work, per_proc, MPI_INT, work.data(), per_proc, MPI_INT, 0, MPI_COMM_WORLD);

  if (id == 0) // parent
  { 
  }
  else // child 
  {
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}