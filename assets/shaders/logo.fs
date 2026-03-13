#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D MAIN_TEXTURE;
uniform sampler2D NOISE_TEXTURE;
uniform vec4 COLOR;

void main()
{
   vec4 image = texture(MAIN_TEXTURE, TexCoord);

   if (image.a == 0.0f)
      discard;

   vec2 noiseOffset = vec2(1.0f);
   vec2 noiseScale = vec2(5.0f);
   vec2 noiseUV = (TexCoord + noiseOffset) * noiseScale;
   float noisePower = 0.3;

   vec4 noise = vec4(vec3(texture(NOISE_TEXTURE, noiseUV).r), 0.0f) * noisePower;
   
   FragColor = (image * COLOR) - noise;// * vec4(ourColor, 1.0);
   //FragColor = noise;
}