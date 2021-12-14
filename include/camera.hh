#pragma once

#include "GL/glew.h"
#include "GL/glu.h"
#include "GL/freeglut.h"

#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

namespace mygl
{
    class Camera
    {
    public:
        Camera()
        {
            // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
            m_projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);

            // Camera matrix
            m_camera_position = glm::vec3(0., 1.f, -6.f);
            m_camera_look_at = glm::vec3(0.f, 0.f, 0.f);
            m_up = glm::vec3(0.0f, 1.0f, 0.0f);

            m_view = glm::lookAt(
                    m_camera_position,//camera
                    m_camera_look_at,
                    m_up
                    );


            // Model matrix : an identity matrix (model will be at the origin)
            m_identity = glm::mat4(1.0f);

            centerX = GLUT_WINDOW_WIDTH / 2;
            centerY = GLUT_WINDOW_HEIGHT / 2;
        }
        ~Camera();

        void enterkeyboard(int key, int mouse_x, int mouse_y);
        void enterkeyboard(unsigned char key, int mouse_x, int mouse_y);

        void entermouse(int mouse_x, int mouse_y);
        
        float pitch;
        float yarn;
        int centerX;
        int centerY;

        glm::vec3 m_camera_look_at;
        glm::vec3 m_camera_position;
        glm::vec3 m_up;
        glm::mat4 m_projection;
        glm::mat4 m_view;
        glm::mat4 m_identity;
    };
}
