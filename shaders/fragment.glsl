#version 330

uniform sampler2D texture0;

in vec2 fragTexCoord;
in vec2 fragPosition;

uniform vec2 lightPosition;

out vec4 finalColor;

const float MIN_BRIGHTNESS = 0.05; // avoid total darkness

float attenuate(float distance, float linearTerm, float quadraticTerm)
{
    float attenuation = 1.0 / (1.0 + 
                               linearTerm * distance + 
                               quadraticTerm * distance * distance);
                               
    return max(attenuation, MIN_BRIGHTNESS);
}

// Adjustable attenuation parameters
const float LINEAR_TERM    = 0.00003; // linear term
const float QUADRATIC_TERM = 0.00003; // quadratic term


void main()
{
    float distance   = distance(lightPosition, fragPosition);
    float brightness = attenuate(distance, LINEAR_TERM, QUADRATIC_TERM);
    vec4 color = texture(texture0, fragTexCoord);
    finalColor = vec4(color.rgb * brightness, color.a);
}