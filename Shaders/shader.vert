#version 460
in vec3 pos;

uniform mat4 MVP;

out vec4 colorParticle;

void main()
{
  gl_Position = MVP*(vec4( pos.x, pos.y, pos.z, 1 ));
  //colorParticle = vec4(1.0f,1.0f,1.0f,1.0f);
  colorParticle = vec4(1.0f,1.0f,1.0f,0.075f);
}
