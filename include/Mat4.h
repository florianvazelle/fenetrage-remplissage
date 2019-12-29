#include <iostream>
# define M_PI 3.14159265358979323846f

float cot(float x) {
    return cos(x) / sin(x);
}

struct mat4 {
    float data[16];

    void identity() {
        for (int i = 0; i < 16; i++) {
            if (i % 5 == 0) {
                this->data[i] = 1.0f;
            } else {
                this->data[i] = 0.0f;
            }
        }
    }

    void display() {
        for (int i = 0; i < 16; i++) {
            if (i % 4 == 0) {
                std::cout << std::endl;
            }
            std::cout << this->data[i] << ' ';
        }
        std::cout << std::endl;
    }

    void scale(float sx, float sy, float sz) {
        this->data[0] = sx;
        this->data[5] = sy;
        this->data[10] = sz;
    }

    void translate(float x, float y, float z) {
        this->data[12] = x;
        this->data[13] = y;
        this->data[14] = z;
    }

    void rotateY(float angle) {
        this->data[0] = cos(angle);
        this->data[2] = -sin(angle);
        this->data[8] = sin(angle);
        this->data[10] = cos(angle);
    }

    void rotateZ(float angle) {
        this->data[0] = cos(angle);
        this->data[1] = sin(angle);
        this->data[4] = -sin(angle);
        this->data[5] = cos(angle);
    }

    void ortho(float left, float right, float bottom, float top, float n, float f) {
        this->data[0] = 2 / (right - left);
        this->data[12] = -(right + left) / (right - left);
        this->data[5] = 2 / (top - bottom);
        this->data[13] = -(top + bottom) / (top - bottom);
        this->data[10] = -2 / (f - n);
        this->data[14] = -(f + n) / (f - n);
    }

    void proj3D(float fovy, float aspect, float n, float f) {
        // cot => cotangents
        float f2 = 1 / tan(fovy / 2);
        float rangeInv = 1.0f / (n - f);

        this->data[0] = f2 / aspect;
        this->data[5] = f2;
        this->data[10] = (n + f) * rangeInv;
        this->data[11] = -1;
        this->data[14] = n * f * rangeInv * 2;
        this->data[15] = 0;
    }

    void makePerspective(float fieldOfViewInRadians, float aspect, float n, float f) {
        float ymax, xmax;
        ymax = n * tanf(fieldOfViewInRadians * M_PI / 360);
        xmax = ymax * aspect;
        this->frustum(-xmax, xmax, -ymax, ymax, n, f);
    }

    void frustum(float left, float right, float bottom, float top, float n, float f){
        float temp, temp2, temp3, temp4;
        temp = 2.0f * n;
        temp2 = right - left;
        temp3 = top - bottom;
        temp4 = f - n;
        this->data[0] = temp / temp2;
        this->data[1] = 0.0;
        this->data[2] = 0.0;
        this->data[3] = 0.0;
        this->data[4] = 0.0;
        this->data[5] = temp / temp3;
        this->data[6] = 0.0;
        this->data[7] = 0.0;
        this->data[8] = (right + left) / temp2;
        this->data[9] = (top + bottom) / temp3;
        this->data[10] = (-f - n) / temp4;
        this->data[11] = -1.0;
        this->data[12] = 0.0;
        this->data[13] = 0.0;
        this->data[14] = (-temp * f) / temp4;
        this->data[15] = 0.0;
    }
};
