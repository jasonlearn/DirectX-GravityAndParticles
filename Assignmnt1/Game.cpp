#include "Game.h"

//default ctor
Game::Game() { }

/*
* releases resources.  First display adapter because COM object created it, then COM object
*/
Game::~Game() {

    if (models) {
        int length = 2;
        for (int i = 0; i < length; i++)
            models[i].release();
        delete[] models;
    }
    if (player)
        free(player);
    
    if (pDevice)
        pDevice->Release();
    if (pD3D)
        pD3D->Release();
    if (font)
        font->Release();
    if (pTimer)
        free(pTimer);
    if (pSurface)
        pSurface->Release();
}

/*
* loop for the game
* Return:
*   if the game loop was successful
*/
int Game::gameLoop() {
    pTimer->frameStart();
    render();
    pTimer->frameEnd();

    if (GetAsyncKeyState(VK_ESCAPE))
        PostQuitMessage(0);
    return S_OK;
}

/*
* initializes the COM object and the pDevice, setup for the game
* Return:
*   if game initialization was successful
*/
int Game::gameInit() {
    HRESULT r = 0; //return value

    // seed random number generator
    srand((unsigned int)time(0));

    pD3D = Direct3DCreate9(D3D_SDK_VERSION); //COM object
    if (pD3D == NULL) {
        SetError("Couldn't create IDirect3D9 object");
        return E_FAIL;
    }

    r = InitDirect3DDevice(hWndMain, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), TRUE, D3DFMT_X8R8G8B8, pD3D, &pDevice);
    if (FAILED(r)) { //FAILED is a macro that returns false if return value is failure - safer than using the value itself
        SetError("Initialization of the device failed!");
        return E_FAIL;
    }
    
    //create FPS timer
    pTimer = new Timer();
    if (FAILED(pTimer)) {
        SetError("Initialization of the timer failed!");
        return E_FAIL;
    }

    //setup text font
    font = NULL;
    r = D3DXCreateFont(pDevice, 25, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &font);
    if (FAILED(r)) {
        SetError("Initialization of the font failed!");
        return E_FAIL;
    }

    SetRect(&fRect, 0, 0, 200, 100);  //set dimantions of the text rectangle

    //setup bitmap
    r = initBitmap(TEXT("baboon.bmp"));
    if (FAILED(r)) {
        SetError("Initialization of the bitmap failed!");
        return E_FAIL;
    }
    
    //setup camera
    player = new Camera();

    //setup the scene geometry
    r = loadModels();
    if (FAILED(r)) {
        SetError("Initialization of the geometry failed!");
        return E_FAIL;
    }

    //create the cube object
    cube.Setup(&pDevice);

    //create and setup the snow particle effect
    BoundingBox bb;
    bb._min = D3DXVECTOR3(-100.0f, -100.0f, -100.0f);
    bb._max = D3DXVECTOR3(100.0f, 100.0f, 100.0f);
    snow = new Snow(&bb, 5000);
    if (!snow->init(&pDevice, TEXT("snowflake.dds")))
        OutputDebugStringA("No snowflake");
    
    D3DXVECTOR3 origin(0.0f, 10.0f, 50.0f);
    exp = new Firework(&origin, 6000);
    exp->init(&pDevice, TEXT("flare.bmp"));


    //setup the lighting
    initLighting();
    setLightingMode(AMBIENT);

    changingLighting = false;
    changingSelection = false;

    //enables mirroring of multiple textures
    pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    return S_OK;
}

/*
* sets the handle to the window for the game
* Params:
*   pHWND - the pointer to the handle to the window
*/
void Game::setHWND(HWND *pHWND) {
    hWndMain = *pHWND;
}

