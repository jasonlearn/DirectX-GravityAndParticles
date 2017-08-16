#include "Bounds.h"

Sphere::Sphere() {
    _radius = 0.0f;
}

BoundingBox::BoundingBox()
{
    // infinite small bounding box
    _min.x = FLT_MAX;
    _min.y = FLT_MAX;
    _min.z = FLT_MAX;

    _max.x = -FLT_MAX;
    _max.y = -FLT_MAX;
    _max.z = -FLT_MAX;
}

bool BoundingBox::isPointInside(D3DXVECTOR3& p)
{
    // is the point inside the bounding box?
    if (p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
        p.x <= _max.x && p.y <= _max.y && p.z <= _max.z)
        return true;
    else
        return false;
}