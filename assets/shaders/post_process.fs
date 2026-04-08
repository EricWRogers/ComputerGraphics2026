#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D SCREEN_TEXTURE;
uniform float TIME;
uniform vec2 SCREEN_SIZE;

void main()
{
   FragColor = texture(SCREEN_TEXTURE, TexCoord);
}
