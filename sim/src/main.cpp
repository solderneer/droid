#include <cinder/Easing.h>
#include <cinder/CinderImGui.h>
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

    vec3 rotation = vec3(0);
    vec3 camPos = vec3(0.0f, 2.5f, 5.0f);
    vec3 camAngle = vec3(0);
};

void Droid::setup() {
  auto lambert = gl::ShaderDef().lambert().color();
  gl::GlslProgRef shader = gl::getStockShader(lambert);

  mServo1 = gl::Batch::create(geom::Cube().size(1, 1, 1), shader);
  mServo2 = gl::Batch::create(geom::Cube().size(1, 1, 1), shader);

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
  ImGui::Begin("Rotation Settings");
  ImGui::SliderFloat3("Rotation", &rotation, 0.00f, 2 * M_PI);
  ImGui::End();

  // Camera settings
  ImGui::Begin("Camera Settings");
  ImGui::SliderFloat3("Position", &camPos, 0, 5);
  ImGui::SliderFloat3("Angle", &camAngle, 0.00f, 2 * M_PI);
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
  gl::rotate( angleAxis( rotation[0], vec3( 1, 0, 0 ) ) );
  gl::rotate( angleAxis( rotation[1], vec3( 0, 1, 0 ) ) );
  gl::rotate( angleAxis( rotation[2], vec3( 0, 0, 1 ) ) );
  mServo1->draw();
  gl::drawCoordinateFrame(1.3, 0.2, 0.05);

  gl::translate(vec3(1.5, 0, 0));
  mServo2->draw();
}

// Unused stub
void Droid::cleanup() {
  ImGui::DestroyContext();
}

CINDER_APP(Droid, RendererGl(), [&](App::Settings *settings) {
  settings->setHighDensityDisplayEnabled(true);
})
