#include "Application.h"

//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);

int main()
{
    CADMageddon::Application app;
    app.Init();
    app.Run();


    return EXIT_SUCCESS;
}