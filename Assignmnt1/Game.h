#include "Timer.h"
#include "Model.h"
#include "Camera.h"
#include "Cube.h"
#include "Bounds.h"
#include "ParticleSystem.h"
#include <Windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <atlbase.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

class Game {
public:
    Game();
    ~Game();

    int gameInit();
    int gameLoop();
    void setHWND(HWND*);
    void SetError(char*, ...);
private:

    struct Ray {
        D3DXVECTOR3 _origin;
        D3DXVECTOR3 _direction;
    };

    enum LightingType {
        AMBIENT, POINT, DIRECTIONAL, SPOTLIGHT
    };

    int render();
    int initBitmap(const TCHAR*);
    int InitDirect3DDevice(HWND, int, int, BOOL, D3DFORMAT, LPDIRECT3D9, LPDIRECT3DDEVICE9*);
    void displayInfo();
    HRESULT loadModels();
    //void setupMatricies();
    void setupMatrix(Model&);
    void updateCamera(float timeDelta);
    void checkSelection();
    void checkTransformation(float);
    void initLighting();
    void setLightingMode(LightingType);
    Ray calculatePickingRay(int, int);
    void TransformRay(Ray*, D3DXMATRIX*);
    bool checkPickingRay(Ray*, Sphere*);

    //mirror dunctions
    void renderMirror(int);

    LPDIRECT3D9 pD3D = 0; //COM object
    LPDIRECT3DDEVICE9 pDevice = 0; //graphics device
    HWND hWndMain; //handle to main window
    //timer
    Timer* pTimer; //timer object (FPS counter)
    ID3DXFont *font; //font to display FPS
    RECT fRect; //area to display text
    //bitmap
    BITMAP bitmap; //bitmap
    LPDIRECT3DSURFACE9 pSurface; //surface
    //Meshes
    Model *models = NULL; //Holds information for each model
    DWORD selectedModel = 0;
    bool changingSelection;
    Cube cube;
    //Camera
    Camera *player = NULL;
    //lighting
    LightingType lightingMode;
    std::string lightingDesc;
    bool changingLighting;
    //Particles
    ParticleSystem *snow = 0;
    ParticleSystem *exp = 0;
};