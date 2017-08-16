#include "ParticleSystem.h"

ParticleSystem::ParticleSystem() {
    ppDevice = 0;
    _vb = 0;
    _tex = 0;
}

ParticleSystem::~ParticleSystem() {
    if (_vb)
        _vb->Release();
    if (_tex)
        _tex->Release();
}

DWORD ParticleSystem::FtoDw(float f) {
    return *((DWORD*)&f);
}

float ParticleSystem::GetRandomFloat(float lowBound, float highBound) {
    if (lowBound >= highBound) // bad input
        return lowBound;
    // get random float in [0, 1] interval
    float f = (rand() % 10000) * 0.0001f;
    // return float in [lowBound, highBound] interval.
    return (f * (highBound - lowBound)) + lowBound;
}

void ParticleSystem::GetRandomVector(
        D3DXVECTOR3* out,
        D3DXVECTOR3* min,
        D3DXVECTOR3* max) {
    out->x = GetRandomFloat(min->x, max->x);
    out->y = GetRandomFloat(min->y, max->y);
    out->z = GetRandomFloat(min->z, max->z);
}

bool ParticleSystem::init(LPDIRECT3DDEVICE9 *ppd, TCHAR* texFileName) {
    
    ppDevice = ppd;

    HRESULT r = 0;

    r = (*ppDevice)->CreateVertexBuffer(
        _vbSize * sizeof (Particle),
        D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
        Particle::FVF,
        D3DPOOL_DEFAULT, //D3DPOOL_MANAGED cant be used with D3DUSAGE_DYNAMIC
        &_vb,
        0);
    if (FAILED(r)) {
        OutputDebugString(TEXT("CreateVertexBuffer() - FAILED"));
        return false;
    }

    r = D3DXCreateTextureFromFile(
        *ppDevice,
        texFileName,
        &_tex);

    if (FAILED(r)) {
        OutputDebugString(TEXT("D3DXCreateTextureFromFile() - FAILED"));
        return false;
    }

    return true;
}

void ParticleSystem::reset() {
    std::list<Attribute>::iterator i;
    for (i = _particles.begin(); i != _particles.end(); i++)
    {
        resetParticle(&(*i));
    }
}

void ParticleSystem::addParticle() {
    Attribute attribute;

    resetParticle(&attribute);

    _particles.push_back(attribute);
}

