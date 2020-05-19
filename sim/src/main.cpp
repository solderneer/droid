#include <cinder/Easing.h>
#include <cinder/CinderImGui.h>
#include <cinder/GeomIo.h>
#include <cinder/Vector.h>
#include <cinder/app/AppBase.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/wrapper.h>
#include <glm/fwd.hpp>
#include <imgui/imgui.h>
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

    vec3 servo2Pos = vec3(3.4, 0.3, 0);
    vec3 servo3Pos = vec3(0, 6.0, 0);
    vec3 legPos = vec3(2.0, 1.5, 0);
};

void Droid::setup() {
  auto lambert = gl::ShaderDef().lambert().color();
  gl::GlslProgRef shader = gl::getStockShader(lambert);

  mServo1 = gl::Batch::create(geom::Cube().size(4.0, 3.5, 1.9), shader);
  mServo2 = gl::Batch::create(geom::Cube().size(1.9, 4.0, 3.5), shader);
  mServo3 = gl::Batch::create(geom::Cube().size(1.9, 4.0, 3.5), shader);
  mLeg = gl::Batch::create(geom::Cone().height(10.0).radius(1, 0.2).direction(vec3(1, 0, 0)), shader);

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
  ImGui::Begin("Joint Settings");
  ImGui::SliderFloat("Joint 1", &jointPosition[0], -0.5 * M_PI, 0.5 * M_PI);
  ImGui::SliderFloat("Joint 2", &jointPosition[1], 0, -M_PI);
  ImGui::SliderFloat("Joint 3", &jointPosition[2], -0.5 * M_PI, 0.5 * M_PI);
  ImGui::End();

  // Camera settings
  ImGui::Begin("Camera Settings");
  ImGui::SliderFloat3("Position", &camPos, 0, 50);
  ImGui::SliderFloat3("Angle", &camAngle, 0.00f, 2 * M_PI);
  ImGui::End();

  // Linkage Constraint Adjust
  ImGui::Begin("Linkage Constraint Adjust");
  ImGui::SliderFloat3("Servo 1 - Servo 2", &servo2Pos, 0, 5);
  ImGui::SliderFloat3("Servo 2 - Servo 3", &servo3Pos, 0, 15);
  ImGui::SliderFloat3("Servo 3 - Leg", &legPos, 0, 15);
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

  gl::ScopedModelMatrix scpModelMtx;

  /* Servo 2 transformation and creation */
  gl::rotate( angleAxis( jointPosition[0], vec3( 0, 1, 0 ) ) );
  mServo1->draw();
  gl::drawCoordinateFrame(3, 0.2, 0.05);

  /* Servo 2 transformation and creation */
  gl::translate(servo2Pos);
  gl::rotate( angleAxis( jointPosition[1], vec3( 0, 0, 1 ) ) );
  
  // Calculations to fix rotation about a point that is not the center
  // translate horizontally by dsin(theta) where d is the distance from the center
  // translate vertically by d - dcos(theta) where d is the distance from the center
  
  float d = -1.0f;
  vec3 rotationPoint = vec3(d * sin(jointPosition[1]), -d + d * cos(jointPosition[1]), 0);
  gl::translate(rotationPoint);

  mServo2->draw();

  /* Servo 3 transformation and creation */
  gl::translate(servo3Pos);
  mServo3->draw();

  /* Leg transformation and creation */
  gl::translate(legPos);
  gl::rotate( angleAxis( jointPosition[2], vec3( 0, 0, 1 ) ) );
  mLeg->draw();
}

// Unused stub
void Droid::cleanup() {
  ImGui::DestroyContext();
}

CINDER_APP(Droid, RendererGl(), [&](App::Settings *settings) {
  settings->setHighDensityDisplayEnabled(true);
})
