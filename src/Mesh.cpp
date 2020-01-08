#include "Mesh.h"

void Mesh::addVertex(Eigen::Vector2f vec) {
    mesh.push_back(vec);
}

void Mesh::init() {
    glGenBuffers(1, &_vbo);
}

void Mesh::destroy() {
    glDeleteBuffers(1, &_vbo);
}

void Mesh::draw(int width, int height, uint32_t shader, bool includeMouse, Eigen::Vector2f mouse) {
    if (mesh.size() > 0) {
        std::vector<float> tmp;

        float xClip, yClip;
        for (auto v : mesh) {
            xClip = ((v[0] + 0.5f) / width) * 2.0f - 1.0f;
            yClip = 1.0f - ((v[1] + 0.5f) / height) * 2.0f;
            tmp.push_back(xClip);
            tmp.push_back(yClip);
        }

        if (includeMouse) {
            float xClip = ((mouse[0] + 0.5f) / width) * 2.0f - 1.0f;
            float yClip = 1.0f - ((mouse[1] + 0.5f) / height) * 2.0f;
            tmp.push_back(xClip);
            tmp.push_back(yClip);
        }

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, tmp.size() * sizeof(float), &tmp[0], GL_STATIC_DRAW);

        int loc_position = glGetAttribLocation(shader, "a_position");
        glEnableVertexAttribArray(loc_position);
        glVertexAttribPointer(loc_position, 2, GL_FLOAT, false, sizeof(float) * 2, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        int loc_color = glGetUniformLocation(shader, "u_color");
        glUniform4f(loc_color, color.r(), color.g(), color.b(), color.w());

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        if (tmp.size() > 2)
            glDrawArrays(GL_LINE_STRIP, 0, tmp.size() / 2);
        else if (tmp.size() == 2)
            glDrawArrays(GL_LINES, 0, tmp.size() / 2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Mesh::setColor(nanogui::Color c) {
    color = c;
}
