#include "../include/camera.hh"
#include "iostream"
#include <GL/freeglut_std.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace mygl {

    void Camera::enterkeyboard(int key, int mouse_x, int mouse_y)
    {
        float speed = 0.1f;
        glm::vec3 target(0.0f, 0.0f, 1.0f);
        glm::vec3 left_dir = glm::cross(target, m_up);
        std::cout << mouse_x << std::endl;
        std::cout << mouse_y << std::endl;

       /* if (mouse_x != 0)
        {
            m_view[0] -= static_cast<float>(mouse_x) / 4;
        }

        if (mouse_y != 0)
           m_view[1] -= static_cast<float>(mouse_y) / 4;*/

        switch(key){
            case GLUT_KEY_UP:
                m_camera_position += (target * speed);
                break;
            case GLUT_KEY_DOWN:
                m_camera_position -= (target * speed);
                break;
            case GLUT_KEY_LEFT:
                m_camera_position += (left_dir * speed);
                break;
            case GLUT_KEY_RIGHT:
                m_camera_position -= (left_dir * speed);
                break;
        }
    }
    void Camera::enterkeyboard(unsigned char key, int mouse_x, int mouse_y)
    {
        float speed = 0.1f;
        glm::vec3 target(0.0f, 0.0f, 1.0f);
        glm::vec3 left_dir = glm::cross(target, m_up);

        std::cout << mouse_x << std::endl;
        std::cout << mouse_y << std::endl;

        /*if (mouse_x != 0)
            m_view[0] -= static_cast<float>(mouse_x) / 4;

        if (mouse_y != 0)
           m_view[1] -= static_cast<float>(mouse_y) / 4;*/

        switch(key){
            case GLUT_KEY_UP:
                m_camera_position += (target * speed);
                break;
            case GLUT_KEY_DOWN:
                m_camera_position -= (target * speed);
                break;
            case GLUT_KEY_LEFT:
                m_camera_position += (left_dir * speed);
                break;
            case GLUT_KEY_RIGHT:
                m_camera_position -= (left_dir * speed);
                break;
        }
    }
    void Camera::entermouse(int mouse_x, int mouse_y)
    {

        float deltaX = (mouse_x - GLUT_WINDOW_WIDTH / 2);
        float deltaY = (mouse_y - GLUT_WINDOW_HEIGHT / 2);

        yarn = (float)deltaX/100000.0;
        pitch = (float)deltaY/100000.0;

        if (mouse_x > centerX)
        {
            m_view = glm::rotate(m_view, yarn, glm::vec3(0, 1.0,0.0)); // Along X axis
        }
        else
        {
            m_view = glm::rotate(m_view, -yarn, glm::vec3(0, 1.0, 0.0)); // Along X axis
        }

        if (mouse_y > centerY)
        {
            m_view = glm::rotate(m_view, -pitch, glm::vec3(1.0, 0, 0)); // Along X axis
        }
        else
        {
            m_view = glm::rotate(m_view, pitch, glm::vec3(1.0, 0, 0)); // Along X axis
        }

        centerX = mouse_x;
        centerY = mouse_y;

    }
}
