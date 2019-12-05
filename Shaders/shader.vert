#version 460
in vec3 pos;
in vec3 col;

uniform mat4 MVP;

out vec4 colorParticle;

void main()
{
  gl_Position = MVP*(vec4( pos.x, pos.y, pos.z, 1 ));
  //colorParticle = vec4(1.0f,1.0f,1.0f,1.0f);
  colorParticle = 3.0f*vec4(col.x,col.y,col.z,0.075f);
}
