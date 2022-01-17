#include "../include/camera.hh"
#include "iostream"
#include <GL/freeglut_std.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtx/rotate_vector.hpp>


namespace mygl {

    void Camera::enterkeyboard(int key, int mouse_x, int mouse_y)
    {
        float speed = 0.01f;
        glm::vec3 target(0.0f, 0.0f, 1.0f);
        glm::vec3 left_dir = glm::cross(target, m_up);

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
        float speed = 0.01f;
        glm::vec3 target(0.0f, 0.0f, 1.0f);
        glm::vec3 left_dir = glm::cross(target, m_up);

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
    void Camera::entermouse(int x, int y)
    {

        glm::vec2 angles(0,0);

        int ww = glutGet(GLUT_WINDOW_WIDTH);
        int wh = glutGet(GLUT_WINDOW_HEIGHT);
        dx = x - ww / 2;
        dy = y - wh / 2;
        const float mousespeed = 0.00001;

        angles.x += dx * mousespeed;
        angles.y -= dy * mousespeed;

        m_mouse = angles;

      /*  if(angles.x < -M_PI)
            angles.x += M_PI * 2;
        else if(angles.x > M_PI)
            angles.x -= M_PI * 2;

        if(angles.y < -M_PI / 2)
            angles.y = -M_PI / 2;
        if(angles.y > M_PI / 2)
            angles.y = M_PI / 2;
        std::cout << m_camera_position.x <<  " " << m_camera_position.z<< std::endl;

        glm::vec3 lookat;
        lookat.x = sinf(angles.x) * cosf(angles.y);
        lookat.y = sinf(angles.y);
        lookat.z = cosf(angles.x) * cosf(angles.y);
        m_camera_look_at = lookat;
        m_view = glm::lookAt(
                    m_camera_position,//camera
                    m_camera_look_at,
                    m_up
                    );*/
    }
}
