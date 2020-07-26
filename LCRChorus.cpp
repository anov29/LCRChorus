#include "LCRChorus.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
	LDepthFaderParam,
	CDepthFaderParam,
	RDepthFaderParam,
	LRateFaderParam,
	CRateFaderParam,
	RRateFaderParam,
	LFeedbackFaderParam,
	CFeedbackFaderParam,
	RFeedbackFaderParam,
	ChorusFaderParam, 
	LFOParam,
	PolarityParam,
	OnOffParam,
	KnobGainControl, 
	KnobDryWetControl,
	kNumParams
};

enum ELayout
{
	kWidth = GUI_WIDTH,
	kHeight = GUI_HEIGHT,

	l_FaderDepth_L = 120,
	l_FaderDepth_Vert_X = 5,
	l_FaderDepth_Vert_Y = 157,
	c_FaderDepth_L = 120,
	c_FaderDepth_Vert_X = 169,
	c_FaderDepth_Vert_Y = 157,
	r_FaderDepth_L = 120,
	r_FaderDepth_Vert_X = 340,
	r_FaderDepth_Vert_Y = 157,
	l_FaderRate_L = 120,
	l_FaderRate_Vert_X = 51,
	l_FaderRate_Vert_Y = 157,
	c_FaderRate_L = 120,
	c_FaderRate_Vert_X = 216,
	c_FaderRate_Vert_Y = 157,
	r_FaderRate_L = 120,
	r_FaderRate_Vert_X = 386,
	r_FaderRate_Vert_Y = 157,
	l_FaderFeedback_L = 120,
	l_FaderFeedback_Vert_X = 105,
	l_FaderFeedback_Vert_Y = 157,
	c_FaderFeedback_L = 120,
	c_FaderFeedback_Vert_X = 270,
	c_FaderFeedback_Vert_Y = 157,
	r_FaderFeedback_L = 120,
	r_FaderFeedback_Vert_X = 440,
	r_FaderFeedback_Vert_Y = 157,
	l_CC_Depth_X = -3,
	l_CC_Depth_Y = 277,
	c_CC_Depth_X = 160,
	c_CC_Depth_Y = 277,
	r_CC_Depth_X = 330,
	r_CC_Depth_Y = 277,
	l_CC_Rate_X = 45,
	l_CC_Rate_Y = 277,
	c_CC_Rate_X = 209,
	c_CC_Rate_Y = 277,
	r_CC_Rate_X = 377,
	r_CC_Rate_Y = 277,
	l_CC_Feedback_X = 97,
	l_CC_Feedback_Y = 277,
	c_CC_Feedback_X = 265,
	c_CC_Feedback_Y = 277,
	r_CC_Feedback_X = 430,
	r_CC_Feedback_Y = 277,
	kICC_W = 70,
	kICC_H = 30,
	kIRadioButtonsControl_N = 2,
	kIRBC_W = 96,  // width of bitmap
	kIRBC_H = 26,  // height of one of the bitmap images + 2 for image border 
	kIRadioButtonsControl_V_X_LFO = 317,
	kIRadioButtonsControl_V_Y_LFO = 40,
	kLFObuttons = 2,  // number of vertical buttons for waveform
	// ITextControl
	kISwitchControl_2_N = 2,  // # of sub-bitmaps.
	kISwitchControl_2_X = 75,  // position of left side of control
	kISwitchControl_2_Y = 55,  // position of top of control
	knob_Gain_X = 165,
    knob_Gain_Y = 48,
	CC_Gain_X = 150,
	CC_Gain_Y = 85,
	knob_DW_X = 245,
	knob_DW_Y = 48,
	CC_DW_X = 233,
	CC_DW_Y = 85,
	knob_Frames = 64
};

int const channelCount = 2;


LCRChorus::LCRChorus(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
  TRACE;
  createGraphics();

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);

  modDelayLeft.setPhase(ModDelayGenerator::Phase::quad);
  modDelayCenter.setPhase(ModDelayGenerator::Phase::normal);
  modDelayLeft.setPhase(ModDelayGenerator::Phase::quad);

  modDelayLeft.invertLFO(false); 
  modDelayCenter.invertLFO(false);
  modDelayRight.invertLFO(true);

  modDelayLeft.setModType(ModDelayGenerator::Mod::chorus);
  modDelayCenter.setModType(ModDelayGenerator::Mod::chorus);
  modDelayRight.setModType(ModDelayGenerator::Mod::chorus);
}

LCRChorus::~LCRChorus() {}

