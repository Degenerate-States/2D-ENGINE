#version 410

uniform bool isPaused;
uniform float time;
in vec4 position;
out vec4 frag_pos;

void main()
{
   gl_Position = position;
   frag_pos = position;
}