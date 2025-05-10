#include <iostream>
#include "Header/World.hpp"
#include "Header/Physics.hpp"

using namespace std;

int main() {

    try {
        World app;
        app.initApp();
        app.getRoot()->startRendering();
        app.closeApp();
    } catch (const std::exception& e) {
        std::cerr << "An exception has occurred: " << e.what() << std::endl;
    }

    return 0;
}