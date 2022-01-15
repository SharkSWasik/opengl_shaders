#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/glu.h"

#include "program.hh"
#include "image.hh"
#include "image_io.hh"
#include "util.hh"
#include "matrix4.hh"
#include "cube.hh"
#include "camera.hh"


#include <GL/freeglut_std.h>
#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>
#include <math.h>
#include <vector>

//vbo for vertices and texture coords
GLuint program_id;
GLuint cs_program_id;
mygl::Camera *camera;
//vao buffers ids
GLuint VertexArrayID[2];
GLuint vboId[4];

#define TEST_OPENGL_ERROR()                                                             \
  do {									\
    GLenum err = glGetError();					                        \
    if (err != GL_NO_ERROR) std::cerr << "OpenGL ERROR!" << __LINE__ << std::endl;      \
  } while(0)


void init_VAO()
{
    glGenVertexArrays(2, VertexArrayID);TEST_OPENGL_ERROR();
    glBindVertexArray(VertexArrayID[0]);TEST_OPENGL_ERROR();
}

    std::vector<float> ripples = {
            1.5, 1.5,
            1.5, 1.5,
            1.5, 1.5,
            1.5, 1.5,
            1.5, 1.5,
            1.5, 1.5,
        };
//cubemap vertices positions
    std::vector<GLfloat> drops = {
    // positions

    -10.0f, -10.0f, -10.0f,
};

// init vertex buffer;
void init_VBO()
{
    GLint vertex_location = glGetAttribLocation(program_id, "position");TEST_OPENGL_ERROR();
    GLint uv_location = glGetAttribLocation(program_id, "uv");TEST_OPENGL_ERROR();

    // reservation of buffer
    init_VAO();
    glGenBuffers(4, vboId);TEST_OPENGL_ERROR();

    //teture coordonates for triangles
    if (uv_location != -1)
    {

        std::vector<GLfloat> uv = {

            1.0f, 1.0f,
            0, 1.f,
            0, 0,
            0, 0,
            1, 0,
            1.0f, 1,
        };

        //activation of buffer, vbo type
        glBindBuffer(GL_ARRAY_BUFFER, uv_location);TEST_OPENGL_ERROR();

        //allocation
        glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(float), uv.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();

        //how to read buffer
        glVertexAttribPointer(uv_location, 2, GL_FLOAT,GL_FALSE, 0, 0);TEST_OPENGL_ERROR();
        glEnableVertexAttribArray(uv_location);TEST_OPENGL_ERROR();
    }
    if (vertex_location != -1)
    {
        //activation of buffer, vbo type
        glBindBuffer(GL_ARRAY_BUFFER, vboId[1]);TEST_OPENGL_ERROR();

        //allocation
        glBufferData(GL_ARRAY_BUFFER, cube_vertices.size() * sizeof(GLfloat), cube_vertices.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();

        //how to read buffer
        glVertexAttribPointer(vertex_location,3, GL_FLOAT, GL_FALSE, 0, 0);TEST_OPENGL_ERROR();

        glEnableVertexAttribArray(vertex_location);TEST_OPENGL_ERROR();
    }
    //activation of buffer, vbo type
    glBindVertexArray(VertexArrayID[0]);
}

    GLint img_location;
    GLuint img_texture;
    GLint sun_location;
    GLuint sun_texture;
    tifo::rgb24_image *raymarch;
    GLuint texture_id;
    GLint tex_location;
//textures cubemap initiation
void init_textures()
{

   /* std::vector<std::string> sky_faces = {"../textures/skybox_px.tga",
                                        "../textures/skybox_nx.tga",
                                        "../textures/skybox_py.tga",
                                        "../textures/skybox_ny.tga",
                                        "../textures/skybox_pz.tga",
                                        "../textures/skybox_nz.tga"
                                        };*/
    std::vector<std::string> sky_faces = {"../textures/skybox/right.tga",
                                        "../textures/skybox/left.tga",
                                        "../textures/skybox/top.tga",
                                        "../textures/skybox/bottom.tga",
                                        "../textures/skybox/front.tga",
                                        "../textures/skybox/back.tga"};


    glGenTextures(1, &texture_id);TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();

    //activation of texture
    //glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    for (int i = 0; i < 6; i++)
    {
        tifo::rgb24_image *sky = tifo::load_image(sky_faces[i].c_str());
        //generation of texture
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, sky->sx, sky->sy, 0, GL_RGB, GL_UNSIGNED_BYTE, sky->pixels);TEST_OPENGL_ERROR();
    }

   // tifo::rgb24_image *sky = tifo::load_image(sky_faces[0].c_str());
   // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, sky->sx, sky->sy, 0, GL_RGB, GL_UNSIGNED_BYTE, sky->pixels);TEST_OPENGL_ERROR();
    //glGenerateMipmapEXT(GL_TEXTURE_2D);
    glGenerateMipmapEXT(GL_TEXTURE_CUBE_MAP);


    //putting cubemap in uniform vairable
    tex_location = glGetUniformLocation(program_id, "sky_sampler");TEST_OPENGL_ERROR();
    glUniform1i(tex_location, 0);TEST_OPENGL_ERROR();
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
    
    raymarch = tifo::load_image(sky_faces[0].c_str());

    glGenTextures(1, &img_texture);TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE1);TEST_OPENGL_ERROR();

    //activation of texture
    //glBindImageTexture(0, img_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA);TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, img_texture);TEST_OPENGL_ERROR();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 900, 900, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);TEST_OPENGL_ERROR();
    
    img_location = glGetUniformLocation(program_id, "raymarch");TEST_OPENGL_ERROR();
    glUniform1i(img_location, 1);TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    std::string sun_file = "../textures/tint2.tga";
    auto sun_color = tifo::load_image(sun_file.c_str());

    glGenTextures(1, &sun_texture);TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE2);TEST_OPENGL_ERROR();

    //activation of texture
    //glBindImageTexture(0, img_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA);TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, sun_texture);TEST_OPENGL_ERROR();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, sun_color->sx, sun_color->sy, 0, GL_RGB, GL_UNSIGNED_BYTE, sun_color->pixels);TEST_OPENGL_ERROR();
    
    sun_location = glGetUniformLocation(program_id, "sun_texture");TEST_OPENGL_ERROR();
    glUniform1i(sun_location, 2);TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


   // glGenerateMipmap(GL_TEXTURE_2D);TEST_OPENGL_ERROR();

    //putting cubemap in uniform vairable


}

