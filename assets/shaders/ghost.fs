#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 viewSpacePosition;

uniform sampler2D MAIN_TEXTURE;
uniform vec4 COLOR;
uniform mat4 VIEW;

void main()
{
   vec4 image = texture(MAIN_TEXTURE, TexCoord);

   if (image.a == 0.0f)
      discard;
   
   FragColor = image * COLOR;
}