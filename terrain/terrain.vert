// Vertex shader for our environment shader program. Doesn't change anything.

#version 420

//  Coordinates in and out
in  vec4 Position;
out vec3 vPosition;

uniform sampler2D terrain;
uniform float heightFactor;

void main()
{
   // Coordinate passthrough
   vPosition = Position.xyz;
}
