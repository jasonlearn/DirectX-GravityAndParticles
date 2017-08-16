//class from the textbook:
//    Introduction to 3D Game Programming with DirectX 9.0
#include "Camera.h"

/*
* Default ctor, initializes the camera and defaults to AIRCRAFT camera mode
*/
Camera::Camera():
    _cameraType(AIRCRAFT),
    _right(1.0f, 0, 0),
    _up(0.0f, 1.0f, 0.0f),
    _look(0.0f, -0.3f, 1.0f),
    _pos(0.0f, 5.0f, -20.0f) { }

/*
* ctor with specified camera mode
* params:
*   CT - the camera type
*/
Camera::Camera(CameraType CT) :
    _cameraType(CT),
    _right(1.0f, 0, 0),
    _up(0.0f, 1.0f, 0.0f),
    _look(0.0f, -0.3f, 1.0f),
    _pos(-2.0f, 5.0f, -20.0f) { }

/*
* saves the net transformation matrix for the camera to V
* params:
*   V - the D3DXMATRIX object to be saved to
*/
void Camera::getViewMatrix(D3DXMATRIX* V)
{
    // Keep camera's axes orthogonal to each other:
    D3DXVec3Normalize(&_look, &_look);
    D3DXVec3Cross(&_up, &_look, &_right);
    D3DXVec3Normalize(&_up, &_up);
    D3DXVec3Cross(&_right, &_up, &_look);
    D3DXVec3Normalize(&_right, &_right);
    // Build the view matrix:
    float x = -D3DXVec3Dot(&_right, &_pos);
    float y = -D3DXVec3Dot(&_up, &_pos);
    float z = -D3DXVec3Dot(&_look, &_pos);
    (*V)(0, 0) = _right.x;
    (*V)(0, 1) = _up.x;
    (*V)(0, 2) = _look.x;
    (*V)(0, 3) = 0.0f;
    (*V)(1, 0) = _right.y;
    (*V)(1, 1) = _up.y;
    (*V)(1, 2) = _look.y;
    (*V)(1, 3) = 0.0f;
    (*V)(2, 0) = _right.z;
    (*V)(2, 1) = _up.z;
    (*V)(2, 2) = _look.z;
    (*V)(2, 3) = 0.0f;
    (*V)(3, 0) = x;
    (*V)(3, 1) = y;
    (*V)(3, 2) = z;
    (*V)(3, 3) = 1.0f;
}

/*
* change the angle that the camera is looking (up/down)
* params:
*   angle - the angle to turn the camera up/down
*/
void Camera::pitch(float angle)
{
    D3DXMATRIX T;
    D3DXMatrixRotationAxis(&T, &_right, angle);
    // rotate _up and _look around _right vector
    D3DXVec3TransformCoord(&_up, &_up, &T);
    D3DXVec3TransformCoord(&_look, &_look, &T);
}

/*
* change the angle that the camera is looking (left/right)
* params:
*   angle - the angle to turn the camera left/right
*/
void Camera::yaw(float angle)
{
    D3DXMATRIX T;
    // rotate around world y (0, 1, 0) always for land object
    if (_cameraType == LANDOBJECT)
        D3DXMatrixRotationY(&T, angle);
    // rotate around own up vector for aircraft
    if (_cameraType == AIRCRAFT)
        D3DXMatrixRotationAxis(&T, &_up, angle);
    // rotate _right and _look around _up or y-axis
    D3DXVec3TransformCoord(&_right, &_right, &T);
    D3DXVec3TransformCoord(&_look, &_look, &T);
}

/*
* roll the camera by an angle
* params:
*   angle - the angle to roll the camera
*/
void Camera::roll(float angle)
{
    // only roll for aircraft type
    if (_cameraType == AIRCRAFT)
    {
        D3DXMATRIX T;
        D3DXMatrixRotationAxis(&T, &_look, angle);
        // rotate _up and _right around _look vector
        D3DXVec3TransformCoord(&_right, &_right, &T);
        D3DXVec3TransformCoord(&_up, &_up, &T);
    }
}

/*
* walk in the direction the camera is facing
* params:
*   units - the amount of units the camera will move
*/
void Camera::walk(float units)
{
    // move only on xz plane for land object
    if (_cameraType == LANDOBJECT)
        _pos += D3DXVECTOR3(_look.x, 0.0f, _look.z) * units;
    if (_cameraType == AIRCRAFT)
        _pos += _look * units;
}

/*
* walk sideways from where the camera is looking (right/left)
* params:
*   units - the amount of units the camera will move
*/
void Camera::strafe(float units)
{
    // move only on xz plane for land object
    if (_cameraType == LANDOBJECT)
        _pos += D3DXVECTOR3(_right.x, 0.0f, _right.z) * units;
    if (_cameraType == AIRCRAFT)
        _pos += _right * units;
}

/*
* camera flies up or down if it is an aircraft
* params:
*   units - the amount of units the camera will move
*/
void Camera::fly(float units)
{
    if (_cameraType == AIRCRAFT)
        _pos += _up * units;
}

/*
* sets the camera type
* params:
*   units - the camera type
*/
void Camera::setCameraType(CameraType cameraType) {
    _cameraType = cameraType;
}

/*
* stores the position of the camera in the parameter
* params:
*   pos - the pointer to save the position to
*/
void Camera::getPosition(D3DXVECTOR3* pos) {
    *pos = _pos;
}

/*
* sets the position of the camera
* params:
*   pos - the pointer to the new position
*/
void Camera::setPosition(D3DXVECTOR3* pos) {
    _pos = *pos;
}

/*
* stores the right vector of the camera in the parameter
* params:
*   pos - the pointer to save the vector to
*/
void Camera::getRight(D3DXVECTOR3* right) {
    *right = _right;
}

/*
* stores the up vector of the camera in the parameter
* params:
*   pos - the pointer to save the vector to
*/
void Camera::getUp(D3DXVECTOR3* up) {
    *up = _up;
}

/*
* stores the look vector of the camera in the parameter
* params:
*   pos - the pointer to save the vector to
*/
void Camera::getLook(D3DXVECTOR3* look) {
    *look = _look;
}