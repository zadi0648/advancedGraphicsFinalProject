// Draw grey triangles with red outlines to display how the environment is tessellating.

#version 420

//  Colors
const vec3 AmbientMaterial = vec3(0.05,0.05,0.05);
const vec3 line = vec3(1.0,0.0,0.0);
// Normal
in vec3 gFacetNormal;
// Distance to edge of patch and triange
in vec3 gTriDistance;
in vec4 gPatchDistance;
// Output color
out vec4 FragColor;

void main()
{
   // Set everything to intial ambient color
   vec3 color = AmbientMaterial;

   // Draw mesh in line, heavy for patches, light for triangles
   float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
   float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
   color = mix(line, color, step(0.02,d1) * step(0.01,d2));

   // Pixel color
   FragColor = vec4(color, 1.0);
}
