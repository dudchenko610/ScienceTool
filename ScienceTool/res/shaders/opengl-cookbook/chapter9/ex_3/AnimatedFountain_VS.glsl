#version 430

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexVelocity;
layout (location = 2) in float VertexStartTime;
layout (location = 3) in vec3 VertexInitialVelocity;

out      vec3   Position;         // To transform feedback
out      vec3   Velocity;         // To transform feedback
out      float  StartTime;        // To transform feedback

out      float  Transp;           // To fragment shader

uniform  float  Time;             // Simulation time
uniform  float  H;                // Elapsed time between frames
uniform  vec3   Accel;            // Particle acceleration
uniform  float  ParticleLifetime; // Particle lifespan

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;


subroutine (RenderPassType)
void update() {

    Position = VertexPosition;
    Velocity = VertexVelocity;
    StartTime = VertexStartTime;

    if( Time >= StartTime ) {

        float age = Time - StartTime;

        if( age > ParticleLifetime ) 
        {
            // The particle is past its lifetime, recycle.
            Position = vec3(0.0);
            Velocity = VertexInitialVelocity;
            StartTime = Time;
        } 
        else 
        {
            // The particle is alive, update.
            Position += Velocity * H;
            Velocity += Accel * H;
        }
    }
}

subroutine (RenderPassType)
void render() {

    float age = Time - VertexStartTime;
    Transp = 1.0 - age / ParticleLifetime;
    gl_Position = u_Projection * u_View * u_Model * vec4(VertexPosition, 1.0);

}

void main()
{
    // This will call either render() or update()
    RenderPass();
}