#include "instanced_renderer.h"
#include <stdlib.h>
#include <stdio.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>

static renderer* instancedRenderer = NULL;
static GLfloat *buffer = NULL;
static GLuint vbuffer;
static GLuint vertexPosition_modelspaceID;
static GLuint offsetLocation;
static GLuint obuffer;
static GLfloat *offset = NULL;


void finish() {
    if (offset) {
        free(offset);
    }
    if (buffer) {
        free(buffer);
    }
    if (instancedRenderer) {
        free(instancedRenderer);
    }
}

void prepare(GLuint programID) {
    // Get a handle for our buffers
    vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");

    int i, j;

    buffer = malloc(instancedRenderer->numObjects * instancedRenderer->numVertices * 3 * sizeof(GLfloat));

    for (i=0;i<instancedRenderer->numVertices * 3;i++) {
        if (i % 2 != 2) {
            buffer[i] = (rand() % 2000 - 1000) / 10000.0f;
        } else {
            buffer[i] = 0.0f;
        }
    }
    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, instancedRenderer->numVertices * 3 * sizeof(GLfloat), buffer, GL_STATIC_DRAW);

    offsetLocation = glGetAttribLocation(programID, "offset");
    offset = malloc(3 * sizeof(float) * instancedRenderer->numObjects);

    offset[0] = 0;
    offset[1] = 0;
    offset[2] = 0;

    for (i=1;i<instancedRenderer->numObjects;i++) {
        offset[i * 3 + 0] = (rand() % 2000 - 1000) / 1000.0f;
        offset[i * 3 + 1] = (rand() % 2000 - 1000) / 1000.0f;
    }

    glGenBuffers(1, &obuffer);
    glBindBuffer(GL_ARRAY_BUFFER, obuffer);
    glBufferData(GL_ARRAY_BUFFER, 3 * instancedRenderer->numObjects * sizeof(GLfloat), offset, GL_STATIC_DRAW);
}

void draw() {
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

    glDrawArraysInstancedARB(GL_LINE_LOOP, 0, instancedRenderer->numVertices, instancedRenderer->numObjects);

    glDisableVertexAttribArray(vertexPosition_modelspaceID);
    glDisableVertexAttribArray(offsetLocation);
    /* Render here */
}

renderer* getInstancedRenderer(int numVertices, int numObjects) {
    if (instancedRenderer) {
        return instancedRenderer;
    }
    printf("Working with instanced renderer!\n");
    instancedRenderer = malloc(sizeof(renderer));
    instancedRenderer->numVertices = numVertices;
    instancedRenderer->numObjects = numObjects;
    instancedRenderer->vshader = "attribute vec3 offset;\nattribute vec3 vertexPosition_modelspace;\nvoid main() {\ngl_Position = vec4(offset + vertexPosition_modelspace, 1.0);\n}";
    instancedRenderer->fshader = "void main() {\ngl_FragColor = vec4(1.0, 0.0, 0.0, 1);\n}";
    instancedRenderer->prepare = prepare;
    instancedRenderer->finish = finish;
    instancedRenderer->draw = draw;

    return instancedRenderer;
}
