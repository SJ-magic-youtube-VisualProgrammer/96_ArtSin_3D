/************************************************************
************************************************************/
#include "sj_common.h"

/************************************************************
************************************************************/
/********************
********************/
int GPIO_0 = 0;
int GPIO_1 = 0;

/********************
********************/
GUI_GLOBAL* Gui_Global = NULL;

FILE* fp_Log = nullptr;


/************************************************************
func
************************************************************/
/******************************
******************************/
double LPF(double LastVal, double CurrentVal, double Alpha_dt, double dt)
{
	double Alpha;
	if((Alpha_dt <= 0) || (Alpha_dt < dt))	Alpha = 1;
	else									Alpha = 1/Alpha_dt * dt;
	
	return CurrentVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
double LPF(double LastVal, double CurrentVal, double Alpha)
{
	if(Alpha < 0)		Alpha = 0;
	else if(1 < Alpha)	Alpha = 1;
	
	return CurrentVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
double sj_max(double a, double b)
{
	if(a < b)	return b;
	else		return a;
}

/******************************
******************************/
bool checkIf_ContentsExist(char* ret, char* buf)
{
	if( (ret == NULL) || (buf == NULL)) return false;
	
	string str_Line = buf;
	Align_StringOfData(str_Line);
	vector<string> str_vals = ofSplitString(str_Line, ",");
	if( (str_vals.size() == 0) || (str_vals[0] == "") ){ // no_data or exist text but it's",,,,,,,".
		return false;
	}else{
		return true;
	}
}

/******************************
******************************/
void Align_StringOfData(string& s)
{
	size_t pos;
	while((pos = s.find_first_of(" 　\t\n\r")) != string::npos){ // 半角・全角space, \t 改行 削除
		s.erase(pos, 1);
	}
}

/******************************
******************************/
void print_separatoin()
{
	printf("---------------------------------\n");
}

/******************************
******************************/
void ClearFbo(ofFbo& fbo)
{
	fbo.begin();
		ofClear(0, 0, 0, 0);
	fbo.end();
}

/******************************
******************************/
void Read_Lissajous_id_info_from_gui(int id[], int size)
{
	id[0] = int(Gui_Global->Li_GainId_0);
	for(int i = 1; i < size; i++){
		id[i] = id[i - 1] + int(Gui_Global->Li_Gain_ofs);
		if(AUDIO_BUF_SIZE/2 - 1 < id[i]) id[i] = AUDIO_BUF_SIZE/2 - 1;
	}
}

/******************************
******************************/
float toRad(float val){
	return val * 3.1415 / 180.0;
}

/******************************
******************************/
float toDeg(float val){
	return val * 180.0 / 3.1415;
}


/************************************************************
class
************************************************************/

/******************************
******************************/
void GUI_GLOBAL::setup(string GuiName, string FileName, float x, float y)
{
	/********************
	********************/
	gui.setup(GuiName.c_str(), FileName.c_str(), x, y);
	
	/********************
	********************/
	Group_Audio.setup("Audio");
		Group_Audio.add(b_Audio_Start.setup("Start", false));
		Group_Audio.add(b_Audio_Stop.setup("Stop", false));
		Group_Audio.add(b_Audio_Reset.setup("Reset", false));
	gui.add(&Group_Audio);
	
	Group_FFT.setup("FFT");
		Group_FFT.add(Gain.setup("Gain", 1.0, 1.0, 5.0));
		Group_FFT.add(k_smooth.setup("k_smooth", 0.95, 0.8, 1.0));
		Group_FFT.add(dt_smooth_2.setup("dt_smooth_2", 167, 10, 300));
		Group_FFT.add(b_Window.setup("b_window", true));
	gui.add(&Group_FFT);
	
	Group_ArtSin.setup("ArtSin");
		Group_ArtSin.add(ArtSin_Band_min.setup("ArtSin_Band_min", 1.0, 1.0, 255.0));
		Group_ArtSin.add(ArtSin_Band_max.setup("ArtSin_Band_max", 1.0, 1.0, 255.0));
		Group_ArtSin.add(ArtSin_PhaseMap_k.setup("ArtSin_PhaseMap_k", 1.0, 0.0, 2.0));
		Group_ArtSin.add(b_ArtSin_abs.setup("b_ArtSin_abs", false));
		Group_ArtSin.add(b_Window_artSin.setup("b_Window_artSin", false));
		Group_ArtSin.add(ArtSin_window_alpha.setup("window_alpha", 0.3, 0.0, 1.0));
	gui.add(&Group_ArtSin);
	
	Group_ArtSin3D.setup("ArtSin3D");
		Group_ArtSin3D.add(b_Draw_ArtSin3D_Quads.setup("b_Draw_Quads3D", false));
		Group_ArtSin3D.add(b_Draw_ArtSin3D_Points.setup("b_Draw_Points3D", true));
		Group_ArtSin3D.add(b_ArtSin3D_DrawMode_Add.setup("b_DrawMode_Add3D", true));
		{
			ofColor initColor = ofColor(255, 255, 255, 140);
			ofColor minColor = ofColor(0, 0, 0, 0);
			ofColor maxColor = ofColor(255, 255, 255, 255);
			Group_ArtSin3D.add(col_ArtSin3D.setup("col_ArtSin3D", initColor, minColor, maxColor));
		}
		Group_ArtSin3D.add(b_ArtSin3D_EnableDepthTest.setup("b_EnableDepthTest", false));
		Group_ArtSin3D.add(ArtSin3D_BarHeight.setup("BarHeight3D", 300, 0.0, 1000));
		Group_ArtSin3D.add(ArtSin3D_BarSpace.setup("BarSpace3D", 3, 1.0, 10.0));
		Group_ArtSin3D.add(ArtSin3D_BarWidth.setup("BarWidth3D", 1, 1.0, 10.0));
		Group_ArtSin3D.add(ArtSin3D_PointSize.setup("PointSize3D", 1.5, 1.0, 10.0));
		Group_ArtSin3D.add(ArtSin3D_NumDraw.setup("NumDraw", 1.0, 1.0, 1000.0));
		Group_ArtSin3D.add(b_Draw_ArtSin3D_Mirror.setup("b_Draw_Mirror3D", false));
	gui.add(&Group_ArtSin3D);
	
	Group_ArtSin2D.setup("ArtSin2D");
		Group_ArtSin2D.add(b_Draw_ArtSin2D.setup("b_Draw_ArtSin2D", false));
		{
			ofVec2f initVec2f = ofVec2f(192, 810);
			ofVec2f minVec2f = ofVec2f(0, 0);
			ofVec2f maxVec2f = ofVec2f(1920, 1080);
			Group_ArtSin2D.add(ArtSin2D_Graph_ofs.setup("Graph_ofs", initVec2f, minVec2f, maxVec2f));
		}
		Group_ArtSin2D.add(ArtSin2D_BarHeight.setup("BarHeight2D", 400, 0.0, 1000));
		{
			ofColor initColor = ofColor(255, 255, 255, 140);
			ofColor minColor = ofColor(0, 0, 0, 0);
			ofColor maxColor = ofColor(255, 255, 255, 255);
			Group_ArtSin2D.add(col_ArtSin2D.setup("col_ArtSin2D", initColor, minColor, maxColor));
		}
	gui.add(&Group_ArtSin2D);
	
	Group_Lissajous.setup("Lissajous");
		Group_Lissajous.add(b_DrawLissajous.setup("b_DrawLissajous", false));
		Group_Lissajous.add(b_EnableDistortion.setup("b_EnableDistortion", true));
		{
			ofVec2f initVec2f = ofVec2f(600, 600);
			ofVec2f minVec2f = ofVec2f(0, 0);
			ofVec2f maxVec2f = ofVec2f(2000, 2000);
			Group_Lissajous.add(Li_GraphSize.setup("GraphSize", initVec2f, minVec2f, maxVec2f));
		}
		Group_Lissajous.add(Li_GainId_0.setup("GainId_0", 2.0, 1.0, 255.0));
		Group_Lissajous.add(Li_Gain_ofs.setup("Gain_ofs", 1.0, 1.0, 10.0));
		Group_Lissajous.add(Lissajous_Scale.setup("Scale", 100.0, 10.0, 3000.0));
		Group_Lissajous.add(Lissajous_PointSize.setup("PointSize", 3.0, 1.0, 10.0));
		Group_Lissajous.add(Lissajous_LineWidth.setup("LineWidth", 2.0, 1.0, 10.0));
		Group_Lissajous.add(Lissajous_theta_rot_speed.setup("rot_speed", 0.0, 0.0, 360.0));
		{
			ofColor initColor = ofColor(255, 255, 255, 120);
			ofColor minColor = ofColor(0, 0, 0, 0);
			ofColor maxColor = ofColor(255, 255, 255, 255);
			Group_Lissajous.add(col_Lissajous.setup("color", initColor, minColor, maxColor));
		}
		Group_Lissajous.add(b_Lissajous_Line.setup("b_Lissajous_Line", false));
		Group_Lissajous.add(b_EnableDistortion.setup("b_EnableDistortion", true));
		Group_Lissajous.add(Lissajous_a_b__alpha.setup("a_b__alpha", 0.15, 0.05, 1.0));
		Group_Lissajous.add(Lissajous_theta_b__alpha.setup("theta_b__alpha", 0.1, 0.05, 1.0));
	gui.add(&Group_Lissajous);
	
	Group_CamParam.setup("CamParam");
		{
			ofVec3f initVec3f = ofVec3f(0, 0, 1200);
			ofVec3f minVec3f = ofVec3f(-3000, -3000, -3000);
			ofVec3f maxVec3f = ofVec3f(3000, 3000, 3000);
			Group_CamParam.add(Cam_Pos.setup("Pos", initVec3f, minVec3f, maxVec3f));
		}
		Group_CamParam.add(Cam_FOV.setup("FOV", 60.0, 20.0, 120.0));
		Group_CamParam.add(Cam_NearClip.setup("NearClip", 5.0, 0.0, 100.0));
		Group_CamParam.add(Cam_FarClip.setup("FarClip", 4000.0, 0.0, 6000.0));
		{
			ofVec3f initVec3f = ofVec3f(0, 0, 0);
			ofVec3f minVec3f = ofVec3f(-100, -100, -100);
			ofVec3f maxVec3f = ofVec3f(100, 100, 100);
			Group_CamParam.add(Cam_LookAt.setup("LookAt", initVec3f, minVec3f, maxVec3f));
		}
		{
			ofVec3f initVec3f = ofVec3f(0, 1, 0);
			ofVec3f minVec3f = ofVec3f(-1, -1, -1);
			ofVec3f maxVec3f = ofVec3f(1, 1, 1);
			Group_CamParam.add(Cam_Up.setup("UpVector", initVec3f, minVec3f, maxVec3f));
		}
		Group_CamParam.add(Cam_AspectRatio.setup("AspectRatio", 1.78, 0.5625, 2.0));
		Group_CamParam.add(Cam_Ortho.setup("Ortho", false));
		Group_CamParam.add(Cam_V_Flip.setup("V_Flip", false));
	gui.add(&Group_CamParam);
	
	Group_img.setup("img");
		Group_img.add(img_alpha.setup("alpha", 100, 0.0, 255.0));
	gui.add(&Group_img);
	
	
	/********************
	********************/
	gui.minimizeAll();
}

