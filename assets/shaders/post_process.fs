#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D SCREEN_TEXTURE;
uniform float TIME;
uniform vec2 SCREEN_SIZE;

const float BLOOM_THRESHOLD = 0.5;
const float BLOOM_SOFTNESS = 0.50;
const float BLOOM_INTENSITY = 10.9;

const float VIGNETTE_INNER = 0.8;
const float VIGNETTE_OUTER = 1.0;
const float VIGNETTE_STRENGTH = 0.3;
const vec3 VIGNETTE_TINT_COLOR = vec3(0.55, 0.42, 0.32);

vec3 BrightPass(vec2 uv)
{
   vec3 color = texture(SCREEN_TEXTURE, uv).rgb;
   float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
   float bloomMask = smoothstep(BLOOM_THRESHOLD, BLOOM_THRESHOLD + BLOOM_SOFTNESS, brightness);
   return color * bloomMask;
}

vec3 SimpleBloom(vec2 uv)
{
   vec2 texel = 1.0 / max(SCREEN_SIZE, vec2(1.0));
   vec3 bloom = BrightPass(uv) * 0.20;

   bloom += BrightPass(uv + texel * vec2( 1.5,  0.0)) * 0.10;
   bloom += BrightPass(uv + texel * vec2(-1.5,  0.0)) * 0.10;
   bloom += BrightPass(uv + texel * vec2( 0.0,  1.5)) * 0.10;
   bloom += BrightPass(uv + texel * vec2( 0.0, -1.5)) * 0.10;

   bloom += BrightPass(uv + texel * vec2( 2.5,  2.5)) * 0.075;
   bloom += BrightPass(uv + texel * vec2(-2.5,  2.5)) * 0.075;
   bloom += BrightPass(uv + texel * vec2( 2.5, -2.5)) * 0.075;
   bloom += BrightPass(uv + texel * vec2(-2.5, -2.5)) * 0.075;

   bloom += BrightPass(uv + texel * vec2( 5.0,  0.0)) * 0.035;
   bloom += BrightPass(uv + texel * vec2(-5.0,  0.0)) * 0.035;
   bloom += BrightPass(uv + texel * vec2( 0.0,  5.0)) * 0.035;
   bloom += BrightPass(uv + texel * vec2( 0.0, -5.0)) * 0.035;

   return bloom;
}

float VignetteAmount(vec2 uv)
{
   vec2 centered = uv * 2.0 - 1.0;
   centered.x *= SCREEN_SIZE.x / max(SCREEN_SIZE.y, 1.0);

   float dist = length(centered);
   float edge = smoothstep(VIGNETTE_INNER, VIGNETTE_OUTER, dist);
   return edge * VIGNETTE_STRENGTH;
}

void main()
{
   vec3 color = texture(SCREEN_TEXTURE, TexCoord).rgb;
   color += SimpleBloom(TexCoord) * BLOOM_INTENSITY;

   float vignette = VignetteAmount(TexCoord);
   color = mix(color, color * VIGNETTE_TINT_COLOR, vignette);

   FragColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}
