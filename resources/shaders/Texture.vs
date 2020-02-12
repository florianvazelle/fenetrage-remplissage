// Vertex
attribute vec2 a_position;
attribute vec2 a_texcoords;

varying vec2 v_texcoords;

void main(void) {
    v_texcoords = a_texcoords;
    gl_Position = vec4(a_position, 1.0, 1.0);
}