/*
* initializes directX 9 device
* Params:
*   hWndTarget - the handle to the window
*   Width - width of window
*   Height - height of window
*   bWindowed - if the window should be windowed
*   FullScreenFormat - format for fullscreen
*   pD3D - DirectX COM object
*   ppDevice - Device to be initialized
* Return:
*   if init was successful
*/
int Game::InitDirect3DDevice(HWND hWndTarget, int Width, int Height, BOOL bWindowed, D3DFORMAT FullScreenFormat, LPDIRECT3D9 pD3D, LPDIRECT3DDEVICE9* ppDevice) {
    D3DPRESENT_PARAMETERS d3dpp;//rendering info
    D3DDISPLAYMODE d3ddm;//current display mode info
    HRESULT r = 0;

    if (*ppDevice)
        (*ppDevice)->Release();

    ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
    r = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
    if (FAILED(r)) {
        SetError("Could not get display adapter information");
        return E_FAIL;
    }

    d3dpp.BackBufferWidth = Width;
    d3dpp.BackBufferHeight = Height;
    d3dpp.BackBufferFormat = bWindowed ? d3ddm.Format : FullScreenFormat;
    d3dpp.BackBufferCount = 1;
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_COPY;
    d3dpp.hDeviceWindow = hWndTarget;
    d3dpp.Windowed = bWindowed;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;//D3DFMT_D16;
    d3dpp.FullScreen_RefreshRateInHz = 0;//default refresh rate
    d3dpp.PresentationInterval = bWindowed ? 0 : D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    r = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWndTarget, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, ppDevice);
    if (FAILED(r)) {
        SetError("Could not create the render device");
        return E_FAIL;
    }

    //turn on the zbuffer
    (*ppDevice)->SetRenderState(D3DRS_ZENABLE, TRUE);

    return S_OK;
}

/*
* initializes The .x files into the program
* Return:
*   if loading the models was successful
*/
HRESULT Game::loadModels() {
    std::string s;
    models = new Model[3];

    for (size_t i = 0; i < 3; ++i)
        models[i].setDevice(&pDevice);

    if (FAILED(models[0].loadModel(TEXT("teapot.x"), "teapot")))
        return E_FAIL;
    //set initial position
    models[0].setPosition(3, 0, 0);
    models[0].calculateBoundingSphere();

    if (FAILED(models[1].loadModel(TEXT("chair.x"), "chair")))
        return E_FAIL;
    //set initial position
    models[1].setPosition(-7, -2.25, 0);
    models[1].calculateBoundingSphere();
        
    if (FAILED(models[2].loadModel(TEXT("room.x"), "room")))
        return E_FAIL;
    //set initial position
    models[2].setPosition(0, -2.15f, 0);

    return S_OK;
}

/*
* initializes the lighting objects
*/
void Game::initLighting() {
    D3DLIGHT9 light;
    
    //Setup Point light
    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_POINT;    //make the light type point light
    light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    light.Position = D3DXVECTOR3(0.0f, 2.0f, 0.f);
    light.Range = 100.0f;   
    light.Attenuation0 = 0.0f;  
    light.Attenuation1 = 0.125f;
    light.Attenuation2 = 0.0f;
    D3DXVECTOR3 vecDir;
    vecDir = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
    //attach light structure to a Direct3D Lighting index
    pDevice->SetLight(0, &light);
    pDevice->LightEnable(0, FALSE);

    //Setup Directional light
    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_DIRECTIONAL;    //make the light type directional light
    light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    light.Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    vecDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
    //attach light structure to a Direct3D Lighting index
    pDevice->SetLight(1, &light);
    pDevice->LightEnable(1, FALSE);
    
    //Setup Spot Light
    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_SPOT;    //make the light type spot light
    light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
    light.Position = D3DXVECTOR3(-5.0f, 10.0f, 0.0f);
    light.Direction = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
    vecDir = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
    light.Range = 100.0f;
    light.Attenuation0 = 0.0f;
    light.Attenuation1 = 0.125f;
    light.Attenuation2 = 0.0f;
    light.Phi = D3DXToRadian(40.0f);   //set the outer cone to 30 degrees
    light.Theta = D3DXToRadian(20.0f);  //set the inner cont to 10 degrees
    light.Falloff = 1.0f;

    //attach light structure to a Direct3D Lighting index
    pDevice->SetLight(2, &light);
    pDevice->LightEnable(2, FALSE);

    // Turn on lighting
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

/*
* tuns off all lighting modes except for the selected lighting mode
* Params:
*   pathName - The lighting mode to switch to
*/
void Game::setLightingMode(LightingType lighting) {
    lightingMode = lighting;
    
    switch(lighting) {
    case AMBIENT:
        // Set the ambient light level
        pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(150, 150, 150));
        for (int i = 0; i < 3; i++)
            pDevice->LightEnable(i, FALSE);

        lightingDesc = "Ambient";
        break;
    case POINT:
        // Set the ambient light level
        pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(25, 25, 25));
        pDevice->LightEnable(0, TRUE);
        pDevice->LightEnable(1, FALSE);
        pDevice->LightEnable(2, FALSE);

        lightingDesc = "Point";
        break;
    case DIRECTIONAL:
        // Set the ambient light level
        pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(25, 25, 25));
        pDevice->LightEnable(0, FALSE);
        pDevice->LightEnable(1, TRUE);
        pDevice->LightEnable(2, FALSE);

        lightingDesc = "Direntional";
        break;
    case SPOTLIGHT:
        // Set the ambient light level
        pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(25, 25, 25));
        pDevice->LightEnable(0, FALSE);
        pDevice->LightEnable(1, FALSE);
        pDevice->LightEnable(2, TRUE);

        lightingDesc = "Spot";
        break;
    }
}

