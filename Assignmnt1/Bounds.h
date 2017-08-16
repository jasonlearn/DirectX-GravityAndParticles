#pragma once
#ifndef BOUNDS
#define BOUNDS
#include <d3d9.h>
#include <d3dx9.h>

struct Sphere {
    Sphere();

    D3DXVECTOR3 _center;
    float _radius;
};

struct BoundingBox {
    BoundingBox();

    bool isPointInside(D3DXVECTOR3& p);

    D3DXVECTOR3 _min;
    D3DXVECTOR3 _max;
};



#endif