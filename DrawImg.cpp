/************************************************************
************************************************************/
#include "DrawImg.h"

/************************************************************
************************************************************/

/******************************
******************************/
DRAW_IMG::DRAW_IMG()
{
}

/******************************
******************************/
DRAW_IMG::~DRAW_IMG()
{
}

/******************************
******************************/
void DRAW_IMG::setup()
{
	img.load("img/img.png");
}

/******************************
******************************/
void DRAW_IMG::update()
{
}

/******************************
******************************/
void DRAW_IMG::draw(ofFbo& fbo)
{
	fbo.begin();
		ofEnableAlphaBlending();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofDisableSmoothing(); // OF_BLENDMODE_ADDとofEnableSmoothingは同時に有効化できない
		
		ofSetColor(Gui_Global->img_alpha);
		img.draw(0, 0, fbo.getWidth(), fbo.getHeight());
	fbo.end();
}

