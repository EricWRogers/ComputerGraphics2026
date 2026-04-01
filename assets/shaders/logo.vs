#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 viewSpacePosition;
out vec3 fragPos;
out vec3 worldNormal;

uniform float TIME;
uniform mat4 TRANSFORM;
uniform mat4 PROJECTION;
uniform mat4 VIEW;
void main()
{
   fragPos = vec3(TRANSFORM * vec4(aPos, 1.0f));
   viewSpacePosition = vec3(VIEW * vec4(fragPos, 1.0f));

   if (length(aNormal) < 0.001f)
   {
      vec3 worldRight = vec3(TRANSFORM * vec4(1.0f, 0.0f, 0.0f, 0.0f));
      vec3 worldUp = vec3(TRANSFORM * vec4(0.0f, 1.0f, 0.0f, 0.0f));
      worldNormal = normalize(cross(worldRight, worldUp));
   }
   else
   {
      mat3 normalMatrix = mat3(transpose(inverse(TRANSFORM)));
      worldNormal = normalize(normalMatrix * aNormal);
   }

   gl_Position = PROJECTION * VIEW * vec4(fragPos, 1.0f);
   TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
