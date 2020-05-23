#include <cinder/gl/gl.h>

#include "../include/leg.h"

void Leg::Leg(gl::GlslProgRef shader) {
  mServo1 = gl::Batch::create(geom::Cube().size(4.0, 3.5, 1.9), shader);
  mServo2 = gl::Batch::create(geom::Cube().size(1.9, 4.0, 3.5), shader);
  mServo3 = gl::Batch::create(geom::Cube().size(1.9, 4.0, 3.5), shader);
  mLeg = gl::Batch::create(geom::Cone().height(tibiaLength).radius(1, 0).direction(vec3(1, 0, 0)), shader);
}