void ParticleSystem::preRender() {
    (*ppDevice)->SetRenderState(D3DRS_LIGHTING, false);
    (*ppDevice)->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
    (*ppDevice)->SetRenderState(D3DRS_POINTSCALEENABLE, true);
    (*ppDevice)->SetRenderState(D3DRS_POINTSIZE, FtoDw(_size));
    (*ppDevice)->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

    // control the size of the particle relative to distance
    (*ppDevice)->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(1.0f));
    (*ppDevice)->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(1.0f));
    (*ppDevice)->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

    // use alpha from texture
    (*ppDevice)->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    (*ppDevice)->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

    (*ppDevice)->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    (*ppDevice)->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    (*ppDevice)->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void ParticleSystem::render() {
    if (!_particles.empty())
    {
        // set render states
        preRender();

        (*ppDevice)->SetTexture(0, _tex);
        (*ppDevice)->SetFVF(Particle::FVF);
        (*ppDevice)->SetStreamSource(0, _vb, 0, sizeof(Particle));

        // start at beginning if we're at the end of the vb
        if (_vbOffset >= _vbSize)
            _vbOffset = 0;

        Particle* v = 0;

        _vb->Lock(
            _vbOffset * sizeof(Particle),
            _vbBatchSize * sizeof(Particle),
            (void**)&v,
            _vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

        DWORD numParticlesInBatch = 0;

        //
        // Until all particles have been rendered.
        //
        std::list<Attribute>::iterator i;
        for (i = _particles.begin(); i != _particles.end(); i++)
        {
            if (i->_isAlive)
            {

                //
                // Copy a batch of the living particles to the
                // next vertex buffer segment
                //
                v->position = i->_position;
                v->color = (D3DCOLOR)i->_color;
                v++; // next element;

                numParticlesInBatch++; //increase batch counter
                                       
                // is this batch full?
                if (numParticlesInBatch == _vbBatchSize)
                {
                    //
                    // Draw the last batch of particles that was
                    // copied to the vertex buffer.
                    //
                    _vb->Unlock();

                    (*ppDevice)->DrawPrimitive(
                        D3DPT_POINTLIST,
                        _vbOffset,
                        _vbBatchSize);

                    //
                    // While that batch is drawing, start filling the
                    // next batch with particles.
                    //

                    // move the offset to the start of the next batch
                    _vbOffset += _vbBatchSize;

                    // don't offset into memory thats outside the vb's
                    // range. If we're at the end, start at the beginning.
                    if (_vbOffset >= _vbSize)
                        _vbOffset = 0;

                    _vb->Lock(
                        _vbOffset * sizeof(Particle),
                        _vbBatchSize * sizeof(Particle),
                        (void**)&v,
                        _vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

                    numParticlesInBatch = 0; // reset for new batch
                }//end if
            }//end if
        }//end for

        _vb->Unlock();

        // it’s possible that the LAST batch being filled never
        // got rendered because the condition
        // (numParticlesInBatch == _vbBatchSize) would not have
        // been satisfied. We draw the last partially filled batch now.
        if (numParticlesInBatch)
        {
            (*ppDevice)->DrawPrimitive(
                D3DPT_POINTLIST,
                _vbOffset,
                numParticlesInBatch);
        }

        // next block
        _vbOffset += _vbBatchSize;

        postRender();
    }
}


void ParticleSystem::postRender() {
    (*ppDevice)->SetRenderState(D3DRS_LIGHTING, true);
    (*ppDevice)->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
    (*ppDevice)->SetRenderState(D3DRS_POINTSCALEENABLE, false);
    (*ppDevice)->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

bool ParticleSystem::isEmpty() {
    return _particles.empty();
}
bool ParticleSystem::isDead() {
    std::list<Attribute>::iterator i;
    for (i = _particles.begin(); i != _particles.end(); i++)
    {
        // is there at least one living particle?  If yes,
        // the system is not dead.
        if (i->_isAlive)
            return false;
    }
    // no living particles found, the system must be dead.
    return true;
}

void ParticleSystem::removeDeadParticles() {
    std::list<Attribute>::iterator i;

    i = _particles.begin();

    while (i != _particles.end())
    {
        if (i->_isAlive == false)
        {
            // erase returns the next iterator, so no need to
            // incrememnt to the next one ourselves.
            i = _particles.erase(i);
        }
        else
        {
            i++; // next in list
        }
    }
}


//--------------------
// Snow
//--------------------

Snow::Snow(BoundingBox* boundingBox, int numParticles) {
    _boundingBox = *boundingBox;
    _size = 0.25f;
    _vbSize = 2048;
    _vbOffset = 0;
    _vbBatchSize = 512;

    for (int i = 0; i < numParticles; i++)
        addParticle();
}

void Snow::resetParticle(Attribute* attribute)
{
    attribute->_isAlive = true;

    // get random x, z coordinate for the position of the snowflake.
    GetRandomVector(
        &attribute->_position,
        &_boundingBox._min,
        &_boundingBox._max);

    // no randomness for height (y-coordinate). Snowflake
    // always starts at the top of bounding box.
    attribute->_position.y = _boundingBox._max.y;

    // snowflakes fall downward and slightly to the left
    attribute->_velocity.x = GetRandomFloat(0.0f, 1.0f)*-3.0f;
    attribute->_velocity.y = GetRandomFloat(0.0f, 1.0f)*-10.0f;
    attribute->_velocity.z = 1.0f;

    // white snowflake
    attribute->_color = D3DCOLOR_XRGB(255, 255, 255);
}

void Snow::update(float timeDelta)
{
    std::list<Attribute>::iterator i;
    for (i = _particles.begin(); i != _particles.end(); i++)
    {
        i->_position += i->_velocity * timeDelta;
        // is the point outside bounds?
        if (_boundingBox.isPointInside(i->_position) == false)
        {
            // nope so kill it, but we want to recycle dead
            // particles, so respawn it instead.
            resetParticle(&(*i));
        }
    }
}

//*****************************************************************************
// Explosion System
//********************

Firework::Firework(D3DXVECTOR3* origin, int numParticles)
{
    _origin = *origin;
    _size = 0.9f;
    _vbSize = 2048;
    _vbOffset = 0;
    _vbBatchSize = 512;

    for (int i = 0; i < numParticles; i++)
        addParticle();
}

void Firework::resetParticle(Attribute* attribute)
{
    attribute->_isAlive = true;
    attribute->_position = _origin;

    D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
    D3DXVECTOR3 max = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

    GetRandomVector(
        &attribute->_velocity,
        &min,
        &max);

    // normalize to make spherical
    D3DXVec3Normalize(
        &attribute->_velocity,
        &attribute->_velocity);

    attribute->_velocity *= 100.0f;

    attribute->_color = D3DXCOLOR(
        GetRandomFloat(0.0f, 1.0f),
        GetRandomFloat(0.0f, 1.0f),
        GetRandomFloat(0.0f, 1.0f),
        1.0f);

    attribute->_age = 0.0f;
    attribute->_lifeTime = 2.0f; // lives for 2 seconds
}

void Firework::update(float timeDelta)
{
    std::list<Attribute>::iterator i;

    for (i = _particles.begin(); i != _particles.end(); i++)
    {
        // only update living particles
        if (i->_isAlive)
        {
            i->_position += i->_velocity * timeDelta;

            i->_age += timeDelta;

            if (i->_age > i->_lifeTime) // kill 
                i->_isAlive = false;
        }
    }
}

void Firework::preRender()
{
    ParticleSystem::preRender();

    (*ppDevice)->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    (*ppDevice)->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    // read, but don't write particles to z-buffer
    (*ppDevice)->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void Firework::postRender()
{
    ParticleSystem::postRender();

    (*ppDevice)->SetRenderState(D3DRS_ZWRITEENABLE, true);
}