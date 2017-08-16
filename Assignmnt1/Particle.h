#pragma once
#ifndef PARTICLE
#define PARTICLE

#include <d3d9.h>
#include <d3dx9.h>

struct Attribute {
    D3DXVECTOR3 _position;
    D3DXVECTOR3 _velocity;
    D3DXVECTOR3 _acceleration;
    float       _lifeTime;
    float       _age;
    //float       _size;
    D3DXCOLOR   _color;
    D3DXCOLOR   _colorFade;
    bool        _isAlive;
};

struct Particle {
    D3DXVECTOR3 position;
    D3DCOLOR color;
    //float size;
    static const DWORD FVF;
};

#endif