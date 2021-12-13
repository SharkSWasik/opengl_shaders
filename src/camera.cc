#include "../include/camera.hh"

namespace mygl {

    void Camera::enterkeyboard(int key)
    {
        float speed = 0.1f;
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
    void Camera::enterkeyboard(unsigned char key)
    {
        float speed = 0.1f;
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
}
