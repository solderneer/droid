#include <cinder/CinderImGui.h>
#include <cinder/GeomIo.h>
#include <cinder/Vector.h>
#include <cinder/app/AppBase.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/wrapper.h>
#include <cmath>
#include <cstdio>
#include <glm/fwd.hpp>
#include <imgui/imgui.h>
#include <iostream>
#include <ostream>
#include <stdlib.h>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

// Defining Droid top level class
class Droid : public App {
  public:
    void setup() override;
    void update() override;
    void draw() override;
    void cleanup() override;

    CameraPersp		mCam;
	  gl::BatchRef	mServo1;
	  gl::BatchRef	mServo2;
    gl::BatchRef  mServo3;
    gl::BatchRef  mLeg;

    vec3 jointPosition = vec3(0);
    vec3 camPos = vec3(0.0f, 25.0f, 40.0f);
    vec3 camAngle = vec3(0);

    float coxaLength = 3.4;
    float femurLength = 6.0;
    float tibiaLength = 10.0;

    vec3 targetPos = vec3(0, 0, 0);

    bool enableIK = false;

  private:
    vec3 fkCalculate();
    vec3 ikCalculate(vec3 pos);
};

void Droid::setup() {
  auto lambert = gl::ShaderDef().lambert().color();
  gl::GlslProgRef shader = gl::getStockShader(lambert);

  mServo1 = gl::Batch::create(geom::Cube().size(4.0, 3.5, 1.9), shader);
  mServo2 = gl::Batch::create(geom::Cube().size(1.9, 4.0, 3.5), shader);
  mServo3 = gl::Batch::create(geom::Cube().size(1.9, 4.0, 3.5), shader);
  mLeg = gl::Batch::create(geom::Cone().height(tibiaLength).radius(1, 0).direction(vec3(1, 0, 0)), shader);

  mCam.lookAt(camPos, camAngle);

  // Setup ImGui and associated variables
  ImGui::Initialize();

  // Adjusting for high density display problems on Mojave (https://github.com/simongeilfus/Cinder-ImGui/issues/75)
  ImGui::GetStyle().ScaleAllSizes(1.4);
  ImGui::GetIO().FontGlobalScale = 1.4;
}

// Unused stub
void Droid::update() {
  // Rotation Settings
  ImGui::Begin("Control Settings");
  ImGui::SliderFloat("Joint 1", &jointPosition[0], -0.5 * M_PI, 0.5 * M_PI);
  ImGui::SliderFloat("Joint 2", &jointPosition[1], 0, -M_PI);
  ImGui::SliderFloat("Joint 3", &jointPosition[2], -0.5 * M_PI, 0.5 * M_PI);

  ImGui::Separator();

  ImGui::SliderFloat("TargetX", &targetPos[0], 0.0f, 19.0f);
  ImGui::SliderFloat("TargetY", &targetPos[1], -20.0f, 20.0f);
  ImGui::SliderFloat("TargetZ", &targetPos[2], -20.0f, 20.0f);

  ImGui::Checkbox("Enable IK", &enableIK);

  ImGui::End();

  // Camera settings
  ImGui::Begin("Camera Settings");
  ImGui::SliderFloat3("Position", &camPos, 0, 50);
  ImGui::SliderFloat3("Angle", &camAngle, 0.00f, 2 * M_PI);
  ImGui::End();

  // Linkage Constraint Adjust
  ImGui::Begin("Linkage Constraint Adjust");
  ImGui::SliderFloat("Coxa Length", &coxaLength, 0, 5);
  ImGui::SliderFloat("Femur Length", &femurLength, 0, 15);
  ImGui::SliderFloat("Tibia Length", &tibiaLength, 0, 15);
  ImGui::End();

  ImGui::Render();
  
  // Update camera position
  mCam.lookAt(camPos, camAngle);
}

