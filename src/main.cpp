#include <iostream>
#include <memory>

#include "LLShooter.hpp"

int main(int argc, char** argv) {
#ifdef NDEBUG
    try {
#endif
        std::unique_ptr<LLShooter> game = std::make_unique<LLShooter>();
        game->start();
#ifdef NDEBUG
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: \"" << e.what() << '\"' << std::endl;
        return EXIT_FAILURE;
    }
#endif

    return EXIT_SUCCESS;
}
