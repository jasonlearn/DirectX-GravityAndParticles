#include "Cube.h"

Cube::Cube() {}

Cube::~Cube()
{
	if (VB) { VB->Release(); VB = 0; }
}


bool Cube::Setup(LPDIRECT3DDEVICE9 *ppd) {
    ppDevice = ppd;
    faceVertices = new Vertex[6];

    D3DVIEWPORT9 vp;
    (*ppDevice)->GetViewport(&vp);
    width = vp.Width;
    height = vp.Height;

    // create vertex and index buffers
    (*ppDevice)->CreateVertexBuffer(
        36 * sizeof(Vertex),
        //4 * sizeof(Vertex),
        D3DUSAGE_WRITEONLY,
        Vertex::FVF,
        D3DPOOL_MANAGED,
        &VB,
        0);

    // fill the buffers with the cube data
    Vertex* vertices;
    VB->Lock(0, 0, (void**)&vertices, 0);

    // fill in the front face vertex data
    faceVertices[0] = Vertex(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);

    vertices[0] = Vertex(-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    vertices[1] = Vertex(-1.0f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    vertices[2] = Vertex(3.0f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

    vertices[3] = Vertex(-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    vertices[4] = Vertex(3.0f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
    vertices[5] = Vertex(3.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

    /*
    vertices[0] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    vertices[1] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    vertices[2] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

    vertices[3] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    vertices[4] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
    vertices[5] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
    */
    // fill in the back face vertex data

    faceVertices[1] = Vertex(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    
    vertices[6] = Vertex(-1.0f, 0.0f, 3.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    vertices[7] = Vertex(3.0f, 0.0f, 3.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    vertices[8] = Vertex(3.0f, 6.0f, 3.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

    vertices[9] = Vertex(-1.0f, 0.0f, 3.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    vertices[10] = Vertex(3.0f, 6.0f, 3.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    vertices[11] = Vertex(-1.0f, 6.0f, 3.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
    


    // fill in the top face vertex data

    faceVertices[2] = Vertex(0.0f, 6.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);

    vertices[12] = Vertex(-1.0f, 6.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    vertices[13] = Vertex(-1.0f, 6.0f, 3.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    vertices[14] = Vertex(3.0f, 6.0f, 3.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

    vertices[15] = Vertex(-1.0f, 6.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    vertices[16] = Vertex(3.0f, 6.0f, 3.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
    vertices[17] = Vertex(3.0f, 6.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

    // fill in the bottom face vertex data

    faceVertices[3] = Vertex(0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);

    vertices[18] = Vertex(-1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
    vertices[19] = Vertex(3.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
    vertices[20] = Vertex(3.0f, 0.0f, 3.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);

    vertices[21] = Vertex(-1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
    vertices[22] = Vertex(3.0f, 0.0f, 3.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
    vertices[23] = Vertex(-1.0f, 0.0f, 3.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

    // fill in the left face vertex data

    faceVertices[4] = Vertex(-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    vertices[24] = Vertex(-1.0f, 0.0f, 3.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    vertices[25] = Vertex(-1.0f, 6.0f, 3.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    vertices[26] = Vertex(-1.0f, 6.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    vertices[27] = Vertex(-1.0f, 0.0f, 3.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    vertices[28] = Vertex(-1.0f, 6.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    vertices[29] = Vertex(-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // fill in the right face vertex data

    faceVertices[5] = Vertex(3.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    vertices[30] = Vertex(3.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    vertices[31] = Vertex(3.0f, 6.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    vertices[32] = Vertex(3.0f, 6.0f, 3.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    vertices[33] = Vertex(3.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    vertices[34] = Vertex(3.0f, 6.0f, 3.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    vertices[35] = Vertex(3.0f, 0.0f, 3.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    

    VB->Unlock();

    //load in texture for the cube
    if (FAILED(D3DXCreateTextureFromFile(*ppDevice, TEXT("mirror.bmp"), &pTexture))) {
        OutputDebugString(TEXT("Failed to load mirror"));
        return false;
    }

    //setup material
    material = WHITE_MTRL;

    return true;
}



bool Cube::Display(float timeDelta) {
    if (*ppDevice)
    {
        (*ppDevice)->SetStreamSource(0, VB, 0, sizeof(Vertex));
        (*ppDevice)->SetFVF(Vertex::FVF);
        (*ppDevice)->SetMaterial(&material);
        (*ppDevice)->SetTexture(0, pTexture);
        (*ppDevice)->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);
    }
    return true;
}


D3DMATERIAL9 Cube::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
    D3DMATERIAL9 mtrl;
    mtrl.Ambient = a;
    mtrl.Diffuse = d;
    mtrl.Specular = s;
    mtrl.Emissive = e;
    mtrl.Power = p;
    return mtrl;
}
