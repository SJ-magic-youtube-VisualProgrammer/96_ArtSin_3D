/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include <ofMain.h>
#include "fft.h"
#include <ofxVboSet.h>
#include "sj_common.h"

/************************************************************
************************************************************/

/**************************************************
**************************************************/
class DRAW_ARTSIN_3D{
private:
	/****************************************
	****************************************/
	ofColor LastCol;
	ofx__VBO_SET VboSet_Quads;
	ofx__VBO_SET VboSet_Points;
	
	/****************************************
	****************************************/
	void Refresh_vboVerts(const FFT& fft);
	void Refresh_vboColor(const FFT& fft);
	void do_draw( ofFbo& fbo, ofCamera camera, ofVec3f _scale = ofVec3f(1.0, 1.0, 1.0) );
	
public:
	DRAW_ARTSIN_3D();
	~DRAW_ARTSIN_3D();
	
	void setup(const FFT& fft);
	void update(const FFT& fft);
	void draw(ofFbo& fbo, ofCamera camera);
};

