#include "TraceRoute.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <hostname>" << std::endl;
        return 1;
    }

    TraceRoute tracer(argv[1]);
    tracer.run();

    return 0;
}