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
            0, 1.5,
            0, 0,
            1, 0,
            0, 1.5,
            1, 1.5,
            1, 0,
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
        glBufferData(GL_ARRAY_BUFFER, cube_vertices.size() * sizeof(GLfloat), cube_vertices.data(), GL_DYNAMIC_COPY);TEST_OPENGL_ERROR();

        //how to read buffer
        glVertexAttribPointer(vertex_location,3, GL_FLOAT, GL_FALSE, 0, 0);TEST_OPENGL_ERROR();

        glEnableVertexAttribArray(vertex_location);TEST_OPENGL_ERROR();
    }
    //activation of buffer, vbo type
    glBindVertexArray(VertexArrayID[0]);
}

//textures cubemap initiation
void init_textures()
{

    std::vector<std::string> sky_faces = {"../textures/skybox_px.tga",
                                        "../textures/skybox_nx.tga",
                                        "../textures/skybox_py.tga",
                                        "../textures/skybox_ny.tga",
                                        "../textures/skybox_pz.tga",
                                        "../textures/skybox_nz.tga"
                                        };
    GLuint texture_id;
    GLint tex_location;

    glGenTextures(1, &texture_id);TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();

    //activation of texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    for (int i = 0; i < 6; i++)
    {
        tifo::rgb24_image *sky = tifo::load_image(sky_faces[i].c_str());
        //generation of texture
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, sky->sx, sky->sy, 0, GL_RGB, GL_UNSIGNED_BYTE, sky->pixels);TEST_OPENGL_ERROR();
    }


    //putting cubemap in uniform vairable
    tex_location = glGetUniformLocation(program_id, "sky_sampler");TEST_OPENGL_ERROR();
    glUniform1i(tex_location, 0);TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
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
    glm::mat4 mvp = camera->m_projection * camera->m_view * camera->m_identity;

    GLint mvp_location = glGetUniformLocation(program_id, "MVP");
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);

    GLint look_at_location = glGetUniformLocation(program_id, "camera_look_at");
    glUniform3fv(look_at_location, 1, &camera->m_camera_look_at[0]);

    GLint camera_location = glGetUniformLocation(program_id, "camera_position");
    glUniform3fv(camera_location, 1, &camera->m_camera_position[0]);

    glutPostRedisplay();
}

//loop function
void display()
{

    //compute shader
    glUseProgram(cs_program_id);TEST_OPENGL_ERROR();
    /*glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vboId[2]);

    glBufferData(GL_SHADER_STORAGE_BUFFER, ripples.size() * sizeof(GLfloat), ripples.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, vboId[3]);

    glBufferData(GL_SHADER_STORAGE_BUFFER, drops.size() * sizeof(GLfloat), drops.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();*/
    glDispatchCompute(8, 1, 1);TEST_OPENGL_ERROR();
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glUseProgram(program_id);TEST_OPENGL_ERROR();
    // clear the color buffer before each drawing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(VertexArrayID[0]);TEST_OPENGL_ERROR();

    glDrawArrays(GL_TRIANGLES, 0, 36);TEST_OPENGL_ERROR();

    glBindVertexArray(0);

    // swap the buffers and hence show the buffers
    // content to the screen
    glutSwapBuffers();

}

void KeyboardCB(unsigned char key, int mouse_x, int mouse_y)
{
    camera->enterkeyboard(key);
}

void KeyboardCB(int key, int mouse_x, int mouse_y)
{
    camera->enterkeyboard(key);
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
