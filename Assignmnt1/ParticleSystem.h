#pragma once
#ifndef PARTICLESYSTEM
#define PARTICLESYSTEM
#include <list>
#include "Particle.h"
#include "Bounds.h"

class ParticleSystem {
public:
    ParticleSystem();
    virtual ~ParticleSystem();

    virtual bool init(LPDIRECT3DDEVICE9 *ppd, TCHAR* texFileName);
    virtual void reset();
    virtual void resetParticle(Attribute* attribute) = 0;
    virtual void addParticle();
    virtual void update(float timeDelta) = 0;

    virtual void preRender();
    virtual void render();
    virtual void postRender();

    //helper functions
    DWORD FtoDw(float);
    float GetRandomFloat(float, float);
    void GetRandomVector(D3DXVECTOR3*, D3DXVECTOR3*, D3DXVECTOR3*);

    bool isEmpty();
    bool isDead();
protected:
    virtual void removeDeadParticles();

    LPDIRECT3DDEVICE9* ppDevice;
    D3DXVECTOR3 _origin;
    BoundingBox _boundingBox; //used to keep particles only inside a bounding box
    float _emitRate;
    float _size;
    IDirect3DTexture9* _tex;    //texture of particle
    IDirect3DVertexBuffer9* _vb; //vertex buffer
    std::list<Attribute> _particles;
    int _maxParticles;
    DWORD _vbSize;
    DWORD _vbOffset;
    DWORD _vbBatchSize;
};


class Snow : public ParticleSystem {
public:
    Snow(BoundingBox* boundingBox, int numParticles);
    void resetParticle(Attribute* attribute);
    void update(float timeDelta);
};

class Firework : public ParticleSystem
{
public:
    Firework(D3DXVECTOR3* origin, int numParticles);
    void resetParticle(Attribute* attribute);
    void update(float timeDelta);
    void preRender();
    void postRender();
};

#endif