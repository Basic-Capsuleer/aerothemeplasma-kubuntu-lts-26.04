#version 140

#include "colormanagement.glsl"

uniform sampler2D texUnit;
uniform float opacity;
uniform float translate;
uniform vec2 screenResolution;
uniform vec2 windowSize;
uniform mat4 colorMatrix;
uniform vec2 windowPos;

// Glow
uniform vec2 textureSize;
uniform bool useWayland;
uniform sampler2D glowTexture;
uniform bool glowEnable;
uniform float glowOpacity;

in vec2 uv;

out vec4 fragColor;

vec4 glowFragment()
{
    float xpos = clamp((gl_FragCoord.x - windowPos.x) / windowSize.x, 0, 1);

    float t_x = uv.x;

    // Distance from the window's top edge, from the window's own coordinate (uv.y is 1 at the top).
    // gl_FragCoord depends on how each screen's buffer is stored, which differs between screens.
    float t_y = clamp(windowSize.y * (1.0 - uv.y) / textureSize.y, 0, 1);
    vec2 t_uv = vec2(windowSize.x * t_x / textureSize.x, t_y);

    vec4 result = texture2D(glowTexture, t_uv) * glowOpacity;

    // Grab from the other side as well
    t_x = 1 - uv.x;
    t_uv = vec2(windowSize.x * t_x / textureSize.x, t_y);
    result += texture2D(glowTexture, t_uv) * glowOpacity;
    return result;
}

void main(void)
{
    float middleLine = windowPos.x + windowSize.x / 2.0;
    float middleScreenLine = screenResolution.x / 2.0;
    float dx = translate * (middleScreenLine - middleLine) / 10.0;

    float x = (gl_FragCoord.x + dx) / screenResolution.x;
    float y = (gl_FragCoord.y) / screenResolution.y;

    vec2 t_uv;
    if(useWayland) {
        t_uv = vec2(x, y);
    } else {
        t_uv = vec2(x, -y);
    }

    vec4 result = texture(texUnit, t_uv) * opacity;
    //result.a *= opacity;
    if(glowEnable)
    {
        result += glowFragment();
    }
    fragColor = result;

    fragColor *= colorMatrix;
    //fragColor.a = fragColor.a * opacity;

    // The reflection and glow images are sRGB; convert them to the screen's encoding, which KWin
    // sets per screen (based on upstream commit 25e54b4 for Plasma 6.7).
    fragColor = sourceEncodingToNitsInDestinationColorspace(fragColor);
    fragColor = nitsToDestinationEncoding(fragColor);
}
