#ifndef _ARGPARSE_H
#define _ARGPARSE_H

#include <getopt.h>
#include <iostream>
#include <stdlib.h>
#include <string>

struct options_t {
    std::string in_file;
    std::string out_file;
    int steps;
    double theta;
    double dt;
    bool visualize;
};

void get_opts(int argc, char** argv, options_t& opts);
#endif
