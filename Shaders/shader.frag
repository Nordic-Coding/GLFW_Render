#version 460
out vec4 LFragment;

in vec4 colorParticle;

void main()
{
  //Just a straightforward coloring for now.
  LFragment = vec4( abs(colorParticle.x), abs(colorParticle.y), abs(colorParticle.z), colorParticle.w );
}
