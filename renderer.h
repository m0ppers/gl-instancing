#ifndef RENDERER_H
#define RENDERER_H
typedef struct {
    const char *vshader;
    const char *fshader;
    int numVertices;
    int numObjects;
    void (*finish)();
    void (*prepare)();
    void (*draw)();
} renderer;
#endif