/*
* sets up the transformation matrix for a specific model
* Params:
*   m - the model to setup the matrix for
*/
void Game::setupMatrix(Model &m) {
    //for our world matrix, we will just leave it as the identity
    D3DXMATRIXA16 matWorld;
    pDevice->SetTransform(D3DTS_WORLD, &m.getTransformationMatrix());

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 1000.0f);
    pDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}


/*
* prints out error message
* Params:
*   scFormat - format for the message
*   additional messages as per the format
*/
void Game::SetError(char* szFormat, ...) {
    char szBuffer[1024];
    va_list pArgList;

    va_start(pArgList, szFormat);

    sprintf_s(szBuffer, sizeof(szBuffer) / sizeof(char), szFormat, pArgList);

    va_end(pArgList);

    OutputDebugStringA(szBuffer);
    OutputDebugStringA("\n");
}

/*
* initializes bitmap
* Params:
*   pathName - The name of the bitmap
* Return:
*   if the initializing was successful
*/
int Game::initBitmap(const TCHAR* pathName) {
    HRESULT r = 0;
    pSurface = 0;

    //create surface for bitmap
    r = pDevice->CreateOffscreenPlainSurface(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &pSurface, NULL);
    if (FAILED(r)) {
        SetError("Unable to create surface for bitmap load");
        return E_FAIL;
    }

    //load bitmap onto surface
    r = D3DXLoadSurfaceFromFile(pSurface, NULL, NULL, pathName, NULL, D3DX_DEFAULT, 0, NULL);
    if (FAILED(r)) {
        SetError("Unable to laod file to surface");
        return E_FAIL;
    }

    return S_OK;
}

