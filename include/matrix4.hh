#pragma once
#include <initializer_list>
#include <iostream>


#include "vector3.hh"

template <typename T>
class Matrix4
{
    public:
        T data[4][4] = {0};
        
        Matrix4(){}

        Matrix4(std::initializer_list<T> list)
        {
            auto index = 0;
            for (auto elt : list)
            {
                auto i = index / 4;
                auto j = index % 4;

                data[i][j] = elt;
                index++;
            }
        }

        Matrix4<T>& operator*=(const Matrix4<T>& rhs)
        {
            for (auto i = 0; i < 4; i++)
            {
                for (auto j = 0; j < 4; j++)
                {
                    data[i][j] = data[i][0] * rhs.data[0][j] + data[i][1] * rhs.data[1][j]
                        + data[i][2] * rhs.data[2][j] + data[i][3] * rhs.data[3][j];
                }
            }

            return *this;
        }
        static Matrix4<T> indentity()
        {
       return Matrix4<T>{
           1, 0, 0, 0,
           0, 1, 0, 0,
           0, 0, 1, 0,
           0, 0, 0, 1,
       };
        }


        void frucstum(Matrix4<T> &mat,
                const float &left, const float &right,
                const float &bottom, const float &top,
                const float &z_near, const float &z_far)
        {
            Matrix4<T> perspective_mat;
            perspective_mat.data[3][2] = -1;
            perspective_mat.data[0][0] = 2 * z_near / (right - left);
            perspective_mat.data[1][1] = 2 * z_near / (top - bottom);
            perspective_mat.data[2][0] = 2 * z_near / (right - left);
            perspective_mat.data[2][1] = (top + bottom) / (top - bottom);
            perspective_mat.data[2][2] = -(z_far + z_near) / (z_far - z_near);
            perspective_mat.data[2][3] = -2 * z_far * z_near / (z_far - z_near);

            mat *= perspective_mat;
        }

        void look_at(Matrix4 <T>&mat, const float &eyeX,
                const float &eyeY, const float &eyeZ, const float &centerX,
                const float &centerY, const float &centerZ, float upX, float upY, float upZ)
        {
            auto f = Vector3<float>(centerX - eyeX, centerY - eyeY, centerZ - eyeZ).normalize();
            auto up = Vector3<float>(upX, upY, upZ).normalize();
            auto s = f * up;
            auto u = s / s * f; 

            Matrix4<T> viewving_mat;
            viewving_mat.data[0][0] = s[0];
            viewving_mat.data[1][0] = s[1];
            viewving_mat.data[2][0] = s[2];
            viewving_mat.data[0][1] = u[0];
            viewving_mat.data[1][1] = u[1];
            viewving_mat.data[2][1] = u[2];
            viewving_mat.data[0][2] = -f[0];
            viewving_mat.data[1][2] = -f[1];
            viewving_mat.data[2][2] = -f[2];
            viewving_mat.data[3][3] = 1;

            mat *= viewving_mat;

        };
};

template <typename T>
   std::ostream &operator<<(std::ostream &out, const Matrix4<T> &m) {
     out << "{" << std::endl;

     for (auto i = 0; i < 4; i++) {
       out << "  ";
       for (auto j = 0; j < 4; j++) {
         out << " " << m[i][j] << ",";
       }
       out << std::endl;
     }

     return out << "}";
    }
using Mat = Matrix4<float>;
