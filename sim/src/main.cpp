#include <cinder/CinderImGui.h>
#include <cinder/app/AppBase.h>
#include <cinder/gl/draw.h>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "../include/leg.h"

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
    Leg mLeg[6];
    gl::BatchRef mBody;

    // User Input
    vec3 ujointPos = vec3();
    vec3 utargetPos = vec3();
    bool enableIk = false;
    float armRadius = 10.0;

    vec3 camPos = vec3(0.0f, 25.0f, 40.0f);
    vec3 camAngle = vec3(0);

};

void Droid::setup() {
  auto lambert = gl::ShaderDef().lambert().color();
  gl::GlslProgRef shader = gl::getStockShader(lambert);

  for(int i = 0; i < 6; i++) {
    mLeg[i] = Leg(&shader);
  }
  
  mBody = gl::Batch::create(geom::Capsule().radius(4.0).length(10.0).direction(vec3(0, 0, 1)), shader);

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
  ImGui::SliderFloat("Joint 1", &ujointPos[0], -0.5 * M_PI, 0.5 * M_PI);
  ImGui::SliderFloat("Joint 2", &ujointPos[1], 0, -M_PI);
  ImGui::SliderFloat("Joint 3", &ujointPos[2], -0.5 * M_PI, 0.5 * M_PI);

  ImGui::Separator();

  ImGui::SliderFloat("TargetX", &utargetPos[0], 0.0f, 19.0f);
  ImGui::SliderFloat("TargetY", &utargetPos[1], -20.0f, 20.0f);
  ImGui::SliderFloat("TargetZ", &utargetPos[2], -20.0f, 20.0f);

  ImGui::Checkbox("Enable IK", &enableIk);

  ImGui::End();

  // Camera settings
  ImGui::Begin("Camera Settings");
  ImGui::SliderFloat3("Position", &camPos, 0, 50);
  ImGui::SliderFloat3("Angle", &camAngle, 0.00f, 2 * M_PI);
  ImGui::End();

  // Linkage Constraint Adjust
  // ImGui::Begin("Linkage Constraint Adjust");
  // ImGui::SliderFloat("Coxa Length", &coxaLength, 0, 5);
  // ImGui::SliderFloat("Femur Length", &femurLength, 0, 15);
  // ImGui::SliderFloat("Tibia Length", &tibiaLength, 0, 15);
  // ImGui::End();

  ImGui::Render();

  /* Update Leg Position
  if(enableIk) {
    mLeg.moveToCoord(&utargetPos);
    ujointPos = mLeg.jointPos;
  } else {
    mLeg.moveToJoints(&ujointPos);
    utargetPos = mLeg.tipPos;
  } */
  
  // Update camera position
  mCam.lookAt(camPos, camAngle);
}

void Droid::draw() {
  gl::clear();
  gl::enableDepthRead();
  gl::enableDepthWrite();

  gl::setMatrices(mCam);

  mBody->draw();
  gl::drawCoordinateFrame(10.0);

  const float increment = M_PI/3;
  for(int i = 0; i < 6; i++) {
    gl::ScopedModelMatrix scpModelMtx;
    
    gl::translate(vec3(cos(i * increment) * armRadius, 0, sin(i * increment) * armRadius));
    gl::rotate(-i * increment, vec3(0, 1, 0));
    mLeg[i].draw();
  }
}

void Droid::cleanup() {
  ImGui::DestroyContext();
}

CINDER_APP(Droid, RendererGl(), [&](App::Settings *settings) {
  settings->setHighDensityDisplayEnabled(true);
})
