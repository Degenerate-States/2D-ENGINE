#version 410


uniform bool isPaused;
uniform float time;
in vec4 pos;
out vec4 color;

void main ()  
{    
   if (isPaused == true)
      color = vec4(0.4, 0, 0.9, 0.9);
   else
      color = vec4(time, 1-time, 10/distance(vec2(0.0), pos.xy)*(1-time), time/5 + 0.8);
}