#ifndef MAT4_H
#define MAT4_H

#include <iostream>

class Mat4 {
public:
    float data[16];

    Mat4(void);
    Mat4(const Mat4& m);
    void scale(float sx, float sy, float sz);
    void translate(float x, float y, float z);

    Mat4 operator*(const Mat4& m);
    Mat4 operator*=(const Mat4& m);
    friend std::ostream& operator<<(std::ostream& os, const Mat4& m);
};

#endif