/*
* Renders each frame for the game
* Return:
*   If the rendering was succesful
*/
int Game::render() {
    HRESULT r;
    LPDIRECT3DSURFACE9 pBackSurf = 0;
    float dTime = pTimer->getDeltaTime(); //the delta time

    D3DXMATRIX I;
    D3DXMatrixIdentity(&I);

    if (!pDevice) {
        SetError("Cannot render because there is no device");
        return E_FAIL;
    }

    snow->update(dTime);

    exp->update(dTime);

    if (exp->isDead())
        exp->reset();

    //clear display area with colour blue
    pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 25), 1.0f, 0);

    //get pointer to backbuffer
    r = pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurf);
    if (FAILED(r)) {
        SetError("Couldn't get backbuffer");
    }
    
    //load bitmap before locking rect
    r = pDevice->UpdateSurface(pSurface, NULL, pBackSurf, NULL);
    if (FAILED(r))
        SetError("did not copy surface");
    
    // Begin the scene
    if (SUCCEEDED(pDevice->BeginScene())) {
        updateCamera(dTime);
        checkTransformation(dTime);
        checkSelection();

        pDevice->SetTransform(D3DTS_WORLD, &I);
        cube.Display(dTime);
        
        for (DWORD i = 0; i < Model::getNumModels(); i++) {
            setupMatrix(models[i]);

            models[i].draw();
        }

        displayInfo();

        for (int i = 0; i < 6; i++)
            renderMirror(i);

        pDevice->SetTransform(D3DTS_WORLD, &I);
        snow->render();

        //pDevice->SetTransform(D3DTS_WORLD, &I);
        //exp->render();

        //end the scene
        pDevice->EndScene();
    }

    pBackSurf->Release(); //release lock

    pBackSurf = 0;

    pDevice->Present(NULL, NULL, NULL, NULL); //swap over buffer to primary surface
    return S_OK;
}

/*
* displays the current FPS, the lighting mode, and current model selected
*/
void Game::displayInfo() {
    if (font) {
        std::stringstream ss;

        ss << std::setprecision(0) << pTimer->getFPS() << " fps" <<
            "\nLighting: " << lightingDesc <<
            "\nSelected: " << (selectedModel < Model::getNumModels() ? models[selectedModel].name : "No Models");
        font->DrawTextA(NULL, ss.str().c_str(), -1, &fRect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 255));
    }
}

/*
* Checks if you are changing the selected model or lighting mode
*/
void Game::checkSelection() {
    //object picking
    if ((GetKeyState(VK_LBUTTON) & 0x80) != 0) {
        for (DWORD i = 0; i < Model::getNumModels(); i++) {
            ::POINT p;
            GetCursorPos(&p);

            models[i].updateCenter();

            // compute the ray in view space given the clicked screen point
            Game::Ray ray = calculatePickingRay(p.x, p.y);

            // transform the ray to world space
            D3DXMATRIX view;
            pDevice->GetTransform(D3DTS_VIEW, &view);

            D3DXMATRIX viewInverse;
            D3DXMatrixInverse(&viewInverse, 0, &view);

            TransformRay(&ray, &viewInverse);

            // test for a hit
            if (checkPickingRay(&ray, &models[i].BoundingSphere))
                selectedModel = i;
        }
    }

    //lighting modes
    if (changingLighting) {
        if (!(::GetKeyState(VK_TAB) & (1 << 16)))
            changingLighting = false;
    } else {
        if (::GetKeyState(VK_TAB) & (1 << 16)) {
            changingLighting = true;

            switch (lightingMode) {
            case AMBIENT:
                setLightingMode(POINT);
                break;
            case POINT:
                setLightingMode(DIRECTIONAL);
                break;
            case DIRECTIONAL:
                setLightingMode(SPOTLIGHT);
                break;
            case SPOTLIGHT:
                setLightingMode(AMBIENT);
                break;
            }
        }
    }
}

/*
*    Checks for translations and rotations:
*        Translation: 
*            Forward = U
*            Backwards = J
*            Left = H
*            Right = K
*            Up = O
*            Down = L
*        Rotation:
*            rotateX = B,
*            rotateY = N,
*            rotateZ = M
*   param:
*       timeDelta - the change in time from the last frame
*/
void Game::checkTransformation(float timeDelta) {
    if (selectedModel < Model::getNumModels()) {
        //X
        if (::GetAsyncKeyState('K') & 0x8000f)
            models[selectedModel].x += 2 * timeDelta;
        if (::GetAsyncKeyState('H') & 0x8000f)
            models[selectedModel].x -= 2 * timeDelta;
        if (::GetAsyncKeyState('B') & 0x8000f)
            models[selectedModel].rotationX += timeDelta;

        //Y
        if (::GetAsyncKeyState('O') & 0x8000f)
            models[selectedModel].y += 2 * timeDelta;
        if (::GetAsyncKeyState('L') & 0x8000f)
            models[selectedModel].y -= 2 * timeDelta;
        if (::GetAsyncKeyState('N') & 0x8000f)
            models[selectedModel].rotationY += timeDelta;

        //Z
        if (::GetAsyncKeyState('U') & 0x8000f)
            models[selectedModel].z += 2 * timeDelta;
        if (::GetAsyncKeyState('J') & 0x8000f)
            models[selectedModel].z -= 2 * timeDelta;
        if (::GetAsyncKeyState('M') & 0x8000f)
            models[selectedModel].rotationZ += timeDelta;
    }
}

