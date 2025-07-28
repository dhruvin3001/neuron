#include "neuron/cli.hpp"

#include <iostream>

int main(int argc, char** argv) {
    neuron::CLI cli(argc, argv);
    return cli.run();
}
