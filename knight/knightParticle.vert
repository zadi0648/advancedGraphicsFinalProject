// Vertex shader for when the knights are being drawn as bill-boarding particles.
// This part of the shader program makes sure the knight particles are drawn at a
// height to match the environment.

#version 420

in vec4 Position;

uniform sampler2D terrain;
uniform mat4 modelViewProjection;
uniform float heightAdjust; // SizeOfTerrian/HeightOfTerrian
uniform float landSize;

out vec4 vPosition;

void main()
{
   // Adjust the height of the particle to match the environment
   vec4 vertexPos = Position;
   vec2 texLoc = vec2(
      (vertexPos.x / landSize) + 0.5,
      1.0 - ((vertexPos.z / landSize) + 0.5)
      );
   vertexPos.y += texture2D(terrain, texLoc).r*heightAdjust;
   vPosition = vertexPos;
}
