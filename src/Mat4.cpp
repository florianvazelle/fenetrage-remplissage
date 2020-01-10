#include "Mat4.h"

Mat4::Mat4() {
    for (int i = 0; i < 16; i++) {
        if (i % 5 == 0) {
            data[i] = 1.0f;
        }
        else {
            data[i] = 0.0f;
        }
    }
}

Mat4::Mat4(const Mat4& m) {
    for (int i = 0; i < 16; i++) {
        data[i] = m.data[i];
    }
}

Mat4 Mat4::operator*(const Mat4& m) {
    Mat4 tmp(*this);
    return tmp *= m;
}

Mat4 Mat4::operator*=(const Mat4& m) {
    Mat4 tmp(*this);
    Mat4 res = Mat4();
    float sum;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            sum = 0;
            for (int k = 0; k < 4; k++) {
                sum = sum + tmp.data[(i * 4) + k] * m.data[(k * 4) + j];
            }
            res.data[(i * 4) + j] = sum;
        }
    }
    return res;
}

void Mat4::scale(float sx, float sy, float sz) {
    this->data[0] = sx;
    this->data[5] = sy;
    this->data[10] = sz;
}

void Mat4::translate(float x, float y, float z) {
    this->data[12] = x;
    this->data[13] = y;
    this->data[14] = z;
}

std::ostream& operator<<(std::ostream& os, const Mat4& m) {
    for (int i = 0; i <= 16; i++) {
        if (i % 4 == 0) {
            std::cout << "\n";
        }
        std::cout << m.data[i] << ' ';
    }
    return os;
}