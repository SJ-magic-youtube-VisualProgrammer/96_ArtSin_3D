/************************************************************
************************************************************/
#include "DrawArtSin.h"

/************************************************************
************************************************************/

/******************************
******************************/
DRAW_ARTSIN::DRAW_ARTSIN()
{
}

/******************************
******************************/
DRAW_ARTSIN::~DRAW_ARTSIN()
{
}

/******************************
******************************/
void DRAW_ARTSIN::setup(const FFT& fft)
{
	/********************
	********************/
	print_separatoin();
	printf("> setup : Draw ArtSin 2D\n");
	fflush(stdout);
	
	/********************
	********************/
	VboSet.setup(AUDIO_BUF_SIZE * 4);
	Refresh_vboVerts(fft);
	Refresh_vboColor();
	VboSet.update_vertex_color();
	
	LastCol = Gui_Global->col_ArtSin2D;
}

/******************************
1--2
|  |
0--3
******************************/
void DRAW_ARTSIN::Refresh_vboVerts(const FFT& fft)
{
	const ofVec2f ofs = ofVec2f(Gui_Global->ArtSin2D_Graph_ofs->x, Gui_Global->ArtSin2D_Graph_ofs->y);
	const float width_of_bar = 1;
	const float space_per_bar = 3;
	const float BarHeight = Gui_Global->ArtSin2D_BarHeight;
	
	for(int i = 0; i < AUDIO_BUF_SIZE; i++){
		float _y = ofs.y - fft.get_artSin(i) * BarHeight;
		
		VboSet.set_vboVerts( i * 4 + 0, ofs.x + i * space_per_bar,					ofs.y );
		VboSet.set_vboVerts( i * 4 + 1, ofs.x + i * space_per_bar,					_y );
		VboSet.set_vboVerts( i * 4 + 2, ofs.x + i * space_per_bar + width_of_bar,	_y);
		VboSet.set_vboVerts( i * 4 + 3, ofs.x + i * space_per_bar + width_of_bar,	ofs.y );
	}
}

/******************************
******************************/
void DRAW_ARTSIN::Refresh_vboColor()
{
	VboSet.set_singleColor(Gui_Global->col_ArtSin2D);
	
	return;
}

/******************************
******************************/
void DRAW_ARTSIN::update(const FFT& fft)
{
	Refresh_vboVerts(fft);
	VboSet.update_vertex();
	
	ofColor col_gui = Gui_Global->col_ArtSin2D;
	if(col_gui != LastCol){
		Refresh_vboColor();
		VboSet.update_color();
		
		LastCol = Gui_Global->col_ArtSin2D;
	}
}

/******************************
******************************/
void DRAW_ARTSIN::draw(ofFbo& fbo)
{
	const ofVec2f ofs = ofVec2f(Gui_Global->ArtSin2D_Graph_ofs->x, Gui_Global->ArtSin2D_Graph_ofs->y);
	
	fbo.begin();
		/********************
		********************/
		ofEnableAlphaBlending();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofDisableSmoothing(); // OF_BLENDMODE_ADDとofEnableSmoothingは同時に有効化できない
		
		/********************
		********************/
		ofFill();
		
		ofSetColor(255);
		// glPointSize(Gui_Global->particleSize);
		VboSet.draw(GL_QUADS);
	fbo.end();
}

