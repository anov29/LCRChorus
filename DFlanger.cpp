#include "DFlanger.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
	DepthFaderParam,
	RateFaderParam,
	FeedbackFaderParam,
	ChorusFaderParam, 
	LFOParam,
	ModParam,
	PolarityParam,
	OnOffParam,
	kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kIFaderDepth_L = 120,
  kIFaderDepth_Vert_X = 13,
  kIFaderDepth_Vert_Y = 148,
  kIFaderRate_L = 120,
  kIFaderRate_Vert_X = 78,
  kIFaderRate_Vert_Y = 148,
  kIFaderFeedback_L = 120,
  kIFaderFeedback_Vert_X = 148,
  kIFaderFeedback_Vert_Y = 147,
  kIFaderChorus_L = 120,
  kIFaderChorus_Vert_X = 218,
  kIFaderChorus_Vert_Y = 147,
  kICC_Depth_X = 1,
  kICC_Depth_Y = 270,
  kICC_Rate_X = 71,
  kICC_Rate_Y = 270,
  kICC_Feedback_X = 135,
  kICC_Feedback_Y = 270,
  kICC_Chorus_X = 214,
  kICC_Chorus_Y = 270,
  kICC_W = 70,
  kICC_H = 30,
  kIRadioButtonsControl_N = 2,
  kIRBC_W = 96,  // width of bitmap
  kIRBC_H = 26,  // height of one of the bitmap images + 2 for image border 
  kIRadioButtonsControl_V_X_LFO = 310,
  kIRadioButtonsControl_V_Y_LFO = 170,
  kIRadioButtonsControl_V_X_Mod = 310,
  kIRadioButtonsControl_V_Y_Mod = 46,
  kLFObuttons = 4,  // number of vertical buttons for waveform
  kModButtons = 3,  // number of vertical buttons for mode
  // ITextControl
  kITC_X = 115,  // fext for waveform 
  kITC_Y = 155,
  kITC_W = 100,
  kITC_H = 20,
  kITC_X_M = 235, // text for mode
  kISwitchControl_2_N = 2,  // # of sub-bitmaps.
  kISwitchControl_2_X = 75,  // position of left side of control
  kISwitchControl_2_Y = 55,  // position of top of control
};

int const channelCount = 2;


