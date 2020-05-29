#include <cinder/CinderImGui.h>
#include <cinder/app/AppBase.h>
#include <cinder/gl/draw.h>
#include <cstdio>
#include <imgui/imgui.h>
#include <iostream>
#include <ostream>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "../include/leg.h"

#define DEFAULT_GAIT_POS_X 10.0
#define DEFAULT_GAIT_POS_Y -5.0
#define DEFAULT_GAIT_POS_Z 0.0

using namespace ci;
using namespace ci::app;

// Defining Droid top level class
class Droid : public App {
  public:
    void setup() override;
    void update() override;
    void draw() override;
    void ikCalculate();
    void cleanup() override;

    CameraPersp		mCam;
    Leg mLeg[6];
    gl::BatchRef mBody;

    // Initial position params for each leg
    float legRadius = 10.0; // Origin position of leg

    // Leg control input (setting default floor position to -5.0)
    vec3 ujointPos = vec3();
    vec3 utargetPos = vec3(); // Default Gait Position
    
    bool enableIk = true;
    const char* legSelect[6] = {"Leg 0", "Leg 1", "Leg 2", "Leg 3", "Leg 4", "Leg 5"};
    int legSelected = 0;
   
    bool lockLegs = true;

    // Body control input
    vec3 bodyPos = vec3();
    vec3 bodyRot = vec3();

    // Camera Information
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
  // Joint Control Settings
  ImGui::Begin("Joint Control Settings");

  // Conditionally render leg select based on lockLegs
  if(!lockLegs) {
    ImGui::Combo("Leg Select", &legSelected, legSelect, IM_ARRAYSIZE(legSelect));
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
  }
  
  ImGui::SliderFloat("Joint 1", &ujointPos[0], -0.5 * M_PI, 0.5 * M_PI);
  ImGui::SliderFloat("Joint 2", &ujointPos[1], 0, -M_PI);
  ImGui::SliderFloat("Joint 3", &ujointPos[2], -0.5 * M_PI, 0.5 * M_PI);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::SliderFloat("TargetX", &utargetPos[0], 0.0f, 19.0f);
  ImGui::SliderFloat("TargetY", &utargetPos[1], -20.0f, 20.0f);
  ImGui::SliderFloat("TargetZ", &utargetPos[2], -20.0f, 20.0f);
  
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Enable IK", &enableIk);
  if(ImGui::Button("Reset Defaults")) {
    for(int i = 0; i < 6; i++) {
      mLeg[i].updateJointParams(DEFAULT_COXA_LEN, DEFAULT_FEMUR_LEN, DEFAULT_TIBIA_LEN);

      ujointPos = vec3(0, 0, 0);
      mLeg[i].moveToJoints(&ujointPos);
      enableIk = false;
    }
  }
  ImGui::SameLine(); 

  // Conditionally render lock and unlock legs option
  if(lockLegs) {
    if(ImGui::Button("Unlock Legs")) {
      lockLegs = false;
    }
  } else {
    if(ImGui::Button("Lock Legs")) {
      lockLegs = true;
    }
  }

  ImGui::End();

  //  Body Control Settings
  ImGui::Begin("Body Control Settings");
  
  ImGui::SliderFloat("Translate X", &bodyPos[0], -10, 10);
  ImGui::SliderFloat("Translate Y", &bodyPos[1], -10, 10);
  ImGui::SliderFloat("Translate Z", &bodyPos[2], -10, 10);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::SliderFloat("Rotate X", &bodyRot[0], -0.5 * M_PI, 0.5 * M_PI);
  ImGui::SliderFloat("Rotate Y", &bodyRot[1], -0.5 * M_PI, 0.5 * M_PI);
  ImGui::SliderFloat("Rotate Z", &bodyRot[2], -0.5 * M_PI, 0.5 * M_PI);
  
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
  if(lockLegs) {
    if(enableIk) {
      for(int i = 0; i < 6; i++)
        mLeg[i].moveToCoord(&utargetPos);

      ujointPos = mLeg[0].jointPos;
    } else {
      for(int i = 0; i < 6; i++)
        mLeg[i].moveToJoints(&ujointPos);
      
      utargetPos = mLeg[0].tipPos;
    }
  } else {
    if(enableIk) {
        mLeg[legSelected].moveToCoord(&utargetPos);
        ujointPos = mLeg[legSelected].jointPos;
    } else {
      mLeg[legSelected].moveToJoints(&ujointPos);
      utargetPos = mLeg[legSelected].tipPos;
    }
  }
  */

