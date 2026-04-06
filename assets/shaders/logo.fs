#version 330 core
out vec4 FragColor;

const int MAX_LIGHTS = 8;

in vec2 TexCoord;
in vec3 viewSpacePosition;
in vec3 fragPos;
in vec3 worldNormal;

struct Light
{
   vec3 position;
   vec3 color;
   float intensity;
   float ambientStrength;
   float specularStrength;
};

uniform sampler2D MAIN_TEXTURE;
uniform sampler2D NOISE_TEXTURE;
uniform vec4 COLOR;
uniform mat4 VIEW;
uniform int LIGHT_COUNT;
uniform Light LIGHTS[MAX_LIGHTS];
uniform vec3 VIEW_POS;
uniform vec3 FOG_COLOR;
uniform float FOG_NEAR;
uniform float FOG_FAR;

void main()
{
   vec4 image = texture(MAIN_TEXTURE, TexCoord);

   if (image.a == 0.0f)
      discard;

   // noise
   vec2 noiseOffset = vec2(1.0f);
   vec2 noiseScale = vec2(5.0f);
   vec2 noiseUV = (TexCoord + noiseOffset) * noiseScale;
   float noisePower = 0.3;
   float n = texture(NOISE_TEXTURE, noiseUV).r;

   vec4 noise = vec4(vec3(n), 0.0f) * noisePower;
   
   vec3 albedo = max(((image * COLOR) - noise).rgb, 0.0f);

   // lighting
   vec3 norm = normalize(worldNormal);
   vec3 viewDir = normalize(VIEW_POS - fragPos);
   vec3 lighting = vec3(0.05f);

   for (int i = 0; i < LIGHT_COUNT; i++)
   {
      vec3 lightColor = LIGHTS[i].color * LIGHTS[i].intensity;
      vec3 ambient = LIGHTS[i].ambientStrength * lightColor;

      vec3 lightDir = normalize(LIGHTS[i].position - fragPos);
      float diff = max(dot(norm, lightDir), 0.0f);
      vec3 diffuse = diff * lightColor;

      vec3 reflectDir = reflect(-lightDir, norm);
      float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
      vec3 specular = LIGHTS[i].specularStrength * spec * lightColor;

      lighting += ambient + diffuse + specular;
   }

   vec3 litColor = lighting * albedo;

   // fog
   float viewDepth = max(0.0f, -viewSpacePosition.z);
   float fogAmount = (viewDepth - FOG_NEAR) / (FOG_FAR - FOG_NEAR);
   fogAmount = clamp(fogAmount, 0.0f, 1.0f);
   vec3 foggedColor = mix(litColor, FOG_COLOR, fogAmount);

   FragColor = vec4(foggedColor, image.a * COLOR.a);
}
