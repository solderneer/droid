#include <cinder/Vector.h>
#include <cinder/app/AppBase.h>
#include <cinder/gl/wrapper.h>
#include <glm/fwd.hpp>
#include <ostream>
#include <stdlib.h>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

class Template : public App {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void cleanup() override;
    
};

void Template::setup()
{
}
void Template::update()
{
}
void Template::draw()
{
  gl::clear();
  gl::setMatricesWindow(getWindowSize());

  // Preserve the default model matrix
  gl::pushModelMatrix();
  gl::translate(getWindowCenter());

  int numCircles = 16;
  float radius = getWindowHeight() / 2 - 30;

  for (int i = 0; i < numCircles; ++i) {
    float rel = i / (float) numCircles;
    float angle = rel * (M_PI * 2);
    vec2 offset(cos(angle), sin(angle));

    // preserve the Model matrix
		gl::pushModelMatrix();
		// move to the correct position
		gl::translate( offset * radius );
		// set the color using HSV color
		gl::color( Color( CM_HSV, rel, 1, 1 ) );
		// draw a circle relative to Model matrix
		gl::drawStrokedCircle( vec2(), 30 );
		// restore the Model matrix
		gl::popModelMatrix();

    std::cout << i << std::endl;
  }

  // draw a white circle at window center
	gl::color( Color( 1, 1, 1 ) );
	gl::drawSolidCircle( vec2(), 15 );

	// restore the default Model matrix
	gl::popModelMatrix();
}

void Template::cleanup()
{
}

CINDER_APP( Template , RendererGl)
