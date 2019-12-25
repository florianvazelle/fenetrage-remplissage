#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <iostream>
#include <algorithm>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobj_loader.h>

#include <GLShader.h>
#include <GLTexture.h>
#include <Mat4.h>
#include <GUI.h>

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
    float texX, texY;
    GLushort idx;

    bool operator==(const Vertex& v) const {
        return this->x == v.x && this->y == v.y && this->z == v.z
               && this->nx == v.nx && this->ny == v.ny && this->nz == v.nz;
    }
};

GLuint VBO, VAO, IBO;
GLShader g_BasicShader;
std::vector<GLushort> indices;

void Initialize() {
    GLenum error = glewInit();

    if (error != GLEW_OK) {
        std::cout << "erreur d'initialisation de GLEW!" << std::endl;
    }

    std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Vendor : " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer : " << glGetString(GL_RENDERER) << std::endl;

    g_BasicShader.LoadVertexShader("./resources/shaders/Basic.vs");
    g_BasicShader.LoadFragmentShader("./resources/shaders/Basic.fs");
    g_BasicShader.Create();

    const std::string model = "./resources/models/suzanne.obj";

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model.c_str());
    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    bool hasNormals = !attrib.normals.empty();
    bool hasTexCoords = !attrib.texcoords.empty();

    std::vector<Vertex> vertices;

    GLushort idx = 0;
    for (const tinyobj::shape_t& shape : shapes) {
        for (const tinyobj::index_t& index : shape.mesh.indices) {
            Vertex vertex = {};

            // position
            vertex.x = attrib.vertices[3 * index.vertex_index + 0];
            vertex.y = attrib.vertices[3 * index.vertex_index + 1];
            vertex.z = attrib.vertices[3 * index.vertex_index + 2];

            if (hasNormals) {
                // normal
                vertex.nx = attrib.normals[3 * index.normal_index + 0];
                vertex.ny = attrib.normals[3 * index.normal_index + 1];
                vertex.nz = attrib.normals[3 * index.normal_index + 2];
            }

            if (hasTexCoords) {
                // texCoord
                vertex.texX = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.texY = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];
            }

            auto it = std::find(vertices.begin(), vertices.end(), vertex);
            if (it != vertices.end()) {
                indices.push_back(it->idx);
            } else {
                vertex.idx = idx;
                vertices.push_back(vertex);
                indices.push_back(idx);
                idx++;
            }
        }
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

    auto basic = g_BasicShader.GetProgram();

    int loc_position = glGetAttribLocation(basic, "a_position");
    int loc_normal = glGetAttribLocation(basic, "a_normal");
    int loc_tex = glGetAttribLocation(basic, "a_tex");

    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_normal);
    glEnableVertexAttribArray(loc_tex);

    glVertexAttribPointer(loc_position, 3, GL_FLOAT, false, sizeof(Vertex), 0);
    glVertexAttribPointer(loc_normal, 3, GL_FLOAT, false, sizeof(Vertex), (const void*)offsetof(Vertex, nx));
    glVertexAttribPointer(loc_tex, 2, GL_FLOAT, false, sizeof(Vertex), (const void*)offsetof(Vertex, texX));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Shutdown() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    glDeleteVertexArrays(1, &VAO);
    g_BasicShader.Destroy();
}

void Display(GLFWwindow* window){
    int width, height;
    
    glfwGetWindowSize(window, &width, &height);

    glClearColor(0.f, 0.f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);

    auto basic = g_BasicShader.GetProgram();
    glUseProgram(basic);

    mat4 scaleMatrix;
    scaleMatrix.identity();
    scaleMatrix.scale(0.05f, 0.05f, 0.05f);
    int locSMatrix = glGetUniformLocation(basic, "u_scaleMatrix");
    glUniformMatrix4fv(locSMatrix, 1, GL_FALSE, scaleMatrix.data);

    mat4 rotationMatrix;
    rotationMatrix.identity();
    float currentTime = (float)glfwGetTime();
    rotationMatrix.rotateY(currentTime);
    int locRMatrix = glGetUniformLocation(basic, "u_rotationMatrix");
    glUniformMatrix4fv(locRMatrix, 1, GL_FALSE, rotationMatrix.data);

    mat4 translateMatrix;
    translateMatrix.identity();
    translateMatrix.translate(0, 0, -10);
    int locTMatrix = glGetUniformLocation(basic, "u_translateMatrix");
    glUniformMatrix4fv(locTMatrix, 1, GL_FALSE, translateMatrix.data);

    mat4 proj3DMatrix;
    float angle = 90.0f;
    float near = 0.1f;
    float far = 100.0f;
    float aspect = width / height;
    proj3DMatrix.identity();
    proj3DMatrix.makePerspective((angle * M_PI) / 180, aspect, near, far);
    int locMatrix = glGetUniformLocation(basic, "u_proj3DMatrix");
    glUniformMatrix4fv(locMatrix, 1, GL_FALSE, proj3DMatrix.data);

    static GLfloat lightPos[4] = {-5.0, 5.0, 5.0, 1.0};
    glUniform4fv(glGetUniformLocation(basic, "lumPos"), 1, lightPos);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

int main(void) {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

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
