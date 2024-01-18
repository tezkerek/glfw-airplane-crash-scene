#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 inLightPos;
in vec3 inViewPos;
in vec3 dir;
in vec3 ex_Color;
in vec2 TexCoords;

out vec4 out_Color;

uniform vec3 lightColor;
uniform int codCol;

uniform sampler2D texture_diffuse1;

vec3 computeLight() {
    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 normala = normalize(Normal);
    vec3 lightDir = normalize(inLightPos - FragPos);
    // vec3 lightDir = normalize(dir); // cazul unei surse directionale
    float diff = max(dot(normala, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(
        inViewPos - FragPos); // vector catre observator normalizat (V)
    vec3 reflectDir =
        reflect(-lightDir, normala); // reflexia razei de lumina (R)
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
    vec3 specular = specularStrength * spec * lightColor;

    return ambient + diffuse + specular;
}

void main(void) {
    vec3 emission = vec3(0.0, 0.0, 0.0);
    float distance = length(inViewPos - FragPos);

    if (codCol == 0) {
        // Ambient
        vec3 light = computeLight();
        vec3 result = emission + light * ex_Color;

        vec3 fogColor = vec3(0.5f, 0.5f, 0.5f);
        vec3 withFog = mix(fogColor, result, exp(-0.0001f * distance));

        out_Color = vec4(withFog, 1.0f);
    } else if (codCol == 1) {
        vec3 black = vec3(0.0, 0.0, 0.0);
        vec3 fogColor = vec3(0.4f, 0.4f, 0.4f);
        vec3 withFog = mix(fogColor, black, exp(-0.0001f * distance));
        out_Color = vec4(withFog, 1.0);
    } else if (codCol == 2) {
        vec4 tex = texture(texture_diffuse1, TexCoords);
        vec3 light = computeLight();
        vec3 result = emission + light * tex.xyz;
        out_Color = vec4(result, tex.w);
    }
}
