#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NUM_OBJECTS 20000
#define NUM_VERTICES 200

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Instancing test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glClearColor(0.0f, 0.0f, 0.3f, 0.0f);

    printf("OpenGL Version supported: %s\n", glGetString(GL_VERSION));
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("GL_EXTENSIONS: %s\n", glGetString(GL_EXTENSIONS));

    const char* vshader = "attribute vec3 offset;\nattribute vec3 vertexPosition_modelspace;\nvoid main() {\ngl_Position = vec4(offset + vertexPosition_modelspace, 1.0);\n}";
    const char* fshader = "void main() {\ngl_FragColor = vec4(1.0, 0.0, 0.0, 1);\n}";
    
    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs, 1, &vshader, NULL);
    glCompileShader (vs);

    GLint result = GL_FALSE;
    int len;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &len);

    printf("VS RESULT %d, LEN %d\n", result, len);

    if (len > 0) {
        char errmsg[len+1];
        glGetShaderInfoLog(vs, len, NULL, errmsg);
        printf("%s\n", errmsg);
    }


    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &fshader, NULL);
    glCompileShader (fs);

    GLuint programID = glCreateProgram ();
    glAttachShader (programID, fs);
    glAttachShader (programID, vs);
    glLinkProgram (programID);
    
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


    double lastTime = glfwGetTime();
    int nbFrames = 0;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            printf("%d fps\n", nbFrames);
            nbFrames = 0;
            lastTime += 1.0;
        }
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
