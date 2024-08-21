#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "raylib.h"
#include <vector>
#include <functional>

#include "raylib.h"
#include <vector>

// Particle structure
struct Particle {
    Vector2 position;
    Vector2 velocity;
    float alpha;    // Transparency (1.0 is fully visible, 0.0 is invisible)
    float size;
    float lifeTime; // How long the particle lives
    Color color;
};

// Particle System class
class ParticleSystem {
public:
    // Constructor
    ParticleSystem();

    // Adds a particle to the system
    void AddParticle(const Particle& particle);

    // Updates all particles in the system
    void Update(float deltaTime);

    // Draws all particles in the system
    void Draw() const;

private:
    std::vector<Particle> particles;
};

#endif // PARTICLESYSTEM_H
