#include <cinder/Easing.h>
#include <cinder/CinderImGui.h>
#include <cinder/Vector.h>
#include <cinder/app/AppBase.h>
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
	  gl::BatchRef	mBox;
    float rotationTime;
};

void Droid::setup() {
  auto lambert = gl::ShaderDef().lambert().color();
  gl::GlslProgRef shader = gl::getStockShader(lambert);
  mBox = gl::Batch::create(geom::Cube().size(1, 1, 1), shader);

  mCam.lookAt( vec3(3, 4.5, 2), vec3(0));

  // Setup ImGui and associated variables
  rotationTime = 1.0f;
  ImGui::Initialize();

  // Adjusting for high density display problems on Mojave (https://github.com/simongeilfus/Cinder-ImGui/issues/75)
  ImGui::GetStyle().ScaleAllSizes(1.4);
  ImGui::GetIO().FontGlobalScale = 1.4;
}

// Unused stub
void Droid::update() {
  // Adding UI slider for rotationtime
  ImGui::Begin("Rotation Settings");
  ImGui::SliderFloat("Rotation Time", &rotationTime, 1, 5);
  ImGui::End();
  ImGui::Render();
}

void Droid::draw() {
  gl::clear();
  gl::enableDepthRead();
  gl::enableDepthWrite();

  gl::setMatrices(mCam);

  float time  = fmod(getElapsedFrames() / 30.0f, rotationTime);
  float angle = easeInOutQuint(time / rotationTime) * M_PI * 2;

  gl::ScopedModelMatrix scpModelMtx;
  gl::rotate( angleAxis( angle, vec3( 0, 1, 0 ) ) );
  mBox->draw();
}

// Unused stub
void Droid::cleanup() {
  ImGui::DestroyContext();
}

CINDER_APP(Droid, RendererGl(), [&](App::Settings *settings) {
  settings->setHighDensityDisplayEnabled(true);
})
