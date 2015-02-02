// Skybox vertex shader for my final program

varying vec3 vPos;

void main()
{
   // Pass along vertex position
   float x;
   float y;
   float z;
   if (gl_Vertex.x > 0.0) x = 1.0;
   else x = -1.0;
   if (gl_Vertex.y > 0.0) y = 1.0;
   else y = -1.0;
   if (gl_Vertex.z > 0.0) z = 1.0;
   else z = -1.0;
   vPos = vec3(x,y,z);

   vec3 lightDir = vec3(0,0,0);

   // Pass the point
   gl_TexCoord[0] = gl_MultiTexCoord0;
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
