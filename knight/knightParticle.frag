// Fragment shader for when the knights are being drawn as bill-boarding particles.
// Use green as a chroma key to make the rest of the quad that doesn't feature the
// knight model invisible.

#version 420

uniform sampler2D knightSprite;

in vec4 texCoord;// Texture coordinates
out vec4 fragColor;

void main()
{
   
   // Get color from texture
   vec4 color = texture2D(knightSprite, texCoord.st);

   // Set to invisible if chroma key
   float err = 0.075;
   if ((color.g > color.r+err) && (color.g > color.b+err)) color.a = (1.0 - color.g);

   // Pass color
   fragColor = color;
}
