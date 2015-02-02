// Draw the environment depending in standard terrian form! Use different textures depending
// on the height of the vertex passed in from the evaluation shader. The height number
// is altered by a noise map so that the same texture does not always start at the same
// height (giving it a more relastic)

#version 420

// How much tiling is done for each texture
const float grassTile = 25.0;
const float rockTile = 37.5;
const float roadTile = 40.0;
// Minimal heights for a texture to occur (remember that height variation is very little!)
const float rockHeight = 0.05;
const float snowHeight = 0.12;
// How suddenly elevation causes a change from one texture to another
const float grassToRock = 0.01;
const float rockToSnow = 0.001;

const float ambient = 1.0; // i.e. max darkness of normals
const float dimMult = 1.35;

uniform sampler2D terrain; // Heightmap
// Textures used on the environment
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D road;
uniform vec3 lightDirection;

in vec2 texcoord; // Coordinate of this pixel
in float height; // The elevation of this pixel
in float roadVal; // Is there a road here?
in vec3 normal; // Normal of this vertex

out vec4 fragment;

void main() {
   vec3 norm = normalize(normal);
   vec3 lightDir = normalize(lightDirection);

   // Textures colors
   vec3 grassCol = texture(grass, texcoord*grassTile).rgb;
   vec3 rockCol = texture(rock, texcoord*rockTile).rgb;
   vec3 snowCol = vec3(1,1,1);
   vec3 roadCol = texture(road, texcoord*roadTile).rgb;

   vec3 color = vec3(0,0,0);
   float mixAmount;

   // Grass
   if (height < rockHeight) {
      color = grassCol;
   }
   // Rock
   else if (height < snowHeight) {
      mixAmount = (height - rockHeight)/(grassToRock);
      mixAmount = min(max(0.0, mixAmount), 1.0); // normalize
      color = mix(grassCol, rockCol, mixAmount);
   }
   // Snow
   else {
      mixAmount = (height - snowHeight)/(rockToSnow);
      mixAmount = min(max(0.0, mixAmount), 1.0); // normalize
      color = mix(rockCol, snowCol, mixAmount);
   }
   
   // Apply road
   color = mix(color, roadCol, roadVal);

   // Apply normal dim
   float dim = dot(lightDir, norm);
   dim = min(1.0, dimMult*dim);
   color = color*dim;

   // Fragment color out
   fragment = vec4(color, 1.0);
}

