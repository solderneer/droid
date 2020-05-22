#ifndef LEG_H_

#include <cinder/gl/gl.h>

using namespace ci;

class Leg {
  public:
    void setup();
    void draw();
    void showImGuiWindows();
  private:
    gl::BatchRef mServo1;
    gl::BatchRef mServo2;
    gl::BatchRef mServo3;
    gl::BatchRef mLeg;

    vec3 jointPos;
    vec3 targetPos;

    float coxaLength;
    float femurLength;
    float tibiaLength;
};

#endif /* LEG_H_ */