void init_GL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0, 0.1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClearColor(0, 0, 0, 0);
}

void idle()
{
    GLint time_location = glGetUniformLocation(program_id, "time");
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.;
    glUniform1f(time_location, time);

    // Our ModelViewProjection : multiplication of our 3 matrices
    //  mvp = glm::rotate(mvp, -
    camera->m_view = glm::lookAt(
            camera->m_camera_position,//camera
            camera->m_camera_look_at,
            camera->m_up
            ); // Along Y axis

    glm::mat4 mvp = camera->m_projection * camera->m_view * camera->m_identity;
    GLint mvp_location = glGetUniformLocation(program_id, "MVP");
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);

    GLint look_at_location = glGetUniformLocation(program_id, "camera_look_at");
    glUniform3fv(look_at_location, 1, &camera->m_camera_look_at[0]);

    GLint camera_location = glGetUniformLocation(program_id, "camera_position");
    glUniform3fv(camera_location, 1, &camera->m_camera_position[0]);

    GLint mouse_location = glGetUniformLocation(program_id, "mouse");
    glUniform2fv(mouse_location, 1, &camera->m_mouse[0]);
    
    glutPostRedisplay();
}

//loop function
void display()
{

    //compute shader


    glBindImageTexture(0, img_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);TEST_OPENGL_ERROR();
  //  glBindImageTexture(1, texture_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);TEST_OPENGL_ERROR();

    for (int i = 0; i < 5; i++)
        glBindImageTexture(1, texture_id, i, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);TEST_OPENGL_ERROR();
    glBindImageTexture(2, sun_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);TEST_OPENGL_ERROR();

    glUseProgram(cs_program_id);TEST_OPENGL_ERROR();

    glDispatchCompute(raymarch->sx / 20, raymarch->sy / 20, 1 );TEST_OPENGL_ERROR();
   // glDispatchCompute(1,1, 1 );TEST_OPENGL_ERROR();
    glUniform1i(glGetUniformLocation(cs_program_id, "raymarch"), 0);
    glUniform1i(glGetUniformLocation(cs_program_id, "sun_texture"), 2);
    glUniform1i(glGetUniformLocation(cs_program_id, "sky_sampler"), 1);
    glUniform3fv(glGetUniformLocation(cs_program_id, "camera_position"), 1, &camera->m_camera_position[0]);
    GLint time_location = glGetUniformLocation(cs_program_id, "time");
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.;
    glUniform1f(time_location, time);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);TEST_OPENGL_ERROR();

    glUseProgram(program_id);TEST_OPENGL_ERROR();
    // clear the color buffer before each drawing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(VertexArrayID[0]);TEST_OPENGL_ERROR();

    glDrawArrays(GL_TRIANGLES, 0, 6);TEST_OPENGL_ERROR();

    // swap the buffers and hence show the buffers
    // content to the screen
    glutSwapBuffers();

}

void KeyboardCB(unsigned char key, int mouse_x, int mouse_y)
{
    camera->enterkeyboard(key, mouse_x, mouse_y);
}

void KeyboardCB(int key, int mouse_x, int mouse_y)
{
    camera->enterkeyboard(key, mouse_x, mouse_y);
}

void MouseCB(int mouse_x, int mouse_y)
{
    camera->entermouse(mouse_x, mouse_y);
}

void init_glut(int &argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitContextVersion(4, 6);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(252, 252);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Test OpenGL - POGL");
    glutDisplayFunc(display);
    glutKeyboardFunc(KeyboardCB);
    glutSpecialFunc(KeyboardCB);
    glutPassiveMotionFunc(MouseCB);
    glutIdleFunc(idle);
}

//glew is opengl extension wrangler
bool init_glew() {
  if (glewInit()) {
    std::cerr << " Error while initializing glew";
    return false;
  }
  return true;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "missing vertexShader and/or fragmentShader path" << std::endl;
        exit(EXIT_FAILURE);
    }

    camera = new mygl::Camera();
    init_glut(argc, argv);

    if (!init_glew())
    {
        std::cerr << "failing init glew" << std::endl;
        exit(EXIT_FAILURE);
    }

    init_GL();

    //loading shaders file
    auto vertex = load_file(argv[1]);
    auto fragment = load_file(argv[2]);
    auto compute = load_file(argv[3]);

    //main program
    auto prog = new mygl::Program();
    prog->insert_vertex(vertex);
    prog->insert_fragment(fragment);
    prog->make_program();

    //compute program
    auto prog_compute = new mygl::Program();
    prog_compute->insert_compute(compute);
    prog_compute->make_program();

    program_id = prog->my_program;
    cs_program_id = prog_compute->my_program;

    if (!prog->is_ready())
    {
        std::cerr << "main prog is not ready" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (!prog_compute->is_ready())
    {
        std::cerr << "prog compute is not ready" << std::endl;
        exit(EXIT_FAILURE);
    }

    prog_compute->use();
    prog->use();

    init_VBO();
    init_textures();

    glutMainLoop();
    delete prog;
    delete prog_compute;
    return 0;
}
