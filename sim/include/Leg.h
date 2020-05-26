#ifndef LEG_H_
#define LEG_H_

#define DEFAULT_COXA_LEN 3.4
#define DEFAULT_FEMUR_LEN 6.0
#define DEFAULT_TIBIA_LEN 10.0

#include <cinder/gl/gl.h>

using namespace ci;

class Leg {
  public:
    Leg();
    Leg(gl::GlslProgRef *shader);

    void draw();
    void moveToCoord(vec3 *target);
    void moveToJoints(vec3 *joints);
    void updateJointParams(float m_coxaLength, float m_femurLength, float m_tibiaLength);
    
    float coxaLength = DEFAULT_COXA_LEN;
    float femurLength = DEFAULT_FEMUR_LEN;
    float tibiaLength = DEFAULT_TIBIA_LEN;
    
    // Position in Joint Space
    vec3 jointPos = vec3();
    // Target world space coordinates
    vec3 targetPos = vec3();
    // Actual world space coordinates
    vec3 tipPos = vec3();
  private:
    gl::BatchRef mServo1;
    gl::BatchRef mServo2;
    gl::BatchRef mServo3;
    gl::BatchRef mLeg;

    // Private helper functions
    vec3 fkCalculate();
    vec3 ikCalculate(vec3 pos);
};

#endif /* LEG_H_ */
