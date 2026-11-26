#version 140

#include "colormanagement.glsl"

uniform sampler2D texUnit;
uniform float offset;
uniform vec2 halfpixel;

uniform float aeroColorR;
uniform float aeroColorG;
uniform float aeroColorB;
uniform float aeroColorA;
uniform float aeroColorBalance;
uniform float aeroAfterglowBalance;
uniform float aeroBlurBalance;

uniform mat4 colorMatrix;

in vec2 uv;

out vec4 fragColor;

void main(void)
{
    vec4 color = vec4(aeroColorR, aeroColorG, aeroColorB, aeroColorA);
    vec4 baseColor = vec4(0.871, 0.871, 0.871, 1.0 - aeroColorA);
    fragColor = vec4(color.r * color.a + baseColor.r * baseColor.a,
                     color.g * color.a + baseColor.g * baseColor.a,
                     color.b * color.a + baseColor.b * baseColor.a, 1.0);
    fragColor *= colorMatrix;

    // The tint is worked out in sRGB; convert the result to the screen's encoding, which KWin sets
    // per screen (as upstream commit 25e54b4 does for Plasma 6.7).
    fragColor = sourceEncodingToNitsInDestinationColorspace(fragColor);
    fragColor = nitsToDestinationEncoding(fragColor);
}
