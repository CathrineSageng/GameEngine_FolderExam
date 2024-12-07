#include "Particle.h"

void Particle::update(float deltaTime, float groundHeight) {
    if (!active) return; // Hvis partiklen er inaktiv, gjør ingenting

    // Oppdater posisjon basert på hastighet og tyngdekraft
    velocity.z -= 9.81f * deltaTime; // Tyngdekraft i y-retning
    position += velocity * deltaTime;

    // Hvis partiklen treffer bakken (B-spline flate), stopp den
    if (position.z <= groundHeight) {
        position.z = groundHeight;
        active = false;
    }
}
