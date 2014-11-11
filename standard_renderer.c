#include "standard_renderer.h"
#include <stdlib.h>
#include <stdio.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>

static renderer* standardRenderer = NULL;
static GLfloat *buffer = NULL;
static GLuint vbuffer;
static GLuint vertexPosition_modelspaceID;
static GLuint VertexArrayID;

void prepareStandard(GLuint programID) {
    int i, j;
    srand(1234);

    buffer = malloc(standardRenderer->numObjects * standardRenderer->numVertices * 3 * sizeof(GLfloat));

    for (i=0;i<standardRenderer->numVertices * 3;i++) {
        if (i % 2 != 2) {
            buffer[i] = (rand() % 2000 - 1000) / 10000.0f;
        } else {
            buffer[i] = 0.0f;
        }
    }
    int dest_index;
    float xoffset, yoffset;
    for (i=1;i<standardRenderer->numObjects;i++) {
        xoffset = (rand() % 2000 - 1000) / 1000.0f;
        yoffset = (rand() % 2000 - 1000) / 1000.0f;
        for (j=0;j<standardRenderer->numVertices*3;j++) {
            dest_index = i * standardRenderer->numVertices * 3 + j;
            buffer[dest_index] = buffer[j];
            if (j % 3 == 0) {
                buffer[dest_index] += xoffset;
            } else if (j % 3 == 1) {
                buffer[dest_index] += yoffset;
            }
        }
    }
    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, standardRenderer->numObjects * standardRenderer->numVertices * 3 * sizeof(GLfloat), buffer, GL_STATIC_DRAW);
}

void drawStandard() {
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
            0,
            3,
            GL_FLOAT, 
            GL_FALSE, 
            0,       
            (void*)0  
            );

    for (int i=0;i<standardRenderer->numObjects;i++) {
        glDrawArrays(GL_TRIANGLES, i*standardRenderer->numVertices, standardRenderer->numVertices);
    }

    glDisableVertexAttribArray(0);
}

void finishStandard() {
    if (buffer) {
        free(buffer);
    }
	glDeleteBuffers(1, &vbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
    if (standardRenderer) {
        free(standardRenderer);
    }
}

renderer* getStandardRenderer(int numVertices, int numObjects) {
    if (standardRenderer) {
        return standardRenderer;
    }
    printf("Working with standard renderer!\n");
    standardRenderer = malloc(sizeof(renderer));
    standardRenderer->numVertices = numVertices;
    standardRenderer->numObjects = numObjects;
    standardRenderer->vshader = "#version 330\nlayout(location = 0) in vec3 vertexPosition_modelspace;\nvoid main() {\ngl_Position.xyz = vertexPosition_modelspace;\ngl_Position.w = 1.0;\n}";
    standardRenderer->fshader = "#version 330\nout vec3 color;\nvoid main() {\ncolor = vec3(1.0, 0.0, 0.0);\n}";
    standardRenderer->prepare = prepareStandard;
    standardRenderer->finish = finishStandard;
    standardRenderer->draw = drawStandard;

    return standardRenderer;
}
