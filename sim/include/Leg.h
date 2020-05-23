#ifndef LEG_H_
#define LEG_H_

#define DEFAULT_COXA_LEN 3.4
#define DEFAULT_FEMUR_LEN 6.0
#define DEFAULT_TIBIA_LEN 10.0

#include <cinder/gl/gl.h>

using namespace ci;

class Leg {
  public:
    Leg(gl::GlslProgRef shader);

    void draw();
    void moveTo(vec3 target);
    void updateJointParams(float m_coxaLength, float m_femurLength, float m_tibiaLength);
    bool toggleIk();
  private:
    gl::BatchRef mServo1;
    gl::BatchRef mServo2;
    gl::BatchRef mServo3;
    gl::BatchRef mLeg;

    vec3 jointPos = vec3();
    vec3 targetPos = vec3();

    float coxaLength = DEFAULT_COXA_LEN;
    float femurLength = DEFAULT_FEMUR_LEN;
    float tibiaLength = DEFAULT_TIBIA_LEN;

    bool enableIk = false;

    // Private helper functions
    vec3 fkCalculate();
    vec3 ikCalculate(vec3 pos);
};

#endif /* LEG_H_ */
