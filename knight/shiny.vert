// Fragment shader used on the model of the knight, except the chain mail part

varying vec4 diffuse;
varying vec4 ambient;
varying vec3 normal;
varying vec3 halfVector;

uniform sampler2D terrain;
uniform vec2 texLoc;
uniform float heightAdjust;

void main()
{
   // Get the normal, don't bother normalizing because that will be done if the
   // fragment shader
   normal = gl_NormalMatrix * gl_Normal;
 
   // Pass along half vector
   halfVector = gl_LightSource[0].halfVector.xyz;
 
   diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse; // Diffuse
   ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient; // Ambient
   ambient += gl_FrontMaterial.ambient * gl_LightModel.ambient; // Global ambient

   // Pass along UV
   gl_TexCoord[0] = gl_MultiTexCoord0;

   // Adjust the height of the model depending the terrian map
   vec4 vertexPos = gl_Vertex;
   vertexPos.y += texture2D(terrain, texLoc).r*heightAdjust;
   gl_Position = gl_ModelViewProjectionMatrix * vertexPos;
}