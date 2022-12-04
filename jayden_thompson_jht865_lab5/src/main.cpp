#include "argparse.hpp"
#include "quadtree.hpp"

#include <fstream>
#include <iostream>
#include <mpi.h>
#include <sstream>

int main(int argc, char** argv)
{
    int id, num_processes;

    options_t opts;
    get_opts(argc, argv, opts);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    auto start = MPI_Wtime();

    std::ifstream in(opts.in_file);
    int num_bodies;
    in >> num_bodies;

    std::vector<body> bodies(num_bodies);
    for (int i = 0; i < num_bodies; i++) {
        bodies[i].readIn(in);
    }

    size_t work_size = num_bodies < num_processes ? num_processes
                                                : num_bodies % num_processes == 0 ? num_bodies
                                                                                  : num_bodies + (num_processes - (num_bodies % num_processes));
    while (bodies.size() < work_size)
        bodies.push_back(body());

    if (num_processes <= 1) {
        for (int i = 0; i < opts.steps; i++) {
            window wind(0, 4, 0, 4);
            quadtree tree(bodies, wind);
            for (auto& e : bodies) {
                tree.calcBody(e, opts.dt, opts.theta);
            }
        }
    } else {
        int per_proc = work_size / num_processes;
        std::vector<body> my_work(per_proc);
        auto per_proc_bytes = per_proc * sizeof(body);
        MPI_Scatter(bodies.data(), per_proc_bytes, MPI_BYTE, my_work.data(), per_proc_bytes, MPI_BYTE, 0, MPI_COMM_WORLD);

        for (int i = 0; i < opts.steps; i++) {
            window wind(0, 4, 0, 4);
            quadtree tree(bodies, wind);
            for(auto & e : my_work){
                if(e.index < 0) continue;
                tree.calcBody(e, opts.dt, opts.theta);
            }
            MPI_Allgather(my_work.data(), per_proc_bytes, MPI_BYTE, bodies.data(), per_proc_bytes, MPI_BYTE, MPI_COMM_WORLD);
        }
    }

    auto end = MPI_Wtime();
    if (id == 0) {
        printf("%lf\n", end - start);
        std::ofstream outfile(opts.out_file);
        outfile << num_bodies << std::endl;
        for (auto& e : bodies) {
            if (e.index >= 0) {
                outfile << e.index << " "
                        << e.x << " "
                        << e.y << " "
                        << e.mass << " "
                        << e.vx << " "
                        << e.vy << " "
                        << std::endl;
            }
        }
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}