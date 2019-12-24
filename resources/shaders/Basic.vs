uniform mat4 u_translateMatrix;
uniform mat4 u_rotationMatrix;
uniform mat4 u_scaleMatrix;
uniform mat4 u_proj3DMatrix;

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_tex;

varying vec3 v_normal;
varying vec3 v_position;
varying vec3 v_modPos;

void main(void) {
  mat4 modelViewMatrix = u_translateMatrix * u_rotationMatrix * u_scaleMatrix;
  mat4 projectionMatrix = u_proj3DMatrix;

  vec4 mp = modelViewMatrix * vec4(a_position, 1.0);
  v_normal = (transpose(inverse(modelViewMatrix)) * vec4(a_normal, 0.0)).xyz;
  v_position = a_position;
  v_modPos = mp.xyz;
  gl_Position = projectionMatrix * mp;
}