#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 viewSpacePosition;

uniform sampler2D MAIN_TEXTURE;
uniform vec4 COLOR;
uniform mat4 VIEW;
uniform vec3 FOG_COLOR;
uniform float FOG_NEAR;
uniform float FOG_FAR;

void main()
{
   vec4 image = texture(MAIN_TEXTURE, TexCoord);

   if (image.a == 0.0f)
      discard;
   
   vec4 color = image * COLOR;
   float viewDepth = max(0.0f, -viewSpacePosition.z);
   float fogAmount = (viewDepth - FOG_NEAR) / (FOG_FAR - FOG_NEAR);
   fogAmount = clamp(fogAmount, 0.0f, 1.0f);
   vec3 foggedColor = mix(color.rgb, FOG_COLOR, fogAmount);

   FragColor = vec4(foggedColor, color.a);
}