DFlanger::DFlanger(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
  TRACE;
  createGraphics();

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

DFlanger::~DFlanger() {}

void DFlanger::createGraphics() {
	IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
	pGraphics->AttachBackground(BG_ID, BG_FN);

	// Attach the ISwitchControl 2 image "switch" for on/off button
	IBitmap bitmap = pGraphics->LoadIBitmap(ISWITCHCONTROL_2_ID, ISWITCHCONTROL_2_FN, kISwitchControl_2_N);
	pGraphics->AttachControl(new ISwitchControl(this, kISwitchControl_2_X, kISwitchControl_2_Y, OnOffParam, &bitmap));

	// Attach the vertical fader controls 
	bitmap = pGraphics->LoadIBitmap(IFADERCONTROL_VERT_ID, IFADERCONTROL_VERT_FN);
	IText text = IText(14);
	// depth
	GetParam(DepthFaderParam)->InitDouble("IFaderControl Depth", 50, 0, 100, .1, "%");
	pGraphics->AttachControl(new IFaderControl(this, kIFaderDepth_Vert_X, kIFaderDepth_Vert_Y, kIFaderDepth_L, DepthFaderParam, &bitmap)); // kVertical is default
	ICaptionControl *captionControlDepth = new ICaptionControl(this, IRECT(kICC_Depth_X, kICC_Depth_Y, (kICC_Depth_X + kICC_W), (kICC_Depth_Y + kICC_H)), DepthFaderParam, &text);
	captionControlDepth->DisablePrompt(false);
	pGraphics->AttachControl(captionControlDepth);

	// rate
	GetParam(RateFaderParam)->InitDouble("IFaderControl Rate", .18, 0.02, 5, .1, "Hz");
	pGraphics->AttachControl(new IFaderControl(this, kIFaderRate_Vert_X, kIFaderRate_Vert_Y, kIFaderRate_L, RateFaderParam, &bitmap));
	ICaptionControl *captionControlRate = new ICaptionControl(this, IRECT(kICC_Rate_X, kICC_Rate_Y, (kICC_Rate_X + kICC_W), (kICC_Rate_Y + kICC_H)), RateFaderParam, &text);
	captionControlRate->DisablePrompt(false);
	pGraphics->AttachControl(captionControlRate);

	// feedback 
	GetParam(FeedbackFaderParam)->InitDouble("IFaderControl Feedback", 0, -100, 100, .1, "%");
	pGraphics->AttachControl(new IFaderControl(this, kIFaderFeedback_Vert_X, kIFaderFeedback_Vert_Y, kIFaderFeedback_L, FeedbackFaderParam, &bitmap));
	ICaptionControl *captionControlFeedback = new ICaptionControl(this, IRECT(kICC_Feedback_X, kICC_Feedback_Y, (kICC_Feedback_X + kICC_W), (kICC_Feedback_Y + kICC_H)), FeedbackFaderParam, &text);
	captionControlFeedback->DisablePrompt(false);
	pGraphics->AttachControl(captionControlFeedback);

	// chorus 
	GetParam(ChorusFaderParam)->InitDouble("IFaderControl Chorus", 0, 0, 30, .1, "%");
	pGraphics->AttachControl(new IFaderControl(this, kIFaderChorus_Vert_X, kIFaderChorus_Vert_Y, kIFaderChorus_L, ChorusFaderParam, &bitmap));
	ICaptionControl *captionControlChorus = new ICaptionControl(this, IRECT(kICC_Chorus_X, kICC_Chorus_Y, (kICC_Chorus_X + kICC_W), (kICC_Chorus_Y + kICC_H)), ChorusFaderParam, &text);
	captionControlChorus->DisablePrompt(false);
	pGraphics->AttachControl(captionControlChorus);


	GetParam(LFOParam)->InitInt("LFO Control", 0, 0, 3, "button"); // needed if using more than 2 radio buttons 
	GetParam(ModParam)->InitInt("Mod Control", 0, 0, 2, "button");
	// Attach the vertical IRadioButtonsControl
	bitmap = pGraphics->LoadIBitmap(IRADIOBUTTONSCONTROL_ID, IRADIOBUTTONSCONTROL_FN, kIRadioButtonsControl_N);
	pGraphics->AttachControl(new IRadioButtonsControl(this, IRECT(kIRadioButtonsControl_V_X_LFO, kIRadioButtonsControl_V_Y_LFO, kIRadioButtonsControl_V_X_LFO + (kIRBC_W*kLFObuttons), kIRadioButtonsControl_V_Y_LFO + (kIRBC_H*kLFObuttons)), LFOParam, kLFObuttons, &bitmap));
	pGraphics->AttachControl(new IRadioButtonsControl(this, IRECT(kIRadioButtonsControl_V_X_Mod, kIRadioButtonsControl_V_Y_Mod, kIRadioButtonsControl_V_X_Mod + (kIRBC_W*kModButtons), kIRadioButtonsControl_V_Y_Mod + (kIRBC_H*kModButtons)), ModParam, kModButtons, &bitmap));

	AttachGraphics(pGraphics);
}


void DFlanger::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
	// Mutex is already locked for us.

	double* in1 = inputs[0];
	double* in2 = inputs[1];
	double* out1 = outputs[0];
	double* out2 = outputs[1];

	for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
	{
		*out1 = modDelayGen.generate(in1); 

		//because we are working in mono we'll just copy the left output to the right output.
		*out2 = *out1;
	}
}

void DFlanger::Reset()
{
  TRACE;
  IMutexLock lock(this);
  modDelayGen.setSampleRate(this->GetSampleRate());
}

void DFlanger::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  switch (paramIdx)
  {
  case DepthFaderParam:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(DepthFaderParam, GetParam(paramIdx)->Value(), false);
	  }
	  modDelayGen.setModDepth(GetParam(paramIdx)->Value());
	  break;
  case RateFaderParam:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(RateFaderParam, GetParam(paramIdx)->Value(), false);
	  }
	  modDelayGen.updateLFO(GetParam(paramIdx)->Value(), (OscillatorGenerator::Waveform)(int)(GetParam(LFOParam)->Value()));
	  break;
  case FeedbackFaderParam:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(FeedbackFaderParam, GetParam(paramIdx)->Value(), false);
	  }
	  modDelayGen.updateDL(GetParam(paramIdx)->Value()); 
	  break;
  case ChorusFaderParam:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(ChorusFaderParam, GetParam(paramIdx)->Value(), false);
	  }
	  modDelayGen.setChorusOffset(GetParam(paramIdx)->Value());
	  break; 
  case OnOffParam:
	  break;
  case LFOParam: {
	  int wf = GetParam(paramIdx)->Value();
	  modDelayGen.updateLFO(GetParam(RateFaderParam)->Value(), (OscillatorGenerator::Waveform)wf); 
	  break;
  }
  case ModParam: {
	  int m = GetParam(paramIdx)->Value();
	  modDelayGen.setModType((ModDelayGenerator::Mod)m); 
	  break;
  }
  default:
	  break;
  }
}
