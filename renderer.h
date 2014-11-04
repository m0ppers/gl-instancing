#ifndef RENDERER_H
#define RENDERER_H
typedef struct {
    const char *vshader;
    const char *fshader;
    void (*finish)();
    void (*prepare)();
    void (*draw)();
} renderer;
#endif
