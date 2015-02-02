// Vertex shader used on chain mail parts of the knight model, which uses
// normal mapping to make the chain mail look more realisitic

varying vec4 Normal;
varying vec4 EyeVec;

uniform vec2 texLoc; // Texture location of this knight
uniform sampler2D terrain; // Terrian height map
uniform float heightAdjust; // Height map multiplier

void main()
{
   // Normal
   Normal = vec4((gl_NormalMatrix * gl_Normal), 1.0);
   vec4 view_vertex = gl_ModelViewMatrix * gl_Vertex;
   EyeVec = view_vertex;

   // Pass along UV
   gl_TexCoord[0] = gl_MultiTexCoord0;

   // Adjust the height of the model depending the terrian map
   vec4 pos = gl_Vertex;
   pos.y += texture2D(terrain, texLoc).r*heightAdjust;
   gl_Position = gl_ModelViewProjectionMatrix * pos;
}