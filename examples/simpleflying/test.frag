varying vec3 n;
varying vec3 l;
varying vec3 pos;

uniform sampler2D groundTexture;
uniform vec3 cameraPos;
uniform vec4 fogColor;

vec3 calcDirLightColor(vec3 l, vec3 n, vec3 lightcolor, float lighta, float lightd)
{
    float ndotl = max(0.0, dot(n, l));
    return lightcolor * (lighta + ndotl*lightd);
}

vec3 calcPointLightColor(vec3 l, vec3 n, float radius, vec3 lightcolor, float lighta, float lightd)
{
    vec3 lightToPos = l - pos;
    vec3 lightdir = normalize(lightToPos);
    float dist = length(lightToPos);

    float ndotl = max(0.0, dot(n, lightdir));
    return lightcolor * (lighta + ndotl*lightd) * max(0.0, 1.0 - dist / radius);
}

void main()
{
//     vec3 lightpos = vec3(lightX, lightY, lightZ);
    vec3 lightDir = normalize(l);
    vec3 l2 = vec3(600.0, -15.0, -1200.0) - vec3(-2500.0, 0.0, -2500.0);
    vec3 normal = normalize(n);

    vec3 finalcolor = calcDirLightColor(lightDir, normal, vec3(1.0, 1.0, 1.0), 0.4, 0.6);
//     finalcolor += calcPointLightColor(l2, n, 200.0, vec3(1.5, 1.2, 0.6), 1.5, 3.0);

    vec3 tex = texture2D(groundTexture, vec2(pos.x, pos.z) / 50.0).rgb;

    float fog = min(1.0, length(cameraPos - pos) / 5000.0);

    gl_FragColor = vec4(mix(finalcolor * tex, fogColor.rgb, fog), 1.0);
}
