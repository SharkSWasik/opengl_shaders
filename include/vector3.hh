#include <cmath>
#pragma once

template <typename T>
class Vector3
{
    public:
        T x;
        T y;
        T z;

        constexpr Vector3(T x_, T y_, T z_)
            : x{x_}
        , y{y_}
        , z{z_}
        {}

        constexpr explicit Vector3(T x)
            : Vector3{x, x, x}
        {}

        constexpr Vector3()
            : Vector3{0}
        {}

        const T& operator[](int i) const
        {
            return i == 0 ? x : i == 1 ? y : z;
        }

        T& operator[](int i)
        {
            return i == 0 ? x : i == 1 ? y : z;
        }

        T dot(const Vector3& rhs) const
        {
            return x * rhs.x + y * rhs.y + z * rhs.z;
        }

        T length() const
        {
            return dot(*this);
        }

        Vector3 normalize() const
        {
            auto len = length();

            if (len > 0)
            {
                auto factor = 1 / std::sqrt(len);
                return Vector3{x * factor, y * factor, z * factor};
            }

            return *this;
        }
        Vector3& operator*=(const Vector3& rhs)
        {
            x *= rhs.x;
            y *= rhs.y;
            z *= rhs.z;

            return *this;
        }

        Vector3& operator/=(const Vector3& rhs)
        {
            x /= rhs.x;
            y /= rhs.y;
            z /= rhs.z;

            return *this;
        }
};

        template <typename T>
            Vector3<T> operator*(Vector3<T> lhs, const Vector3<T>& rhs)
            {
                return lhs *= rhs;
            }

        template <typename T>
            Vector3<T> operator/(Vector3<T> lhs, const Vector3<T>& rhs)
            {
                return lhs /= rhs;
            }
