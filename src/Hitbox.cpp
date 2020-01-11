#include <vector>

#include "Hitbox.h"

Hitbox::Hitbox() {
	mesh.reserve(4);
	color = nanogui::Color(Eigen::Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
}

bool Hitbox::contain(float x, float y) {
	return ((x >= this->mesh[0][0]) && (y >= this->mesh[0][1]) &&
		(x <= this->mesh[2][0]) && (y <= this->mesh[2][1]));
}

void Hitbox::draw(uint32_t shader) {
    // Si il y a des sommets
    if (mesh.size() > 0) {
        // On bind le vbo pour le modifier
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(Eigen::Vector2f), &mesh[0], GL_DYNAMIC_DRAW);

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
        glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)mesh.size());
        // Je bind a zero, pour eviter toute modifications du vbo
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
