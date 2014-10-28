#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NUM_TRIANGLES 20000

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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

    const char* vshader = "attribute vec3 vertexPosition_modelspace;\nvoid main() {\ngl_Position = vec4(vertexPosition_modelspace, 1.0);\n}";
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
    
    int i;
    float x, y;
    srand(time(NULL));
    GLuint vbuffer;

    GLfloat buffer[NUM_TRIANGLES * 9];
    for (i=0;i<NUM_TRIANGLES;i++) {
        x = (rand() % 2000 - 1000) / 1000.0f; 
        y = (rand() % 2000 - 1000) / 1000.0f;

        printf("RAND x: %f, y: %f\n", x, y);
        buffer[i*9+0] = x-0.01f;
        buffer[i*9+1] = y-0.01f;
        buffer[i*9+2] = 0.0f;
        buffer[i*9+3] = x+0.01f;
        buffer[i*9+4] = y-0.01f;
        buffer[i*9+5] = 0.0f;
        buffer[i*9+6] = x;
        buffer[i*9+7] = y+0.01f;
        buffer[i*9+8] = 0.0f;

    }
    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);
    
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
        
        for (i=0;i<NUM_TRIANGLES;i++) {

            // Draw the triangle !
            glDrawArrays(GL_TRIANGLES, i*3, 3);
        }

        glDisableVertexAttribArray(vertexPosition_modelspaceID);
        /* Render here */

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
