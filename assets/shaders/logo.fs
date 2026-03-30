#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 viewSpacePosition;

uniform sampler2D MAIN_TEXTURE;
uniform sampler2D NOISE_TEXTURE;
uniform vec4 COLOR;
uniform mat4 VIEW;
uniform vec3 FOG_COLOR;
uniform float FOG_NEAR;
uniform float FOG_FAR;

void main()
{
   vec4 image = texture(MAIN_TEXTURE, TexCoord);
   vec3 targetPos = vec3(VIEW * vec4(300.0,300.0,-150.0, 1.0f));

   if (image.a == 0.0f)
      discard;

   vec2 noiseOffset = vec2(1.0f);
   vec2 noiseScale = vec2(5.0f);
   vec2 noiseUV = (TexCoord + noiseOffset) * noiseScale;
   float noisePower = 0.3;
   float n = texture(NOISE_TEXTURE, noiseUV).r;

   vec4 noise = vec4(vec3(n), 0.0f) * noisePower;

   vec3 hole;

   if (distance(viewSpacePosition, targetPos) < 40.0 + (n*20.0))
      hole = vec3(0.0);
   else
      hole = vec3(1.0);
   
   vec4 color = (image * COLOR) - noise;// * vec4(ourColor, 1.0);
   color *= vec4(hole, 1.0);

   float viewDepth = max(0.0f, -viewSpacePosition.z);
   float fogAmount = (viewDepth - FOG_NEAR) / (FOG_FAR - FOG_NEAR);
   fogAmount = clamp(fogAmount, 0.0f, 1.0f);
   vec3 foggedColor = mix(color.rgb, FOG_COLOR, fogAmount);

   FragColor = vec4(foggedColor, color.a);
}