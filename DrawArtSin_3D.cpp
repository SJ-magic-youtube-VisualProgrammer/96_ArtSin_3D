/************************************************************
************************************************************/
#include "DrawArtSin_3D.h"

/************************************************************
************************************************************/

/******************************
******************************/
DRAW_ARTSIN_3D::DRAW_ARTSIN_3D()
{
}

/******************************
******************************/
DRAW_ARTSIN_3D::~DRAW_ARTSIN_3D()
{
}

/******************************
******************************/
void DRAW_ARTSIN_3D::setup(const FFT& fft)
{
	/********************
	********************/
	print_separatoin();
	printf("> setup : Draw ArtSin 3D\n");
	fflush(stdout);
	
	/********************
	********************/
	VboSet_Points.setup(AUDIO_BUF_SIZE * 1);
	VboSet_Quads.setup(AUDIO_BUF_SIZE * 4);
	Refresh_vboVerts(fft);
	Refresh_vboColor(fft);
	VboSet_Points.update_vertex_color();
	VboSet_Quads.update_vertex_color();
	
	LastCol = Gui_Global->col_ArtSin3D;
}

/******************************
1--2
|  |
0--3
******************************/
void DRAW_ARTSIN_3D::Refresh_vboVerts(const FFT& fft)
{
	const ofVec2f ofs = ofVec2f(-AUDIO_BUF_SIZE/2 * Gui_Global->ArtSin3D_BarSpace, 0);
	const float width_of_bar = Gui_Global->ArtSin3D_BarWidth;
	const float space_per_bar = Gui_Global->ArtSin3D_BarSpace;
	const float BarHeight = Gui_Global->ArtSin3D_BarHeight;
	
	for(int i = 0; i < AUDIO_BUF_SIZE; i++){
		float _y = fft.get_artSin(i) * BarHeight;
		
		if(Gui_Global->b_Draw_ArtSin3D_Quads){
			VboSet_Quads.set_vboVerts( i * 4 + 0, ofs.x + i * space_per_bar,				ofs.y );
			VboSet_Quads.set_vboVerts( i * 4 + 1, ofs.x + i * space_per_bar,				ofs.y + _y );
			VboSet_Quads.set_vboVerts( i * 4 + 2, ofs.x + i * space_per_bar + width_of_bar,	ofs.y + _y);
			VboSet_Quads.set_vboVerts( i * 4 + 3, ofs.x + i * space_per_bar + width_of_bar,	ofs.y );
		}
		
		if(Gui_Global->b_Draw_ArtSin3D_Points){
			VboSet_Points.set_vboVerts( i, ofs.x + i * space_per_bar,						ofs.y + _y );
		}
	}

}

/******************************
description
	本関数内を
		if(Gui_Global->b_Draw_ArtSin3D_Quads)
		if(Gui_Global->b_Draw_ArtSin3D_Points)
	で分岐してしまうと、color変更の瞬間に設定されていたmodeのみcolorをupdateすることになるので、
	ここは、分岐なし
******************************/
void DRAW_ARTSIN_3D::Refresh_vboColor(const FFT& fft)
{
	VboSet_Points.set_singleColor(Gui_Global->col_ArtSin3D);
	VboSet_Quads.set_singleColor(Gui_Global->col_ArtSin3D);
	
	return;
}

/******************************
******************************/
void DRAW_ARTSIN_3D::update(const FFT& fft)
{
	Refresh_vboVerts(fft);
	if(Gui_Global->b_Draw_ArtSin3D_Points)	VboSet_Points.update_vertex();
	if(Gui_Global->b_Draw_ArtSin3D_Quads)	VboSet_Quads.update_vertex();
	
	ofColor col_gui = Gui_Global->col_ArtSin3D;
	if(col_gui != LastCol){
		Refresh_vboColor(fft);
		
		/********************
		以下を
			if(Gui_Global->b_Draw_ArtSin3D_Quads)
			if(Gui_Global->b_Draw_ArtSin3D_Points)
		で分岐してしまうと、color変更の瞬間に設定されていたmodeのみcolorをupdateすることになるので、
		ここは、分岐なし
		********************/
		VboSet_Quads.update_color();
		VboSet_Points.update_color();
		
		LastCol = Gui_Global->col_ArtSin3D;
	}
}

/******************************
******************************/
void DRAW_ARTSIN_3D::do_draw(ofFbo& fbo, ofCamera camera, ofVec3f _scale)
{
	float d_theta = 180.0 / int(Gui_Global->ArtSin3D_NumDraw);
	
	fbo.begin();
	camera.begin();
	
	ofPushMatrix();
		/********************
		********************/
		ofScale(_scale);
		
		/********************
		********************/
		if(Gui_Global->b_ArtSin3D_EnableDepthTest) ofEnableDepthTest();
		
		/********************
		********************/
		ofEnableAlphaBlending();
		if(Gui_Global->b_ArtSin3D_DrawMode_Add)	ofEnableBlendMode(OF_BLENDMODE_ADD);
		else									ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		ofDisableSmoothing(); // OF_BLENDMODE_ADDとofEnableSmoothingは同時に有効化できない
			
		ofSetColor(255);
		for(int i = 0; i < int(Gui_Global->ArtSin3D_NumDraw); i++){
			/********************
			********************/
			if(Gui_Global->b_Draw_ArtSin3D_Points){
				VboSet_Points.draw(GL_POINTS);
			}
			
			if(Gui_Global->b_Draw_ArtSin3D_Quads){
				ofFill();
				VboSet_Quads.draw(GL_QUADS);
			}
			
			/********************
			********************/
			ofRotateYDeg(d_theta);
		}
	
		/********************
		********************/
		if(Gui_Global->b_ArtSin3D_EnableDepthTest) ofDisableDepthTest();
	ofPopMatrix();
	
	camera.end();
	fbo.end();
}

/******************************
******************************/
void DRAW_ARTSIN_3D::draw(ofFbo& fbo, ofCamera camera)
{
	do_draw(fbo, camera);
	if(Gui_Global->b_Draw_ArtSin3D_Mirror)	do_draw(fbo, camera, ofVec3f(1.0, -1.0, 1.0));
}