void Droid::draw() {
  gl::clear();
  gl::enableDepthRead();
  gl::enableDepthWrite();

  gl::setMatrices(mCam);

  vec3 endPoint;
  
  // Run if enable IK is true
  if(enableIK) {    
    vec3 joints = Droid::ikCalculate(targetPos);

    if(!(isnan(joints[0]) || isnan(joints[1]) || isnan(joints[2]))) {
      jointPosition[0] = joints[0];
      jointPosition[1] = joints[1];
      jointPosition[2]  = joints[2];
    }

    endPoint = fkCalculate();
  } else {
    endPoint = fkCalculate();

    targetPos[0] = endPoint[0];
    targetPos[1] = endPoint[1];
    targetPos[2] = endPoint[2];
  }
  
  // Drawing results of forward kinematics (drawn position in purple vector)
  gl::drawVector(vec3(0, 0, 0), endPoint);

  // Drawing target of inverse kinematics
  gl::drawVector(vec3(0, 0, 0), targetPos);
  
  gl::ScopedModelMatrix scpModelMtx;

  gl::drawCoordinateFrame(3, 0.2, 0.05);
  
  /* Servo 2 transformation and creation */
  gl::rotate( angleAxis( jointPosition[0], vec3( 0, 1, 0 ) ) );
  mServo1->draw();
  gl::drawCoordinateFrame(3, 0.2, 0.05);

  /* Servo 2 transformation and creation */
  gl::translate(vec3(coxaLength, 0, 0));
  gl::rotate( angleAxis( jointPosition[1], vec3( 0, 0, 1 ) ) );
  
  mServo2->draw();
  gl::drawCoordinateFrame(3, 0.2, 0.05);

  /* Servo 3 transformation and creation */
  gl::translate(vec3(0, femurLength, 0));
  mServo3->draw();
  gl::drawCoordinateFrame(3, 0.2, 0.05);

  /* Leg creation */
  gl::rotate( angleAxis( jointPosition[2], vec3( 0, 0, 1 ) ) );
  mLeg->draw();
  gl::drawCoordinateFrame(3, 0.2, 0.05);
}

void Droid::cleanup() {
  ImGui::DestroyContext();
}

vec3 Droid::fkCalculate() {
  // Representing the joints as quaternions
  quat rot1 = angleAxis(jointPosition[0], vec3(0, 1, 0));
  quat rot2 = angleAxis(jointPosition[1], vec3(0, 0, 1));
  quat rot3 = angleAxis(jointPosition[2], vec3(0, 0, 1));

  vec3 end = rot1 * vec3(coxaLength, 0, 0);

  // Adding the servo 3 pos
  end += rot1 * rot2 * vec3(0, femurLength, 0);

  // Adding the tibia length
  end += rot1 * rot2 * rot3 * vec3(tibiaLength, 0, 0);

  return end;
}

// Analytical solution derived from trignometry
// pos[0] = x, pos[1] = y, pos[2] = z
vec3 Droid::ikCalculate(vec3 pos) {
  float legLength = sqrt(pow(pos[0], 2) + pow(pos[2], 2));

  float HF = sqrt(pow((legLength - coxaLength), 2) + pow(pos[1], 2));

  float j2One = atan2((legLength - coxaLength), -pos[1]);
  float j2Two = acos((pow(tibiaLength, 2) - pow(femurLength, 2) - pow(HF, 2)) / (-2 * femurLength * HF));
  std::cout << "j2one: " << j2One << std::endl;
  std::cout << "j2two: " << j2Two << std::endl;
  float j2 = -M_PI + j2One + j2Two;

  float j3 = acos((pow(HF, 2) - pow(tibiaLength, 2) - pow(femurLength, 2)) / (-2 * femurLength * tibiaLength)) - M_PI/2;

  float j1 = -1 * atan2(pos[2], pos[0]);

  return vec3(j1, j2, j3);
}

CINDER_APP(Droid, RendererGl(), [&](App::Settings *settings) {
  settings->setHighDensityDisplayEnabled(true);
})
