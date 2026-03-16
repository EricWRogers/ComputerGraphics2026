#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 worldSpacePosition;

uniform sampler2D MAIN_TEXTURE;
uniform sampler2D NOISE_TEXTURE;
uniform vec4 COLOR;

void main()
{
   vec4 image = texture(MAIN_TEXTURE, TexCoord);
   vec3 targetPos = vec3(300.0,300.0,-150.0);

   if (image.a == 0.0f)
      discard;

   vec2 noiseOffset = vec2(1.0f);
   vec2 noiseScale = vec2(5.0f);
   vec2 noiseUV = (TexCoord + noiseOffset) * noiseScale;
   float noisePower = 0.3;
   float n = texture(NOISE_TEXTURE, noiseUV).r;

   vec4 noise = vec4(vec3(n), 0.0f) * noisePower;

   vec3 hole;

   if (distance(worldSpacePosition, targetPos) < 40.0 + (n*20.0))
      hole = vec3(0.0);
   else
      hole = vec3(1.0);
   
   FragColor = (image * COLOR) - noise;// * vec4(ourColor, 1.0);
   FragColor *= vec4(hole, 1.0);
}