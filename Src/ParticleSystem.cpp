#include "ParticleSystem.h"
#include <algorithm>

// Constructor
ParticleSystem::ParticleSystem() {
   
}

// Adds a particle to the system
void ParticleSystem::AddParticle(const Particle& particle) {
    particles.push_back(particle);
}

// Updates all particles in the system
void ParticleSystem::Update(float deltaTime) {
    for (auto& particle : particles) {
        particle.position.x += particle.velocity.x * deltaTime;
        particle.position.y += particle.velocity.y * deltaTime;
        particle.lifeTime -= deltaTime;
        particle.alpha -= deltaTime / particle.lifeTime;

        if (particle.alpha < 0) particle.alpha = 0;
    }

    // Remove particles that are no longer visible or have no lifetime left
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](Particle& p) { return p.alpha <= 0 || p.lifeTime <= 0; }),
        particles.end());
}

// Draws all particles in the system
void ParticleSystem::Draw() const {
    for (const auto& particle : particles) {
        Color drawColor = Fade(particle.color, particle.alpha);
        DrawCircleV(particle.position, particle.size, drawColor);
    }
}
