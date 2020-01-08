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

GLuint VBO;
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
}

void Shutdown() {
    glDeleteBuffers(1, &VBO);
    g_BasicShader.Destroy();
}

void drawVec(std::vector<float> vec, int width, int height) {
    auto basic = g_BasicShader.GetProgram();
    glUseProgram(basic);

    if (myenv.mode != myenv.Mode::noop) {
        float xClip = ((myenv.mouse[0] + 0.5f) / width) * 2.0f - 1.0f;
        float yClip = 1.0f - ((myenv.mouse[1] + 0.5f) / height) * 2.0f;
        vec.push_back(xClip);
        vec.push_back(yClip);
    }

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(float), &vec[0], GL_STATIC_DRAW);

    int loc_position = glGetAttribLocation(basic, "a_position");
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 2, GL_FLOAT, false, sizeof(float) * 2, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (vec.size() > 4)
        glDrawArrays(GL_LINE_STRIP, 0, vec.size() / 2);
    else if (vec.size() == 4)
        glDrawArrays(GL_LINES, 0, vec.size() / 2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Display(GLFWwindow* window){
    int width, height;
    
    glfwGetWindowSize(window, &width, &height);

    glClearColor(0.f, 0.f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, width, height);

    float xClip, yClip;
    std::vector<float> fenetreVec;
    for (auto v : myenv.f.mesh) {
        xClip = ((v[0] + 0.5f) / width) * 2.0f - 1.0f;
        yClip = 1.0f - ((v[1] + 0.5f) / height) * 2.0f;
        fenetreVec.push_back(xClip);
        fenetreVec.push_back(yClip);
    }

    std::vector<float> polygoneVec;
    for (auto v : myenv.p.mesh) {
        xClip = ((v[0] + 0.5f) / width) * 2.0f - 1.0f;
        yClip = 1.0f - ((v[1] + 0.5f) / height) * 2.0f;
        polygoneVec.push_back(xClip);
        polygoneVec.push_back(yClip);
    }

    if (fenetreVec.size() != 0) {
        drawVec(fenetreVec, width, height);
        fenetreVec.clear();
    }

    if (polygoneVec.size() != 0) {
        drawVec(polygoneVec, width, height);
        polygoneVec.clear();
    }
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
