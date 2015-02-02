// Geometry shader for when the knights are being drawn as bill-boarding particles.
// Creates a billboarding particle that the knight image is drawn in.

#version 420

#extension GL_EXT_geometry_shader4 : enable

// Convert point into quad created from triangle strip
layout(points) in;
layout(triangle_strip,max_vertices=4) out;

uniform mat4 modelViewProjection;
uniform float aspect;
in vec4 vPosition[1];
out vec4 texCoord;

void billboard(float x, float y, int s, int t)
{
   // Set texture coordinates
   texCoord = vec4(s,t,0,1);

   // Determine position
   vec2 delta = vec2(x,y);
   vec4 p = vPosition[0];
   p.x += dot(delta, modelViewProjection[0].xy);
   p.y += dot(delta, modelViewProjection[1].xy);
   p.z += dot(delta, modelViewProjection[2].xy);

   // Set the position
   gl_Position = modelViewProjection*p;

   // Emit new vertex
   EmitVertex();
}

// Create the basic billboarding quad
void main()
{
   float size = 0.55;
   // Bottom left
   billboard(-size*aspect, -size, 0,0);
   // Bottom right
   billboard(+size*aspect, -size, 1,0);
   // Top left
   billboard(-size*aspect, +size, 0,1);
   // Top right
   billboard(+size*aspect, +size, 1,1);
   EndPrimitive(); // Done with shape
}
