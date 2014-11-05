# Instancing Playground

I am trying to find out how to render the same objects multiple times effeciently in OpenGL. My original (performace) problem was WebGL related.
As there is quite some stuff in between browser and OpenGL i wanted to do it very low level so i am not debugging any specifiy WebGL implementation.

WebGL supports ANGLE instancing which should be mapped to ARB instancing. So that's what i am using for instancing.

Structures are randomly generated and so the output is bollocks.

I am neither a C guy (it's been years i did something in in C) nor did i ever do something in OpenGL. A real C/OpenGL programmer would probably say OMG, WTF.

However it works for me and maybe somebody might find the code useful as i didn't find any copy paste example for my problem :D
