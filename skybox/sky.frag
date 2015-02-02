// Skybox fragment shader for my final program

const vec3 highBlue = vec3(82.0/256.0, 101.0/256.0, 148.0/256.0);
const vec3 lowBlue = vec3(182.0/256.0, 189.0/256.0, 207.0/256.0);
const vec3 sunColor = vec3(1.00,0.98,0.92);
const float skyScale = 250.0;
const vec3 up = vec3(0,1,0);
const float mixRate = 0.85;
const float blurJump = 1.0/1024.0;
const float sunSize = 0.03;
const float sunHeight = 0.325;
const float sunMixSize = 0.1;

varying vec3 vPos;

uniform sampler2D skyTexture;
uniform float sunAngle;

// From this pixel texture's coordinate we can tell how close this pixel is to
// the sun's center, but since the texture used isn't square we need to 'warp'
// the x distance by four
float getWarpedDist(float x, float y, float sunX) {
   return sqrt(
         (((x-sunX)*(x-sunX))*4.0) +
         ((y-sunHeight)*(y-sunHeight))
   );
}

void main() {
   vec3 finalColor; // The final resulting color
   vec2 texCoord = gl_TexCoord[0].xy; // Store the texture coordinate

   // ----- SUN -----
   // Calculate where the sun should be in our skybox in terms of the texture
   // coordiantes
   float sunPos;
   float sA = -sunAngle + 45.0 + 180.0;
   sunPos = mod((sA/360.0), 1.0);
   float d1; // Normal distance
   float d2; // 'Wrapped' distance
   float sunDist; // This pixel's distance from the sun
   // Since at one corner the sun would otherwise half-disappear,
   // we need to make it so the distance can 'warp' around the textures edge
   float wrap;
   if (sunPos > 0.5) wrap = -1.0;
   else wrap = +1.0;
   d1 = getWarpedDist(texCoord.x, texCoord.y, sunPos);
   d2 = getWarpedDist(texCoord.x, texCoord.y, sunPos + wrap);
   sunDist = min(d1, d2); // This pixel's final distance from the center of the sun

   // ----- BACK GROUND BLUE -----
   // Determine what shade of blue the background would be here
   vec3 p = normalize(vPos);
   float blueMixAmount = dot(up, p);
   vec3 bgColor = mix(lowBlue, highBlue, blueMixAmount);
   // Apply changes from the sun
   // Inside sun? If so, use sun color
   if (sunDist < sunSize) bgColor = sunColor;
   else if (sunDist < sunMixSize) bgColor = mix(sunColor, bgColor, sunDist/sunMixSize);

   // ----- CLOUD -----
   vec3 cloudColor = texture2D(skyTexture, texCoord).rgb;
   // Use the whiteness of the pixel here to determine if we should the bg color
   // or color from the texture
   float cloudMixAmount = cloudColor.r;
   // Calculate how much we should mix
   if (cloudMixAmount < mixRate) cloudMixAmount = 0.0;
   else cloudMixAmount = 1.0 - ((1.0 - cloudMixAmount)/(1.0 - mixRate));
   // Blur the cloud to make it look more like a cloud
   cloudColor = (cloudColor*0.2)
      + (texture2D(skyTexture, gl_TexCoord[0].xy + vec2(+blurJump, 0.0)).rgb * 0.2)
      + (texture2D(skyTexture, gl_TexCoord[0].xy + vec2(-blurJump, 0.0)).rgb * 0.2)
      + (texture2D(skyTexture, gl_TexCoord[0].xy + vec2(0.0, +blurJump)).rgb * 0.2)
      + (texture2D(skyTexture, gl_TexCoord[0].xy + vec2(0.0, -blurJump)).rgb * 0.2);
   sunDist = max(min(1.0, sunDist), 0.0);
   cloudColor += (vec3(0.10,0.08,0.08))*(sunMixSize/sunDist);

   // ----- FINAL -----
   // Finally, mix the bg & cloud colors appropriately to get the final color
   finalColor = mix(cloudColor, bgColor, cloudMixAmount);
   gl_FragColor = vec4(finalColor, 1.0);
}

