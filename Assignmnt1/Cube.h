#pragma once
#ifndef __CUBE
#define __CUBE
#include <d3d9.h>
#include <d3dx9.h>

#define DRVERTEX_FLAGS (D3DFVF_XY)
struct Vertex
{
    Vertex() {}
    Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) :
        _x(x),
        _y(y),
        _z(z),
        _nx(nx),
        _ny(ny),
        _nz(nz),
        _u(u),
        _v(v) {    }
    float _x, _y, _z;
    float _nx, _ny, _nz;
    float _u, _v;
    static const DWORD FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
};

class Cube
{
public:
	Cube();
	~Cube();
    
    // Colors
    const D3DXCOLOR      WHITE = (D3DCOLOR_XRGB(255, 255, 255));
    const D3DXCOLOR      BLACK = (D3DCOLOR_XRGB(0, 0, 0));
    const D3DXCOLOR        RED = (D3DCOLOR_XRGB(255, 0, 0));
    const D3DXCOLOR      GREEN = (D3DCOLOR_XRGB(0, 255, 0));
    const D3DXCOLOR       BLUE = (D3DCOLOR_XRGB(0, 0, 255));
    const D3DXCOLOR     YELLOW = (D3DCOLOR_XRGB(255, 255, 0));
    const D3DXCOLOR       CYAN = (D3DCOLOR_XRGB(0, 255, 255));
    const D3DXCOLOR    MAGENTA = (D3DCOLOR_XRGB(255, 0, 255));

    // Materials
    D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);

    const D3DMATERIAL9 WHITE_MTRL = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
    const D3DMATERIAL9 RED_MTRL = InitMtrl(RED, RED, RED, BLACK, 2.0f);
    const D3DMATERIAL9 GREEN_MTRL = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
    const D3DMATERIAL9 BLUE_MTRL = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
    const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);

    bool Setup(LPDIRECT3DDEVICE9 *ppd);
    bool Display(float timeDelta);
    IDirect3DVertexBuffer9* getVertexBuffer() { return VB; }
    LPDIRECT3DTEXTURE9 getTexture() { return pTexture; }
    D3DMATERIAL9* getMaterial() { return &material; }
    Vertex *getFaceVertices() { return faceVertices; }
private:
    //parent device
    LPDIRECT3DDEVICE9 *ppDevice = 0; //graphics device
    IDirect3DVertexBuffer9* VB;
    LPDIRECT3DTEXTURE9 pTexture = NULL;   //Textures for our mesh
    D3DMATERIAL9 material;
    
    //face details
    Vertex *faceVertices;

    int width;
    int height;
};
#endif