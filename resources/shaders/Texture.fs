// Fragment
varying vec2 v_texcoords;
uniform sampler2D u_TextureSampler;

void main(void) {
    gl_FragColor = texture2D(u_TextureSampler, vec2(v_texcoords.x, 1.0 - v_texcoords.y));
}