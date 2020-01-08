#if defined(NANOGUI_GLAD)
    #if defined(NANOGUI_SHARED) && !defined(GLAD_GLAPI_EXPORT)
        #define GLAD_GLAPI_EXPORT
    #endif
       
    #include <glad/glad.h>
#else
    #if defined(__APPLE__)
        #define GLFW_INCLUDE_GLCOREARB
    #else
        #define GL_GLEXT_PROTOTYPES
    #endif

    #include <GL/glew.h>
#endif

#include <nanogui/opengl.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "GUI.h"
#include "GLShader.h"

GLShader g_BasicShader;

void Initialize() {
#if defined(NANOGUI_GLAD)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Could not initialize GLAD!");
    glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
#else
    GLenum error = glewInit();

    if (error != GLEW_OK) {
        std::cout << "erreur d'initialisation de GLEW!" << std::endl;
    }
#endif

    std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Vendor : " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer : " << glGetString(GL_RENDERER) << std::endl;

    g_BasicShader.LoadVertexShader("./resources/shaders/Basic.vs");
    g_BasicShader.LoadFragmentShader("./resources/shaders/Basic.fs");
    g_BasicShader.Create();

    myenv.p.init();
    myenv.f.init();
}

void Shutdown() {
    myenv.p.destroy();
    myenv.f.destroy();
    g_BasicShader.Destroy();
}

void Display(GLFWwindow* window){
    int width, height;
    
    glfwGetWindowSize(window, &width, &height);

    glClearColor(0.f, 0.f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, width, height);

    auto basic = g_BasicShader.GetProgram();
    glUseProgram(basic);

    if (myenv.f.size() == 2) myenv.f.setColor(myenv.currentColor);
    if (myenv.p.size() == 2) myenv.p.setColor(myenv.currentColor);
    myenv.f.draw(width, height, basic, (myenv.mode == myenv.Mode::fenetre), myenv.mouse);
    myenv.p.draw(width, height, basic, (myenv.mode == myenv.Mode::polygone), myenv.mouse);
}

int main(void) {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(640, 480, "Fenetrage & Remplissage", NULL, NULL);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    nanogui::ref<GUI> app = new GUI();

    glfwMakeContextCurrent(window);
    Initialize();
    app->init(window);

    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      Display(window);
      app->draw();

      glfwSwapBuffers(window);
    }

    Shutdown();
    glfwTerminate();
    return 0;
}