void LCRChorus::createGraphics() {
	IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
	pGraphics->AttachBackground(BG_ID, BG_FN);

	// Attach the ISwitchControl 2 image "switch" for on/off button
	IBitmap bitmap = pGraphics->LoadIBitmap(ISWITCHCONTROL_2_ID, ISWITCHCONTROL_2_FN, kISwitchControl_2_N);
	pGraphics->AttachControl(new ISwitchControl(this, kISwitchControl_2_X, kISwitchControl_2_Y, OnOffParam, &bitmap));

	// Attach the vertical fader controls 
	bitmap = pGraphics->LoadIBitmap(IFADERCONTROL_VERT_ID, IFADERCONTROL_VERT_FN);
	IText text = IText(14);

	// depth L
	GetParam(LDepthFaderParam)->InitDouble("ChorusL Depth", 50, 0, 100, .1, "%");
	pGraphics->AttachControl(new IFaderControl(this, l_FaderDepth_Vert_X, l_FaderDepth_Vert_Y, l_FaderDepth_L, LDepthFaderParam, &bitmap)); // kVertical is default
	ICaptionControl *captionControlLDepth = new ICaptionControl(this, IRECT(l_CC_Depth_X, l_CC_Depth_Y, (l_CC_Depth_X + kICC_W), (l_CC_Depth_Y + kICC_H)), LDepthFaderParam, &text);
	captionControlLDepth->DisablePrompt(false);
	pGraphics->AttachControl(captionControlLDepth);

	// rate L 
	GetParam(LRateFaderParam)->InitDouble("ChorusL Rate", .18, 0.02, 5, .1, "Hz");
	pGraphics->AttachControl(new IFaderControl(this, l_FaderRate_Vert_X, l_FaderRate_Vert_Y, l_FaderRate_L, LRateFaderParam, &bitmap));
	ICaptionControl *captionControlLRate = new ICaptionControl(this, IRECT(l_CC_Rate_X, l_CC_Rate_Y, (l_CC_Rate_X + kICC_W), (l_CC_Rate_Y + kICC_H)), LRateFaderParam, &text);
	captionControlLRate->DisablePrompt(false);
	pGraphics->AttachControl(captionControlLRate);

	// feedback L
	GetParam(LFeedbackFaderParam)->InitDouble("ChorusL Feedback", 0, -100, 100, .1, "%");
	pGraphics->AttachControl(new IFaderControl(this, l_FaderFeedback_Vert_X, l_FaderFeedback_Vert_Y, l_FaderFeedback_L, LFeedbackFaderParam, &bitmap));
	ICaptionControl *captionControlLFeedback = new ICaptionControl(this, IRECT(l_CC_Feedback_X, l_CC_Feedback_Y, (l_CC_Feedback_X + kICC_W), (l_CC_Feedback_Y + kICC_H)), LFeedbackFaderParam, &text);
	captionControlLFeedback->DisablePrompt(false);
	pGraphics->AttachControl(captionControlLFeedback);

	// depth C
	GetParam(CDepthFaderParam)->InitDouble("ChorusC Depth", 50, 0, 100, .1, "%");
	pGraphics->AttachControl(new IFaderControl(this, c_FaderDepth_Vert_X, c_FaderDepth_Vert_Y, c_FaderDepth_L, CDepthFaderParam, &bitmap)); // kVertical is default
	ICaptionControl *captionControlCDepth = new ICaptionControl(this, IRECT(c_CC_Depth_X, c_CC_Depth_Y, (c_CC_Depth_X + kICC_W), (c_CC_Depth_Y + kICC_H)), CDepthFaderParam, &text);
	captionControlCDepth->DisablePrompt(false);
	pGraphics->AttachControl(captionControlCDepth);

	// rate C
	GetParam(CRateFaderParam)->InitDouble("ChorusC Rate", .18, 0.02, 5, .1, "Hz");
	pGraphics->AttachControl(new IFaderControl(this, c_FaderRate_Vert_X, c_FaderRate_Vert_Y, c_FaderRate_L, CRateFaderParam, &bitmap));
	ICaptionControl *captionControlCRate = new ICaptionControl(this, IRECT(c_CC_Rate_X, c_CC_Rate_Y, (c_CC_Rate_X + kICC_W), (c_CC_Rate_Y + kICC_H)), CRateFaderParam, &text);
	captionControlCRate->DisablePrompt(false);
	pGraphics->AttachControl(captionControlCRate);

	// feedback C
	GetParam(CFeedbackFaderParam)->InitDouble("ChorusC Feedback", 0, -100, 100, .1, "%");
	pGraphics->AttachControl(new IFaderControl(this, c_FaderFeedback_Vert_X, c_FaderFeedback_Vert_Y, c_FaderFeedback_L, CFeedbackFaderParam, &bitmap));
	ICaptionControl *captionControlCFeedback = new ICaptionControl(this, IRECT(c_CC_Feedback_X, c_CC_Feedback_Y, (c_CC_Feedback_X + kICC_W), (c_CC_Feedback_Y + kICC_H)), CFeedbackFaderParam, &text);
	captionControlCFeedback->DisablePrompt(false);
	pGraphics->AttachControl(captionControlCFeedback);

	// depth R
	GetParam(RDepthFaderParam)->InitDouble("ChorusR Depth", 50, 0, 100, .1, "%");
	pGraphics->AttachControl(new IFaderControl(this, r_FaderDepth_Vert_X, r_FaderDepth_Vert_Y, r_FaderDepth_L, RDepthFaderParam, &bitmap)); // kVertical is default
	ICaptionControl *captionControlRDepth = new ICaptionControl(this, IRECT(r_CC_Depth_X, r_CC_Depth_Y, (r_CC_Depth_X + kICC_W), (r_CC_Depth_Y + kICC_H)), RDepthFaderParam, &text);
	captionControlRDepth->DisablePrompt(false);
	pGraphics->AttachControl(captionControlRDepth);

	// rate R
	GetParam(RRateFaderParam)->InitDouble("ChorusR Rate", .18, 0.02, 5, .1, "Hz");
	pGraphics->AttachControl(new IFaderControl(this, r_FaderRate_Vert_X, r_FaderRate_Vert_Y, r_FaderRate_L, RRateFaderParam, &bitmap));
	ICaptionControl *captionControlRRate = new ICaptionControl(this, IRECT(r_CC_Rate_X, r_CC_Rate_Y, (r_CC_Rate_X + kICC_W), (r_CC_Rate_Y + kICC_H)), RRateFaderParam, &text);
	captionControlRRate->DisablePrompt(false);
	pGraphics->AttachControl(captionControlRRate);

	// feedback R
	GetParam(RFeedbackFaderParam)->InitDouble("ChorusR Feedback", 0, -100, 100, .1, "%");
	pGraphics->AttachControl(new IFaderControl(this, r_FaderFeedback_Vert_X, r_FaderFeedback_Vert_Y, r_FaderFeedback_L, RFeedbackFaderParam, &bitmap));
	ICaptionControl *captionControlRFeedback = new ICaptionControl(this, IRECT(r_CC_Feedback_X, r_CC_Feedback_Y, (r_CC_Feedback_X + kICC_W), (r_CC_Feedback_Y + kICC_H)), RFeedbackFaderParam, &text);
	captionControlRFeedback->DisablePrompt(false);
	pGraphics->AttachControl(captionControlRFeedback);

	// gain knob
	GetParam(KnobGainControl)->InitDouble("Gain", 50, 0., 100.0, 0.01, "%");
	GetParam(KnobGainControl)->SetShape(1);
	bitmap = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, knob_Frames);
	pGraphics->AttachControl(new IKnobMultiControl(this, knob_Gain_X, knob_Gain_Y, KnobGainControl, &bitmap));
	ICaptionControl *captionControlGain = new ICaptionControl(this, IRECT(CC_Gain_X, CC_Gain_Y, (CC_Gain_X + kICC_W), (CC_Gain_Y + kICC_H)), KnobGainControl, &text);
	captionControlGain->DisablePrompt(false);
	pGraphics->AttachControl(captionControlGain);

	// dry/wet knob
	GetParam(KnobDryWetControl)->InitDouble("Wetness", 50, 0., 100.0, 0.01, "%");
	GetParam(KnobDryWetControl)->SetShape(1);
	pGraphics->AttachControl(new IKnobMultiControl(this, knob_DW_X, knob_DW_Y, KnobDryWetControl, &bitmap));
	ICaptionControl *captionControlDW = new ICaptionControl(this, IRECT(CC_DW_X, CC_DW_Y, (CC_DW_X + kICC_W), (CC_DW_Y + kICC_H)), KnobDryWetControl, &text);
	captionControlDW->DisablePrompt(false);
	pGraphics->AttachControl(captionControlDW);

	// GetParam(LFOParam)->InitInt("LFO Control", 0, 0, 3, "button"); // needed if using more than 2 radio buttons 
	// Attach the lfo selector
	bitmap = pGraphics->LoadIBitmap(IRADIOBUTTONSCONTROL_ID, IRADIOBUTTONSCONTROL_FN, kIRadioButtonsControl_N);
	pGraphics->AttachControl(new IRadioButtonsControl(this, IRECT(kIRadioButtonsControl_V_X_LFO, kIRadioButtonsControl_V_Y_LFO, kIRadioButtonsControl_V_X_LFO + (kIRBC_W*kLFObuttons), kIRadioButtonsControl_V_Y_LFO + (kIRBC_H*kLFObuttons)), LFOParam, kLFObuttons, &bitmap));

	AttachGraphics(pGraphics);
}


