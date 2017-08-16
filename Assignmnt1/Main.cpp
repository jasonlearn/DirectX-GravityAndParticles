#include "Main.h"

/*
* The main method of the program
* params:
*   hInstance - the handle to the current instance for the application
*   hPRevInstance - handle to the previous instance of the application
*   szCmdLine - command line for the application
*   iCmdShow - how the window is shown
* return:
*   success of the application running
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("Assignment1 DirectX9");
    HWND         hwnd;
    MSG          msg;
    WNDCLASSEX   wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_HAND);
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szAppName;

    RegisterClassEx(&wc);

    hwnd = CreateWindowEx(NULL,
        szAppName,                  // window class name
        szAppName,                  // window caption
        WS_OVERLAPPEDWINDOW,        // window style
        CW_USEDEFAULT,              // initial x position
        CW_USEDEFAULT,              // initial y position
        512,                        // initial x size
        512,                        // initial y size
        NULL,                       // parent window handle
        NULL,                       // window menu handle
        hInstance,                  // program instance handle
        NULL);                      // creation parameters

    pGame = new Game();

    //set our global window handle
    pGame->setHWND(&hwnd);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    if (FAILED(pGame->gameInit())) { //initialize Game
        pGame->SetError("Initialization Failed");
        free(pGame);
        return E_FAIL;
    }

    while (TRUE) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            pGame->gameLoop();
        }
    }
    free(pGame);// clean up the game

    return msg.wParam;
}

/*
* The main method of the program
* params:
*   hwnd - handle to the window
*   message - the message sent
*   wParam - additional message information
*   lParam - additional message information
* return:
*   result of the message processed
*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        return 0;

    case WM_PAINT:
        ValidateRect(hwnd, NULL);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}