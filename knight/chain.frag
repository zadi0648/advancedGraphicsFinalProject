// Fragment shader used on chain mail parts of the knight model, which uses
// normal mapping to make the chain mail look more realisitic

uniform sampler2D chainText; // Color map
uniform sampler2D chainBump; // Normal map

varying vec4 Normal;
varying vec4 EyeVec;

const vec4 lightDiffuse = vec4(0.9,0.9,0.9,1.0);
const vec4 lightSpecular = vec4(0.1,0.1,0.1,1.0);
const vec4 materialDiffuse = vec4(0.64, 0.64, 0.64, 1.00);
const vec4 materialSpecular = vec4(0.00, 0.00, 0.00, 1.00);
const float materialShininess = 14.0;

// From: http://www.thetenthplanet.de/archives/1180
mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
{
   // Screen-space derivatives
   // get edge vectors of the pixel triangle
   vec3 dp1 = dFdx(p);
   vec3 dp2 = dFdy(p);
   vec2 duv1 = dFdx(uv);
   vec2 duv2 = dFdy(uv);
 
   // Solve the linear system
   vec3 dp2perp = cross( dp2, N );
   vec3 dp1perp = cross( N, dp1 );
   vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
   vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
   // Construct a scale-invariant frame 
   float invmax = inversesqrt(max(dot(T,T), dot(B,B)));
   return mat3(T*invmax, B*invmax, N);
}

void main()
{

   vec2 uv = gl_TexCoord[0].xy;
   vec3 lightDir;

   // Get the light direction
   lightDir = normalize(vec3(gl_LightSource[0].position));
  
   // Normalize the inputs
   vec3 N = normalize(Normal.xyz);
   vec3 L = normalize(lightDir.xyz);
   vec3 V = normalize(EyeVec.xyz);

   // Perturb normal from the text file
   const float unsignedFix = (255.0/127.0);
   vec3 map = texture2D(chainBump, uv).xyz;
   map = (map*(255.0/127.0)) - 128.0/127.0; // 'Normalize' the values given from the bump map

   // Get the cotangent frame
   mat3 cotangFrame = cotangent_frame(N, -V, uv);
   // Use the cotangent frame to preturb the normal
   vec3 perturbedNorm = normalize(cotangFrame * map);

   vec4 chainText1_color = texture2D(chainText, uv);
   vec4 final_color = vec4(0.2,0.15,0.15,1.0) * chainText1_color; 

   float lambertTerm = dot(perturbedNorm, L);
   if (lambertTerm > 0.0)
   {
      final_color += lightDiffuse * materialDiffuse * lambertTerm * chainText1_color;  
   
      vec3 E = normalize(EyeVec.xyz);
      vec3 R = reflect(-L, perturbedNorm);
      float specular = pow( max(dot(R, E), 0.0), materialShininess);
      
      final_color += lightSpecular * materialSpecular * specular;  
   }

   // Pass along final color
   gl_FragColor = vec4(final_color.rgb, 1.0);
}