void LCRChorus::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
	// Mutex is already locked for us.

	double* in1 = inputs[0];
	double* in2 = inputs[1];
	double* out1 = outputs[0];
	double* out2 = outputs[1];

	for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
	{
		if (on)
		{

			double chorusOutL = 0;
			double chorusOutC = 0;
			double chorusOutR = 0;

			double centerIn = .5 * *in1 + .5 * *in2; // center channel 
			chorusOutL = modDelayLeft.generate(in1);
			chorusOutC = modDelayCenter.generate(&centerIn);
			chorusOutR = modDelayRight.generate(in2);

			*out1 = ((wet * (chorusOutL + chorusOutC)) + ((1.0 - wet) * *in1)) * gain;
			*out2 = ((wet * (chorusOutR + chorusOutC)) + ((1.0 - wet) * *in1)) * gain;
		}
		else
		{
			*out1 = 0;
			*out2 = 0; 
		}
	}
}

void LCRChorus::Reset()
{
  TRACE;
  IMutexLock lock(this);
  modDelayLeft.setSampleRate(this->GetSampleRate());
  modDelayCenter.setSampleRate(this->GetSampleRate());
  modDelayRight.setSampleRate(this->GetSampleRate());
}

void LCRChorus::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  switch (paramIdx)
  {
  case LDepthFaderParam:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(LDepthFaderParam, GetParam(paramIdx)->Value(), false);
	  }
	  modDelayLeft.setModDepth(GetParam(paramIdx)->Value());
	  break;
  case CDepthFaderParam:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(CDepthFaderParam, GetParam(paramIdx)->Value(), false);
	  }
	  modDelayCenter.setModDepth(GetParam(paramIdx)->Value());
	  break; 
  case RDepthFaderParam:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(RDepthFaderParam, GetParam(paramIdx)->Value(), false);
	  }
	  modDelayRight.setModDepth(GetParam(paramIdx)->Value());
	  break;
  case LRateFaderParam:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(LRateFaderParam, GetParam(paramIdx)->Value(), false);
	  }
	  modDelayLeft.updateLFO(GetParam(paramIdx)->Value(), (OscillatorGenerator::Waveform)(int)(GetParam(LFOParam)->Value()));
	  break;
  case CRateFaderParam:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(CRateFaderParam, GetParam(paramIdx)->Value(), false);
	  }
	  modDelayCenter.updateLFO(GetParam(paramIdx)->Value(), (OscillatorGenerator::Waveform)(int)(GetParam(LFOParam)->Value()));
	  break;
  case RRateFaderParam:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(RRateFaderParam, GetParam(paramIdx)->Value(), false);
	  }
	  modDelayRight.updateLFO(GetParam(paramIdx)->Value(), (OscillatorGenerator::Waveform)(int)(GetParam(LFOParam)->Value()));
	  break;
  case LFeedbackFaderParam:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(LFeedbackFaderParam, GetParam(paramIdx)->Value(), false);
	  }
	  modDelayLeft.updateDL(GetParam(paramIdx)->Value()); 
	  break;
  case CFeedbackFaderParam:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(CFeedbackFaderParam, GetParam(paramIdx)->Value(), false);
	  }
	  modDelayCenter.updateDL(GetParam(paramIdx)->Value());
	  break;
  case RFeedbackFaderParam:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(RFeedbackFaderParam, GetParam(paramIdx)->Value(), false);
	  }
	  modDelayRight.updateDL(GetParam(paramIdx)->Value());
	  break;
  case OnOffParam:
	  if (GetParam(paramIdx)->Value() == 0)
	  {
		  on = true;
	  } 
	  else
	  {
		  on = false; 
	  }
	  break;
  case LFOParam: { 
	  int wf = GetParam(paramIdx)->Value();
	  modDelayLeft.updateLFO(GetParam(LRateFaderParam)->Value(), (OscillatorGenerator::Waveform)wf); 
	  modDelayCenter.updateLFO(GetParam(LRateFaderParam)->Value(), (OscillatorGenerator::Waveform)wf);
	  modDelayRight.updateLFO(GetParam(LRateFaderParam)->Value(), (OscillatorGenerator::Waveform)wf);
	  break;
  }
  case KnobGainControl:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(KnobGainControl, GetParam(paramIdx)->Value(), false);
	  }
	  gain = GetParam(paramIdx)->Value() / 100;
	  break;
  case KnobDryWetControl:
	  if (GetGUI()) {
		  GetGUI()->SetParameterFromPlug(KnobDryWetControl, GetParam(paramIdx)->Value(), false);
	  }
	  wet = GetParam(paramIdx)->Value() / 100;
	  break; 
  default:
	  break;
  }
}
