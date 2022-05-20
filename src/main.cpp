#include <iostream>
#include <memory>

#include "LLShooter.hpp"

int main(int argc, char** argv) {
    try {
        std::unique_ptr<LLShooter> game = std::make_unique<LLShooter>();
        game->start();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: \"" << e.what() << '\"' << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
