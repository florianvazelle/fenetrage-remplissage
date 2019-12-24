uniform vec4 lumPos;

varying vec3 v_normal;
varying vec3 v_modPos;

void main(void) {
  const vec4 lum_diffus = vec4(1, 0.5, 0.5, 1.0);
  const vec4 lum_amb = vec4(0.5, 0.5, 1, 1.0);
  const float IlumAmb = 0.15;

  /* vecteur vue */
  vec3 V = vec3(0, 0, -1);
  /* Lumière vers sommet */
  vec3 L = normalize(v_modPos - lumPos.xyz);
  /* reflet de L par rapport à la normale au fragment */
  vec3 R = reflect(L, v_normal);
  /* Intensité de speculaire */
  float Ispec = pow(clamp(dot(R, -V), 0, 1), 10);
  /* Intensité lumière diffuse : Phong par rapport à la normale au fragment */
  float IdiffusPhong = dot(v_normal, -L);

  gl_FragColor = lum_diffus * IdiffusPhong + lum_amb * IlumAmb;
}