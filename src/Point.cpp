#include "Point.h"

void Point::draw(uint32_t shader, Eigen::Vector2f mouse) {
    if (true /* && contain(mouse[0], mouse[1]) */) {

        hitbox[0] = { position[0] - margin, position[1] - margin };
        hitbox[1] = { position[0] - margin, position[1] + margin };
        hitbox[2] = { position[0] + margin, position[1] + margin };
        hitbox[3] = { position[0] + margin, position[1] - margin };

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Eigen::Vector2f), &hitbox[0], GL_DYNAMIC_DRAW);

        int loc_position = glGetAttribLocation(shader, "a_position");
        glEnableVertexAttribArray(loc_position);
        glVertexAttribPointer(loc_position, 2, GL_FLOAT, false, sizeof(Eigen::Vector2f), 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        int loc_color = glGetUniformLocation(shader, "u_color");
        glUniform4f(loc_color, 0.0f, 1.0f, 0.0f, 1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};
