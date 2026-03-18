#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 worldSpacePosition;

uniform float TIME;
uniform mat4 TRANSFORM;
uniform mat4 PROJECTION;
uniform mat4 VIEW;

void main()
{
   worldSpacePosition = vec3(VIEW * TRANSFORM * vec4(aPos, 1.0f));
   gl_Position = PROJECTION * VIEW * TRANSFORM * vec4(aPos, 1.0f);
   ourColor = abs(vec3(gl_Position));
   TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}