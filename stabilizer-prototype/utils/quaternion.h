
#ifndef STABILIZER_PROTOTYPE_QUATERNION_H
#define STABILIZER_PROTOTYPE_QUATERNION_H

#include <cmath>
#include <iostream>

template<typename T>
class Quaternion {
public:
    T a, b, c, d;

    Quaternion(T w, T x, T y, T z) : a(w), b(x), c(y), d(z) {}

    Quaternion() : Quaternion(0, 0, 0, 0) {}

    template<typename U>
    Quaternion(const Quaternion<U> another): Quaternion(another.a, another.b, another.c, another.d) {}

    Quaternion operator+(const Quaternion &q) const {
        return Quaternion(a + q.a, b + q.b, c + q.c, d + q.d);
    }

    Quaternion operator-(const Quaternion &q) const {
        return Quaternion(a - q.a, b - q.b, c - q.c, d - q.d);
    }

    Quaternion operator*(const Quaternion &q) const {
        return Quaternion(
                a * q.a - b * q.b - c * q.c - d * q.d,
                a * q.b + b * q.a + c * q.d - d * q.c,
                a * q.c - b * q.d + c * q.a + d * q.b,
                a * q.d + b * q.c - c * q.b + d * q.a
        );
    }

    Quaternion operator*(const T &t) const {
        return Quaternion(a * t, b * t, c * t, d * t);
    }

    Quaternion operator/(const T &t) const {
        return Quaternion(a / t, b / t, c / t, d / t);
    }

    Quaternion conjugate() const {
        return Quaternion(a, -b, -c, -d);
    }

    T square_modulus() const {
        return a * a + b * b + c * c + d * d;
    }

    T modulus() const {
        return std::sqrt(a * a + b * b + c * c + d * d);
    }

    Quaternion inv() const {
        return conjugate() / square_modulus();
    }

    Quaternion &normalize_in_place() {
        *this = *this / modulus();
        return *this;
    }

    Quaternion normalized() const {
        return *this / modulus();
    }

    Quaternion rotate_point(Quaternion axis, T theta) const {
        // assume: this->a == 0, axis.a == 0
        axis.normalize_in_place();
        T c = std::cos(theta / 2);
        T s = std::sin(theta / 2);
        Quaternion q(c, axis.b * s, axis.c * s, axis.d * s);
        return q * (*this) * q.inv();
    }
};

template<typename T>
std::ostream &operator<<(std::ostream &os, const Quaternion<T> &q) {
    os << "Quaternion(" << q.a << ", " << q.b << ", " << q.c << ", " << q.d << ")";
    return os;
}

extern template
class Quaternion<double>;

#endif //STABILIZER_PROTOTYPE_QUATERNION_H
