#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "instanced_renderer.h"
#include "standard_renderer.h"
#include "renderer.h"

#define NUM_OBJECTS 20
#define NUM_VERTICES 200

void printFps() {
    static int nbFrames = 0;
    // Measure speed
    static double lastTime;
    double currentTime = glfwGetTime();

    nbFrames++;
    if (!lastTime) {
        lastTime = currentTime;
        return;    
    }
    if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
        // printf and reset timer
        printf("%d fps\n", nbFrames);
        nbFrames = 0;
        lastTime += 1.0;
    }
}

GLFWwindow* initGL() {
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return NULL;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Instancing test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return NULL;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glClearColor(0.0f, 0.0f, 0.3f, 0.0f);

    printf("OpenGL Version supported: %s\n", glGetString(GL_VERSION));
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("GL_EXTENSIONS: %s\n", glGetString(GL_EXTENSIONS));

    return window;
}

GLuint createShader(GLenum type, const GLchar * const *source) {
    GLuint shader;
    GLint result = GL_FALSE;
    int len;

    shader = glCreateShader(type);
    glShaderSource (shader, 1, source, NULL);
    glCompileShader (shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    
    if (result == 0) {
        printf("SHADER FAIL RESULT %d, LEN %d\n", result, len);

        if (len > 0) {
            char errmsg[len+1];
            glGetShaderInfoLog(shader, len, NULL, errmsg);
            printf("%s\n", errmsg);
        }
        return 0;
    }
    return shader;
}

GLuint createGLProgram(const char* vshader, const char* fshader) {
    GLuint programID;
    GLuint vs;
    GLuint fs;
    
    vs = createShader(GL_VERTEX_SHADER, &vshader);
    if (!vs) {
        return 0;
    }

    fs = createShader(GL_FRAGMENT_SHADER, &fshader);
    if (!fs) {
        return 0;
    }

    programID = glCreateProgram ();
    glAttachShader (programID, fs);
    glAttachShader (programID, vs);
    glLinkProgram (programID);

    return programID;
}

int main(void)
{
    GLFWwindow* window = initGL();

    if (!window) {
        return 1;
    }
    srand(1234);

    //renderer* renderer = getInstancedRenderer(NUM_VERTICES, NUM_OBJECTS);
    renderer* renderer = getStandardRenderer(NUM_VERTICES, NUM_OBJECTS);

    GLuint programID = createGLProgram(renderer->vshader, renderer->fshader);
    if (!programID) {
        return 2;
    }
    
    renderer->prepare(programID); 

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        printFps();
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );

        // Use our shader
        glUseProgram(programID);
    
        renderer->draw();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    renderer->finish();

    glfwTerminate();
    return 0;
}
