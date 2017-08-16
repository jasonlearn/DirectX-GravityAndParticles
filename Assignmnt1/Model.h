#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <atlbase.h>
#include <string>
#include <sstream>
#include "Bounds.h"

class Model {
public:
    Model();
    ~Model();

    //static model information
    static DWORD numModels;
    static DWORD getNumModels() { return numModels; }

    //Model Position
    float x;
    float y;
    float z;
    float rotationX;
    float rotationY;
    float rotationZ;
    Sphere BoundingSphere;
    char* name;

    //functions
    void draw();
    void release();
    void setDevice(LPDIRECT3DDEVICE9*);
    void setPosition(float, float, float);
    void setRotation(float, float, float);
    void updateCenter();
    bool calculateBoundingSphere();
    D3DXMATRIXA16 getTransformationMatrix();
    HRESULT loadModel(TCHAR*, char*);

    //getters
    LPD3DXMESH getMesh() { return pMesh; }
    D3DMATERIAL9 *getMaterials() { return pMeshMaterials; }
    LPDIRECT3DTEXTURE9 *getTextures() { return pMeshTextures; }
    DWORD getNumMaterials() { return dwNumMaterials; }
private:
    //parent device
    LPDIRECT3DDEVICE9 *ppDevice = 0; //graphics device

    //Model Data
    LPD3DXMESH pMesh = NULL;    //Our mesh object in system
    D3DMATERIAL9 *pMeshMaterials = NULL;    //Materials for our mesh
    LPDIRECT3DTEXTURE9 *pMeshTextures = NULL;   //Textures for our mesh
    DWORD dwNumMaterials = 0L;  //Number of mesh materials
};