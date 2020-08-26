#shader compute
#version 430

layout( local_size_x = 1000 ) in;

uniform float G1 = 1000.0;
uniform vec3 attractor1_pos;

uniform float G2 = 1000.0;
uniform vec3 attractor2_pos;

const uniform float ParticleMass = 0.1;
uniform float ParticleInvMass = 1.0 / 0.2;
uniform float dt = 0.0005;
uniform float MaxDist = 15.0;

layout(std430, binding = 0) buffer Pos {
  vec4 p[];
};
layout(std430, binding = 1) buffer Vel {
  vec4 v[];
};

void main()
{
    uint i = gl_GlobalInvocationID.x;       // compute shader is 1 dimmensional, so particles accessed via its Global ID in the x
    vec3 particle_pos = p[i].xyz;           // access this particles position data
    vec3 force = vec3(0.0, 0.0, 0.0);
                              
    vec3 d = attractor1_pos - particle_pos; // direction to black hole from particle  
    float r1 = length(d);                   // get magnitude of unnormalized direction vector
    force += (G1/r1) * normalize(d);        // calculate gravitational force

    d = attractor2_pos - particle_pos;      // repeat for other attractor
    float r2 = length(d);
    force += (G2/r2) * normalize(d);

    if( r1 > MaxDist && r2 > MaxDist) { // Reset positions of particles that get too far from the attractors
        p[i] = vec4(0,0,0,1);
    }
    else { // Apply simple Euler integrator
        vec3 a = force * ParticleInvMass;
        p[i] = vec4( particle_pos + v[i].xyz * dt + 0.5*a*dt*dt, 1.0);
        v[i] = vec4( v[i].xyz + a * dt, 0.0);
     }
}
#end