  // ikCalculate();
  
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
    
    vec3 bodyOffset = vec3(cos(increment * i) * legRadius, 0, sin(increment * i) * legRadius);
    vec3 legPos = vec3(
        cos(increment * i) * (DEFAULT_COXA_LEN + DEFAULT_FEMUR_LEN), 
        -DEFAULT_TIBIA_LEN, 
        sin(increment * i) * (DEFAULT_COXA_LEN + DEFAULT_FEMUR_LEN));

    vec3 totalPos = bodyOffset + legPos + bodyPos;

    float distToLeg = sqrt(pow(totalPos[0], 2) + pow(totalPos[2], 2));
    float angleToLeg = atan2(totalPos[2], totalPos[0]);

    std::cout << i << "| Distance: " << distToLeg << " Angle: " << angleToLeg << std::endl;
    
    float roll = tan(bodyRot[2]) * totalPos[0]; // About the Z Axis
    float pitch = tan(bodyRot[0]) * totalPos[2]; // About the X Axis
    
    float bodyIkX = cos(angleToLeg + bodyRot[1]) * distToLeg - totalPos[0];
    float bodyIkY = roll + pitch;
    float bodyIkZ = sin(angleToLeg + bodyRot[1]) * distToLeg - totalPos[2];
  
    vec3 finalLegPos = legPos + vec3(bodyIkX, bodyIkY, bodyIkZ) + bodyPos;

    // Coordinate frame transform from body to leg (rotated)
    vec3 legCordFrame = vec3(
        cos(i * increment) * finalLegPos[0] + sin(i * increment) * finalLegPos[2],
        finalLegPos[1],
        -sin(i * increment) * finalLegPos[0] + cos(i * increment) * finalLegPos[2]);

    gl::drawVector(vec3(0,0,0), legCordFrame);

    mLeg[i].moveToCoord(&legCordFrame);
    
    gl::translate(vec3(cos(i * increment) * legRadius, 0, sin(i * increment) * legRadius));
    gl::rotate(-i * increment, vec3(0, 1, 0));
    mLeg[i].draw();
  }
}

void Droid::ikCalculate() {
  const float increment = M_PI/3;

  for(int i = 0; i < 6; i++) {
    vec3 bodyOffset = vec3(cos(increment * i) * legRadius, sin(increment * i) * legRadius, 0);
    vec3 legPos = vec3(
        cos(increment * i) * (DEFAULT_COXA_LEN + DEFAULT_FEMUR_LEN), 
        DEFAULT_TIBIA_LEN, 
        sin(increment * i) * (DEFAULT_COXA_LEN + DEFAULT_FEMUR_LEN));

    vec3 totalPos = bodyOffset + legPos + bodyPos;
    float distToLeg = sqrt(pow(totalPos[0], 2) + pow(totalPos[2], 2));
    float angleToLeg = atan2(totalPos[2], totalPos[0]);
    
    float rollZ = tan(bodyRot[2]) * totalPos[0];
    float rollX = tan(bodyRot[0]) * totalPos[1];
    float bodyIkX = cos(angleToLeg + bodyRot[1]) * distToLeg - totalPos[0];
    float bodyIkY = sin(angleToLeg + bodyRot[1]) * distToLeg - totalPos[1];
    float bodyIkZ = rollZ + rollX;

    vec3 finalLegPos = legPos + vec3(bodyIkX, bodyIkY, bodyIkZ);
    mLeg[i].moveToCoord(&finalLegPos);
  }
}

void Droid::cleanup() {
  ImGui::DestroyContext();
}

CINDER_APP(Droid, RendererGl(), [&](App::Settings *settings) {
  settings->setHighDensityDisplayEnabled(true);
})
