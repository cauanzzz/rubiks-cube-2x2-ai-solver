#ifndef RENDER_HPP
#define RENDER_HPP

#include "raylib.h"
#include "cubo.hpp"

inline Color obterCorRaylib(int idCor) 
{
    // 0: TOPO (Branco), 1: BASE (Amarelo), 2: ESQUERDA (Laranja), 3: FRENTE (Verde), 4: DIREITA (Vermelho), 5: TRASEIRA (Azul)
    Color cores[] = { WHITE, YELLOW, ORANGE, GREEN, RED, BLUE };
    return cores[idCor];
};

inline void desenharCubo3D(const cubo& c) 
{
    for (float x : {-0.52f, 0.52f}) 
    {
        for (float y : {-0.52f, 0.52f}) 
        {
            for (float z : {-0.52f, 0.52f}) 
            {
                DrawCube((Vector3){x, y, z}, 1.0f, 1.0f, 1.0f, BLACK);
            }
        }
    }

    // TOPO (y = +1.05)
    DrawCube((Vector3){-0.52f, 1.05f, -0.52f}, 0.9f, 0.05f, 0.9f, obterCorRaylib(c.faces[TOPO][0][0]));
    DrawCube((Vector3){ 0.52f, 1.05f, -0.52f}, 0.9f, 0.05f, 0.9f, obterCorRaylib(c.faces[TOPO][0][1]));
    DrawCube((Vector3){-0.52f, 1.05f,  0.52f}, 0.9f, 0.05f, 0.9f, obterCorRaylib(c.faces[TOPO][1][0]));
    DrawCube((Vector3){ 0.52f, 1.05f,  0.52f}, 0.9f, 0.05f, 0.9f, obterCorRaylib(c.faces[TOPO][1][1]));

    // BASE (y = -1.05)
    DrawCube((Vector3){-0.52f, -1.05f,  0.52f}, 0.9f, 0.05f, 0.9f, obterCorRaylib(c.faces[BASE][0][0]));
    DrawCube((Vector3){ 0.52f, -1.05f,  0.52f}, 0.9f, 0.05f, 0.9f, obterCorRaylib(c.faces[BASE][0][1]));
    DrawCube((Vector3){-0.52f, -1.05f, -0.52f}, 0.9f, 0.05f, 0.9f, obterCorRaylib(c.faces[BASE][1][0]));
    DrawCube((Vector3){ 0.52f, -1.05f, -0.52f}, 0.9f, 0.05f, 0.9f, obterCorRaylib(c.faces[BASE][1][1]));

    // FRENTE (z = +1.05)
    DrawCube((Vector3){-0.52f,  0.52f, 1.05f}, 0.9f, 0.9f, 0.05f, obterCorRaylib(c.faces[FRENTE][0][0]));
    DrawCube((Vector3){ 0.52f,  0.52f, 1.05f}, 0.9f, 0.9f, 0.05f, obterCorRaylib(c.faces[FRENTE][0][1]));
    DrawCube((Vector3){-0.52f, -0.52f, 1.05f}, 0.9f, 0.9f, 0.05f, obterCorRaylib(c.faces[FRENTE][1][0]));
    DrawCube((Vector3){ 0.52f, -0.52f, 1.05f}, 0.9f, 0.9f, 0.05f, obterCorRaylib(c.faces[FRENTE][1][1]));

    // TRASEIRA (z = -1.05)
    DrawCube((Vector3){ 0.52f,  0.52f, -1.05f}, 0.9f, 0.9f, 0.05f, obterCorRaylib(c.faces[TRASEIRA][0][0]));
    DrawCube((Vector3){-0.52f,  0.52f, -1.05f}, 0.9f, 0.9f, 0.05f, obterCorRaylib(c.faces[TRASEIRA][0][1]));
    DrawCube((Vector3){ 0.52f, -0.52f, -1.05f}, 0.9f, 0.9f, 0.05f, obterCorRaylib(c.faces[TRASEIRA][1][0]));
    DrawCube((Vector3){-0.52f, -0.52f, -1.05f}, 0.9f, 0.9f, 0.05f, obterCorRaylib(c.faces[TRASEIRA][1][1]));

    // ESQUERDA (x = -1.05)
    DrawCube((Vector3){-1.05f,  0.52f, -0.52f}, 0.05f, 0.9f, 0.9f, obterCorRaylib(c.faces[ESQUERDA][0][0]));
    DrawCube((Vector3){-1.05f,  0.52f,  0.52f}, 0.05f, 0.9f, 0.9f, obterCorRaylib(c.faces[ESQUERDA][0][1]));
    DrawCube((Vector3){-1.05f, -0.52f, -0.52f}, 0.05f, 0.9f, 0.9f, obterCorRaylib(c.faces[ESQUERDA][1][0]));
    DrawCube((Vector3){-1.05f, -0.52f,  0.52f}, 0.05f, 0.9f, 0.9f, obterCorRaylib(c.faces[ESQUERDA][1][1]));

    // DIREITA (x = +1.05)
    DrawCube((Vector3){1.05f,  0.52f,  0.52f}, 0.05f, 0.9f, 0.9f, obterCorRaylib(c.faces[DIREITA][0][0]));
    DrawCube((Vector3){1.05f,  0.52f, -0.52f}, 0.05f, 0.9f, 0.9f, obterCorRaylib(c.faces[DIREITA][0][1]));
    DrawCube((Vector3){1.05f, -0.52f,  0.52f}, 0.05f, 0.9f, 0.9f, obterCorRaylib(c.faces[DIREITA][1][0]));
    DrawCube((Vector3){1.05f, -0.52f, -0.52f}, 0.05f, 0.9f, 0.9f, obterCorRaylib(c.faces[DIREITA][1][1]));
}

#endif