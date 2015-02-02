// Fragment shader used on the model of the knight, except the chain mail part

const float dimMax = 2.0;

uniform sampler2D texture;

varying vec4 diffuse;
varying vec4 ambient;
varying vec3 normal;
varying vec3 halfVector;
 
void main()
{
   vec4 color; // Final color we will be returning
   vec3 norm; // Normal
   vec3 half; // Half vector
   vec3 lightDir; // Light direction
   float normDotProduct; // Light-normal dot product
   float normDotHalfProduct; // Normal-half vector dot product
   vec2 uv = gl_TexCoord[0].xy; // Texture coordiantes

   lightDir = vec3(normalize(gl_LightSource[0].position)); // Directional Light

   color = texture2D(texture, uv); // Start out with the ambient color
   norm = normalize(normal); // Normalize the normal

   normDotProduct = dot(norm, lightDir); // Get normal brightness
   if (normDotProduct > 0.0)
   {
      // Diffuse
      color += diffuse * normDotProduct;
      half = normalize(halfVector);
      // Specular
      normDotHalfProduct = max(dot(norm,half),0.0);
      color += gl_FrontMaterial.specular *
         gl_LightSource[0].specular *
         pow(normDotHalfProduct, gl_FrontMaterial.shininess);
   }
   // Otherwise dim depending on how opposing the normal & light vectors are
   else {
      color = color * (1.0 + normDotProduct/dimMax);
   }

   // Pass along final color
   gl_FragColor = color;
}