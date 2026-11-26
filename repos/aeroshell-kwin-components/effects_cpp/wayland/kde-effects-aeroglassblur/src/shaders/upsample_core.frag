#version 140

#include "colormanagement.glsl"

uniform sampler2D texUnit;
uniform float offset;
uniform vec2 halfpixel;
// Set on the last pass: convert the background from the screen's encoding to sRGB, so the tint
// pass can work in sRGB (as upstream commit 25e54b4 does for Plasma 6.7).
uniform bool applyInverseColorTransform;

in vec2 uv;

out vec4 fragColor;

void main(void)
{
    vec2 texSize = halfpixel.xy;
    vec2 t_uv = (floor(gl_FragCoord.xy) + vec2(0.5, 0.5)) * texSize;
    vec4 sum = texture(texUnit, t_uv + vec2(-halfpixel.x * 2.0, 0.0) * offset);
    sum += texture(texUnit, t_uv + vec2(-halfpixel.x, halfpixel.y) * offset) * 2.0;
    sum += texture(texUnit, t_uv + vec2(0.0, halfpixel.y * 2.0) * offset);
    sum += texture(texUnit, t_uv + vec2(halfpixel.x, halfpixel.y) * offset) * 2.0;
    sum += texture(texUnit, t_uv + vec2(halfpixel.x * 2.0, 0.0) * offset);
    sum += texture(texUnit, t_uv + vec2(halfpixel.x, -halfpixel.y) * offset) * 2.0;
    sum += texture(texUnit, t_uv + vec2(0.0, -halfpixel.y * 2.0) * offset);
    sum += texture(texUnit, t_uv + vec2(-halfpixel.x, -halfpixel.y) * offset) * 2.0;

    sum /= 12.0;

    if (applyInverseColorTransform) {
        sum = sourceEncodingToNitsInDestinationColorspace(sum);
        sum = nitsToDestinationEncoding(sum);
    }
    fragColor = sum;
}