/*
* updates the position of the camera
* Code from textbook:
*   Introduction to 3D Game Programming with DirectX 9.0
* param:
*       timeDelta - the change in time from the last frame
*/
void Game::updateCamera(float timeDelta) {
    if (pDevice) {
        //
        // Update: Update the camera.
        //
        if (::GetAsyncKeyState('W') & 0x8000f)
            player->walk(4.0f * timeDelta);
        if (::GetAsyncKeyState('S') & 0x8000f)
            player->walk(-4.0f * timeDelta);
        if (::GetAsyncKeyState('A') & 0x8000f)
            player->strafe(-4.0f * timeDelta);
        if (::GetAsyncKeyState('D') & 0x8000f)
            player->strafe(4.0f * timeDelta);
        if (::GetAsyncKeyState('R') & 0x8000f)
            player->fly(4.0f * timeDelta);
        if (::GetAsyncKeyState('F') & 0x8000f)
            player->fly(-4.0f * timeDelta);
        if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
            player->pitch(1.0f * timeDelta);
        if (::GetAsyncKeyState(VK_UP) & 0x8000f)
            player->pitch(-1.0f * timeDelta);
        if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
            player->yaw(-1.0f * timeDelta);
        if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
            player->yaw(1.0f * timeDelta);
        if (::GetAsyncKeyState('Q') & 0x8000f)
            player->roll(1.0f * timeDelta);
        if (::GetAsyncKeyState('E') & 0x8000f)
            player->roll(-1.0f * timeDelta);

        // Update the view matrix representing the cameras
        // new position/orientation.
        D3DXMATRIX V;
        player->getViewMatrix(&V);
        pDevice->SetTransform(D3DTS_VIEW, &V);
    }
}

/*
 *
 *
 *
 */
Game::Ray Game::calculatePickingRay(int x, int y) {
    float px = 0.0f;
    float py = 0.0f;

    D3DVIEWPORT9 vp;
    pDevice->GetViewport(&vp);

    D3DXMATRIX proj;
    pDevice->GetTransform(D3DTS_PROJECTION, &proj);

    px = (((2.0f*x) / vp.Width) - 1.0f) / proj(0, 0);
    py = (((-2.0f*y) / vp.Height) + 1.0f) / proj(1, 1);

    Game::Ray ray;
    
    //player->getPosition(&ray._origin);
    ray._origin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    ray._direction = D3DXVECTOR3(px, py, 1.0f);

    return ray;
}


void Game::TransformRay(Game::Ray* ray, D3DXMATRIX* T)
{
    // transform the ray's origin, w = 1.
    D3DXVec3TransformCoord(
        &ray->_origin,
        &ray->_origin,
        T);
    // transform the ray's direction, w = 0.
    D3DXVec3TransformNormal(
        &ray->_direction,
        &ray->_direction,
        T);
    // normalize the direction
    D3DXVec3Normalize(&ray->_direction, &ray->_direction);
}

bool Game::checkPickingRay(Game::Ray* ray, Sphere* sphere) {
    D3DXVECTOR3 v = ray->_origin - sphere->_center;

    float b = 2.0f * D3DXVec3Dot(&ray->_direction, &v);
    float c = D3DXVec3Dot(&v, &v) - (sphere->_radius * sphere->_radius);

    //find the discriminant
    float disc = (b * b) - (4.0f * c);

    //test imaginary number
    if (disc < 0.0)
        return false;

    disc = sqrtf(disc);

    float s0 = (-b + disc) / 2.0f;
    float s1 = (-b - disc) / 2.0f;

    //if solution is >= 0, then we intersected the sphere
    if (s0 >= 0.0f || s1 >= 0.0f)
        return true;

    return false;
}

