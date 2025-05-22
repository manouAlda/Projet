#include "core/Application.h"
#include <iostream>

int main(){
    try
    {
        Application app;
        app.initApp();
        app.getRoot()->startRendering();
        app.closeApp();
    }
    catch (const Ogre::Exception& e)
    {
        std::cerr << "Exception Ogre: " << e.getFullDescription().c_str() << std::endl;
    }
    return 0;
}
