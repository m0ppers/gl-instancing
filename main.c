#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "instanced_renderer.h"
#include "standard_renderer.h"
#include "renderer.h"
#include "argtable2.h"

#define RENDERER_TYPE_STANDARD 1
#define RENDERER_TYPE_INSTANCED 2

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
    
    // Use OpenGL Core v3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Instancing test", NULL, NULL);
    if (!window)
    {
        printf("Couldn't create window\n");
        glfwTerminate();
        return NULL;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
	glewExperimental = 1; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return NULL;
	}

    printf("OpenGL Version supported: %s\n", glGetString(GL_VERSION));
    printf("GL Vendor: %s\n", glGetString(GL_VENDOR));
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("GL_EXTENSIONS: %s\n", glGetString(GL_EXTENSIONS));

    glClearColor(0.0f, 0.0f, 0.3f, 0.0f);

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

int glMain(renderer* renderer)
{
    GLFWwindow* window = initGL();

    if (!window) {
        return 1;
    }
    srand(1234);

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

void printUsage(FILE* stream, const char* progname, void** argtable) {
    fprintf(stream, "Usage: %s", progname);
    arg_print_syntax(stream, argtable, "\n");
    arg_print_glossary(stream, argtable, "  %-10s %s\n");
}

int main(int argc, char** argv) {
    struct arg_int  *numVerticesArg = arg_int0("v","vertices",NULL, "num vertices (default is 20)");
    struct arg_int  *numObjectsArg  = arg_int0("o","objects",NULL, "num objects (default is 20)");
    struct arg_lit  *help           = arg_lit0(NULL,"help",       "print this help and exit");
    struct arg_str  *rendererArg    = arg_str1(NULL,NULL,"RENDERER",NULL);
    struct arg_end  *end            = arg_end(20);

    void* argtable[] = {numVerticesArg, numObjectsArg, help, rendererArg, end};
    const char* progname = "gl-instancing";
    int exitcode=0;
    int nerrors;
    renderer *renderer;
    int numVertices = 20;
    int numObjects = 20;

    /* verify the argtable[] entries were allocated sucessfully */
    if (arg_nullcheck(argtable) != 0)
    {
        /* NULL entries were detected, some allocations must have failed */
        printf("%s: insufficient memory\n",progname);
        exitcode=1;
        goto exit;
    }

    /* Parse the command line as defined by argtable[] */
    nerrors = arg_parse(argc,argv,argtable);

    /* special case: '--help' takes precedence over error reporting */
    if (help->count > 0)
    {
        printUsage(stderr, progname, argtable);
        goto exit;
    }

    if (numVerticesArg->count > 0) {
        numVertices = numVerticesArg->ival[0];
    }
    
    if (numObjectsArg->count > 0) {
        numObjects = numObjectsArg->ival[0];
    }
    
    if (rendererArg->count == 0) {
        fprintf(stderr, "Provide a renderer!\n");
        printUsage(stderr, progname, argtable);
        goto exit;
    }

    if (rendererArg->count > 0) {
        if (strcmp(rendererArg->sval[0], "standard") == 0) {
            renderer = getStandardRenderer(numVertices, numObjects);
        } else if (strcmp(rendererArg->sval[0], "instanced") == 0) {
            renderer = getInstancedRenderer(numVertices, numObjects);
        } else {
            fprintf(stderr, "Renderer %s is unknown. Supported renderers: instanced, standard\n", rendererArg->sval[0]);
            printUsage(stderr, progname, argtable);
            goto exit;
        }
    }

    exitcode = glMain(renderer);

exit:
    /* deallocate each non-null entry in argtable[] */
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

    return exitcode;
}
