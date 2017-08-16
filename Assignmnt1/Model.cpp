#include "Model.h"

//initialize static variable
DWORD Model::numModels = 0;

//initializes all parameters to 0
Model::Model() :
        x(0),
        y(0),
        z(0),
        rotationX(0),
        rotationY(0),
        rotationZ(0),
        BoundingSphere() {
    ++numModels;
}

//destructor for models
Model::~Model() {
    --numModels;
}

/*
* draws the models to the screen using the pointer to the pDevice
*/
void Model::draw() {
    //meshes are divided into subsets, one for each material.  Render them in a loop
    for (DWORD i = 0; i < dwNumMaterials; i++) {
        //set the material and texture for this subset
        (*ppDevice)->SetMaterial(&pMeshMaterials[i]);
        (*ppDevice)->SetTexture(0, pMeshTextures[i]);

        //draw the mesh subset
        pMesh->DrawSubset(i);
    }
}

/*
* releases memory for the data used
*/
void Model::release() {
    if (pMeshMaterials)
        delete[] pMeshMaterials;

    if (pMeshTextures) {
        for (DWORD i = 0; i < dwNumMaterials; i++) {
            if (pMeshTextures[i])
                pMeshTextures[i]->Release();
        }
        delete[] pMeshTextures;
    }

    if (pMesh)
        pMesh->Release();
}

/*
* Sets the pointer to the device context
* params:
*   ppDev - The pointer to the device context    
*/
void Model::setDevice(LPDIRECT3DDEVICE9* ppDev) {
    ppDevice = ppDev;
}

/*
* sets the x, y, and z coordinates for the model
* params:
*   nX - the x position
*   nY - the y position
*   nZ - the z position
*/
void Model::setPosition(float nX, float nY, float nZ) {
    x = nX;
    y = nY;
    z = nZ;
}

/*
* sets the rotation of the model
* params:
*   rX - the x rotation
*   rY - the y rotation
*   rZ - the z rotation
*/
void Model::setRotation(float rX, float rY, float rZ) {
    rotationX = rX;
    rotationY = rY;
    rotationZ = rZ;
}

void Model::updateCenter() {
    BoundingSphere._center = D3DXVECTOR3(x, y, z);
}

bool Model::calculateBoundingSphere() {
    HRESULT hr = 0;
    BYTE* v = 0;
    pMesh->LockVertexBuffer(0, (void**)&v);
    hr = D3DXComputeBoundingSphere(
        (D3DXVECTOR3*)v,
        pMesh->GetNumVertices(),
        D3DXGetFVFVertexSize(pMesh->GetFVF()),
        &(BoundingSphere._center),
        &(BoundingSphere._radius));
    pMesh->UnlockVertexBuffer();

    //apply translations

    if (FAILED(hr))
        return false;
    return true;
}

/*
* calculates the net transformation matrix for the model
* return:
*   the net transformation matrix for the model
*/
D3DXMATRIXA16 Model::getTransformationMatrix() {
    D3DXMATRIXA16 matrix,
        translate,
        rotX,
        rotY,
        rotZ;
    D3DXMatrixTranslation(&translate, x, y, z);
    D3DXMatrixRotationX(&rotX, rotationX);
    D3DXMatrixRotationY(&rotY, rotationY);
    D3DXMatrixRotationZ(&rotZ, rotationZ);

    D3DXMatrixMultiply(&matrix, &rotX, &rotY);
    D3DXMatrixMultiply(&matrix, &matrix, &rotZ);
    D3DXMatrixMultiply(&matrix, &matrix, &translate);

    return matrix;
}

/*
* Loads a model from the specified file name/path
*   params:
*       path - the path name for the .x file
*       n - the name of the model
*   return:
*       if the model was successfully loaded
*/
HRESULT Model::loadModel(TCHAR* path, char* n) {
    LPD3DXBUFFER pD3DXMtrlBuffer; //material buffer

   //Load the mesh from the specified file
    if (FAILED(D3DXLoadMeshFromX(path, D3DXMESH_SYSTEMMEM,
        *ppDevice, NULL,
        &pD3DXMtrlBuffer, NULL, &dwNumMaterials,
        &pMesh)))
    {
        //if texture is not in current folder, try parent folder
        const TCHAR* strPrefix = TEXT("..\\");
        const int lenPrefix = lstrlen(strPrefix);
        TCHAR newPath[MAX_PATH];
        lstrcpyn(newPath, strPrefix, MAX_PATH);
        lstrcpyn(newPath + lenPrefix, path, MAX_PATH - lenPrefix);

        if (FAILED(D3DXLoadMeshFromX(newPath, D3DXMESH_SYSTEMMEM,
            *ppDevice, NULL,
            &pD3DXMtrlBuffer, NULL, &dwNumMaterials,
            &pMesh)))
        {
            OutputDebugString(TEXT("Couldn't find .x file"));
            return E_FAIL;
        }
    }

    //we need to extract the material properties and texture names from the pD3DXMtrlBuffer
    D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
    pMeshMaterials = new D3DMATERIAL9[dwNumMaterials];
    pMeshTextures = new LPDIRECT3DTEXTURE9[dwNumMaterials];

    for (DWORD i = 0; i < dwNumMaterials; i++) {
        //copy the material
        pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

        //set the ambient colour for the material (D3DX does not do this)
        pMeshMaterials[i].Ambient = pMeshMaterials[i].Diffuse;

        pMeshTextures[i] = NULL;
        if (d3dxMaterials[i].pTextureFilename != NULL &&
            lstrlenA(d3dxMaterials[i].pTextureFilename) > 0) {

            //create the texture
            if (FAILED(D3DXCreateTextureFromFile(*ppDevice,
                CA2CT(d3dxMaterials[i].pTextureFilename),
                &pMeshTextures[i]))) {

                //if texture is not in current folder, try parent folder
                const TCHAR* strPrefix = TEXT("..\\");
                const int lenPrefix = lstrlen(strPrefix);
                TCHAR strTexture[MAX_PATH];
                lstrcpyn(strTexture, strPrefix, MAX_PATH);
                lstrcpyn(strTexture + lenPrefix, CA2CT(d3dxMaterials[i].pTextureFilename), MAX_PATH - lenPrefix);

                //if texture is not in current folder try parent folder
                if (FAILED(D3DXCreateTextureFromFile(*ppDevice,
                    strTexture,
                    &pMeshTextures[i])))
                {
                    //MessageBox(NULL, TEXT("Could not find texture map"), TEXT("Assignment2.exe"), MB_OK);
                    OutputDebugString(TEXT("Couldn't find texture map"));
                }
            }
        }
    }

    //done with the material buffer
    pD3DXMtrlBuffer->Release();

    name = n;

    return S_OK;
}