void Game::renderMirror(int face) {

    pDevice->Clear(0, 0, D3DCLEAR_STENCIL, 0, 1.0f, 0);
    

    pDevice->SetRenderState(D3DRS_STENCILENABLE, true);
    pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
    pDevice->SetRenderState(D3DRS_STENCILREF, 0x1);
    pDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
    pDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
    pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
    pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

    // disable writes to the depth and back buffers
    pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    // draw the mirror to the stencil buffer
    pDevice->SetStreamSource(0, cube.getVertexBuffer(), 0, sizeof(Vertex));
    pDevice->SetFVF(Vertex::FVF);
    pDevice->SetMaterial(cube.getMaterial());
    pDevice->SetTexture(0, cube.getTexture());
    D3DXMATRIX I;
    D3DXMatrixIdentity(&I);
    pDevice->SetTransform(D3DTS_WORLD, &I);
    pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, face * 6, 2);

    // re-enable depth writes
    pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

    // only draw reflected model to the pixels where the mirror was drawn
    pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
    pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

    // position reflection
    D3DXMATRIX W, T, R;
    D3DXPLANE plane;
    Vertex checkFace = cube.getFaceVertices()[face];

    //get correct reflection plane
    switch (face / 2) {
    //front & back
    case 0:
        plane = D3DXPLANE(0.0f, 0.0f, 1.0f, 0.0f); //xy plane
        break;
    //top & bottom
    case 1:
        plane = D3DXPLANE(0.0f, 1.0f, 0.0f, 0.0f); //xz plane
        break;
    //left & right
    case 2:
        plane = D3DXPLANE(1.0f, 0.0f, 0.0f, 0.0f); //yz plane
        break;
    default:
        plane = D3DXPLANE();
        break;
    }
    D3DXMatrixReflect(&R, &plane);

    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

    //do for every model
    for (DWORD i = 0; i < Model::getNumModels() - 1; i++) {
        if (checkFace._nx > 0) {
            if (models[i].x < checkFace._x)
                continue;
        } else if (checkFace._nx < 0) {
            if (models[i].x > checkFace._x)
                continue;
        }

        if (checkFace._ny > 0) {
            if (models[i].y < checkFace._y)
                continue;
        }  else if (checkFace._ny < 0) {
            if (models[i].y > checkFace._y)
                continue;
        }

        if (checkFace._nz > 0) {
            if (models[i].z < checkFace._z)
                continue;
        } else if (checkFace._nz < 0) {
            if (models[i].z > checkFace._z)
                continue;
        }

        T = models[i].getTransformationMatrix();

        W = T * R;

        // clear depth buffer and blend the reflected teapot with the mirror
        pDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

        // Finally, draw the reflected model
        pDevice->SetTransform(D3DTS_WORLD, &W);
        
        models[i].draw();
        /*
        if (checkFace._nx > 0) {
            if (models[i].x < checkFace._x) {
                D3DRECT rect = D3DRECT();
                pDevice->Clear(0, , D3DCLEAR_STENCIL, 0, 1.0f, 0);
            }
                
        }
        else if (checkFace._nx < 0) {
            if (models[i].x > checkFace._x)
                continue;
        }

        if (checkFace._ny > 0) {
            if (models[i].y < checkFace._y)
                continue;
        }
        else if (checkFace._ny < 0) {
            if (models[i].y > checkFace._y)
                continue;
        }

        if (checkFace._nz > 0) {
            if (models[i].z < checkFace._z)
                continue;
        }
        else if (checkFace._nz < 0) {
            if (models[i].z > checkFace._z)
                continue;
        }*/
    }
    // Restore render states.
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
    pDevice->SetRenderState(D3DRS_STENCILENABLE, false);
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}