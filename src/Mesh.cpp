#include <iostream>
#include "Mesh.h"

void Mesh::draw(int width, int height, uint32_t shader, bool includeMouse, Eigen::Vector2f mouse) {
    // Si il y a des sommets
    if (mesh.size() > 0) {
        // On copie l'ensemble des sommets dans une variable temporaire
        std::vector<Eigen::Vector2f> tmp(mesh.size());

        for (int i = 0; i < mesh.size(); i++) {
            tmp[i] = mesh[i].getPosition();
        }

        // Si on est entrain de dessinee le mesh, on ajoute la coordonnee de la souris a la liste des sommets
        if (includeMouse && !close) {
            tmp.push_back(mouse);
        }

        // On bind le vbo pour le modifier
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, tmp.size() * sizeof(Eigen::Vector2f), &tmp[0], GL_DYNAMIC_DRAW);

        int loc_position = glGetAttribLocation(shader, "a_position");
        glEnableVertexAttribArray(loc_position);
        glVertexAttribPointer(loc_position, 2, GL_FLOAT, false, sizeof(Eigen::Vector2f), 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // J'envoie la couleur du mesh dans le shader
        int loc_color = glGetUniformLocation(shader, "u_color");
        glUniform4f(loc_color, color.r(), color.g(), color.b(), color.w());

        // Je bind le VBO du mesh
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        // Et je le dessine
        if (close) {
            glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)tmp.size());
        }
        else {
            if (tmp.size() > 2)
                glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)tmp.size());
            else if (tmp.size() == 2)
                glDrawArrays(GL_LINES, 0, (GLsizei)tmp.size());
        }
        // Je bind a zero, pour eviter toute modifications du vbo
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    for (int i = 0; i < mesh.size(); i++) {
        mesh[i].draw(shader, mouse);
    }
};

std::vector<Point>::const_iterator Mesh::contain(float x, float y) const {
    for (std::vector<Point>::const_iterator p = mesh.begin(); p != mesh.end(); ++p) {
        if (p->contain(x, y)) {
            return p;
        }
    }
    return mesh.end();
};

bool Mesh::isValid(std::vector<Point>::const_iterator it) const {
    return it != mesh.end();
};

void Mesh::move(std::vector<Point>::const_iterator it, Eigen::Vector2f coordGL) {
    for (std::vector<Point>::iterator p = mesh.begin(); p != mesh.end(); ++p) {
        if (it == p) {
            p->setPosition(coordGL);
        }
    }
};

void Mesh::destroy() {
    glDeleteBuffers(1, &_vbo);
    clear();
}