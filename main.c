#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

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


    const char* vshader = "attribute vec3 offset;\nattribute vec3 vertexPosition_modelspace;\nvoid main() {\ngl_Position = vec4(offset + vertexPosition_modelspace, 1.0);\n}";
    const char* fshader = "void main() {\ngl_FragColor = vec4(1.0, 0.0, 0.0, 1);\n}";

    GLuint programID = createGLProgram(vshader, fshader);
    if (!programID) {
        return 2;
    }
    
    
    // Get a handle for our buffers
    GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    
    int i, j;
    srand(1234);
    GLuint vbuffer;

    GLfloat *buffer = malloc(NUM_OBJECTS * NUM_VERTICES * 3 * sizeof(GLfloat));
   
    for (i=0;i<NUM_VERTICES * 3;i++) {
        if (i % 2 != 2) {
            buffer[i] = (rand() % 2000 - 1000) / 10000.0f;
        } else {
            buffer[i] = 0.0f;
        }
    }
    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, NUM_VERTICES * 3 * sizeof(GLfloat), buffer, GL_STATIC_DRAW);

    GLuint offsetLocation = glGetAttribLocation(programID, "offset");
    GLuint obuffer;
    GLfloat *offset = malloc(3 * sizeof(float) * NUM_OBJECTS);
    
    offset[0] = 0;
    offset[1] = 0;
    offset[2] = 0;
    
    for (i=1;i<NUM_OBJECTS;i++) {
        offset[i * 3 + 0] = (rand() % 2000 - 1000) / 1000.0f;
        offset[i * 3 + 1] = (rand() % 2000 - 1000) / 1000.0f;
    }
    
    glGenBuffers(1, &obuffer);
    glBindBuffer(GL_ARRAY_BUFFER, obuffer);
    glBufferData(GL_ARRAY_BUFFER, 3 * NUM_OBJECTS * sizeof(GLfloat), offset, GL_STATIC_DRAW);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        printFps();
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );

        // Use our shader
        glUseProgram(programID);
        glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(vertexPosition_modelspaceID);
        glVertexAttribPointer(
                vertexPosition_modelspaceID, // The attribute we want to configure
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
                );
        
        glBindBuffer(GL_ARRAY_BUFFER, obuffer);
        glEnableVertexAttribArray(offsetLocation);
        glVertexAttribDivisorARB(offsetLocation, 1);
        glVertexAttribPointer(
                offsetLocation, // The attribute we want to configure
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
                );

        
        glDrawArraysInstancedARB(GL_LINE_LOOP, 0, NUM_VERTICES, NUM_OBJECTS);

        glDisableVertexAttribArray(vertexPosition_modelspaceID);
        glDisableVertexAttribArray(offsetLocation);
        /* Render here */

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    free(offset);
    free(buffer);

    glfwTerminate();
    return 0;
}
