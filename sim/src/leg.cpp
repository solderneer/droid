#include <cinder/gl/gl.h>

#include "../include/leg.h"

Leg::Leg() {
  auto lambert = gl::ShaderDef().lambert().color();
  gl::GlslProgRef shader = gl::getStockShader(lambert);
  
  mServo1 = gl::Batch::create(geom::Cube().size(4.0, 3.5, 1.9), shader);
  mServo2 = gl::Batch::create(geom::Cube().size(1.9, 4.0, 3.5), shader);
  mServo3 = gl::Batch::create(geom::Cube().size(1.9, 4.0, 3.5), shader);
  mLeg = gl::Batch::create(geom::Cone().height(tibiaLength).radius(1, 0).direction(vec3(1, 0, 0)), shader);
}

Leg::Leg(gl::GlslProgRef *shader) {
  mServo1 = gl::Batch::create(geom::Cube().size(4.0, 3.5, 1.9), *shader);
  mServo2 = gl::Batch::create(geom::Cube().size(1.9, 4.0, 3.5), *shader);
  mServo3 = gl::Batch::create(geom::Cube().size(1.9, 4.0, 3.5), *shader);
  mLeg = gl::Batch::create(geom::Cone().height(tibiaLength).radius(1, 0).direction(vec3(1, 0, 0)), *shader);
}

void Leg::draw() {
  gl::ScopedModelMatrix scpModelMtx;

  // Drawing results of forward kinematics (drawn position in purple vector)
  gl::drawVector(vec3(0, 0, 0), tipPos);
  // Drawing target of inverse kinematics
  gl::drawVector(vec3(0, 0, 0), targetPos);
  
  gl::drawCoordinateFrame(3, 0.2, 0.05);
  
  /* Servo 2 transformation and creation */
  gl::rotate( angleAxis( jointPos[0], vec3( 0, 1, 0 ) ) );
  mServo1->draw();
  gl::drawCoordinateFrame(3, 0.2, 0.05);

  /* Servo 2 transformation and creation */
  gl::translate(vec3(coxaLength, 0, 0));
  gl::rotate( angleAxis( jointPos[1], vec3( 0, 0, 1 ) ) );
  
  mServo2->draw();
  gl::drawCoordinateFrame(3, 0.2, 0.05);

  /* Servo 3 transformation and creation */
  gl::translate(vec3(0, femurLength, 0));
  mServo3->draw();
  gl::drawCoordinateFrame(3, 0.2, 0.05);

  /* Leg creation */
  gl::rotate( angleAxis( jointPos[2], vec3( 0, 0, 1 ) ) );
  mLeg->draw();
  gl::drawCoordinateFrame(3, 0.2, 0.05);
}

void Leg::moveToCoord(vec3 *target) {
  targetPos = *target;
  vec3 joints = ikCalculate(targetPos);
  
  if(!(isnan(joints[0]) || isnan(joints[1]) || isnan(joints[2]))) {
    jointPos = joints;
  }

  tipPos = fkCalculate();
}

void Leg::moveToJoints(vec3 *joints) {
  jointPos = *joints;
  tipPos = fkCalculate();
}

void Leg::updateJointParams(float m_coxaLength, float m_femurLength, float m_tibiaLength) {
  coxaLength = m_coxaLength;
  femurLength = m_femurLength;
  tibiaLength = m_tibiaLength;
}


/****************** HELPER FUNCTIONS ****************************/
vec3 Leg::fkCalculate() {
  // Representing the joints as quaternions
  quat rot1 = angleAxis(jointPos[0], vec3(0, 1, 0));
  quat rot2 = angleAxis(jointPos[1], vec3(0, 0, 1));
  quat rot3 = angleAxis(jointPos[2], vec3(0, 0, 1));

  vec3 end = rot1 * vec3(coxaLength, 0, 0);

  // Adding the servo 3 pos
  end += rot1 * rot2 * vec3(0, femurLength, 0);

  // Adding the tibia length
  end += rot1 * rot2 * rot3 * vec3(tibiaLength, 0, 0);

  return end;
}

// Analytical solution derived from trignometry
// pos[0] = x, pos[1] = y, pos[2] = z
vec3 Leg::ikCalculate(vec3 pos) {
  float legLength = sqrt(pow(pos[0], 2) + pow(pos[2], 2));

  float HF = sqrt(pow((legLength - coxaLength), 2) + pow(pos[1], 2));

  float j2One = atan2((legLength - coxaLength), -pos[1]);
  float j2Two = acos((pow(tibiaLength, 2) - pow(femurLength, 2) - pow(HF, 2)) / (-2 * femurLength * HF));
  float j2 = -M_PI + j2One + j2Two;

  float j3 = acos((pow(HF, 2) - pow(tibiaLength, 2) - pow(femurLength, 2)) / (-2 * femurLength * tibiaLength)) - M_PI/2;

  float j1 = -1 * atan2(pos[2], pos[0]);

  return vec3(j1, j2, j3);
}
