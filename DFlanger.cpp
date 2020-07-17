#include "DFlanger.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
	kDelayMS,
	kFeedbackPC,
	kWetPC,
	kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kGainX = 100,
  kGainY = 100,
  kKnobFrames = 60
};

int const channelCount = 2;


DFlanger::DFlanger(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
  TRACE;

  GetParam(kDelayMS)->InitDouble("Delay", 0., 0., 2000., 0.01, "Milliseconds");
  GetParam(kFeedbackPC)->InitDouble("Feedback", 50., 0., 100.0, 0.01, "%");
  GetParam(kWetPC)->InitDouble("Wet/Dry", 50., 0., 100.0, 0.01, "%");

  //arguments are: name, defaultVal, minVal, maxVal, step, label

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachPanelBackground(&COLOR_RED);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, 20, 200, kDelayMS, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, 80, 200, kFeedbackPC, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, 140, 200, kWetPC, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

DFlanger::~DFlanger() {}

void DFlanger::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
	// Mutex is already locked for us.

	double* in1 = inputs[0];
	double* in2 = inputs[1];
	double* out1 = outputs[0];
	double* out2 = outputs[1];

	for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
	{

		*out1 = (mWet * dGen.generateDelay(in1) + (1 - mWet) * *in1);
		//because we are working in mono we'll just copy the left output to the right output.
		*out2 = *out1;

	}
}

void DFlanger::Reset()
{
  TRACE;
  IMutexLock lock(this);
  dGen.setSampleRate(GetSampleRate()); 
}

void DFlanger::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  switch (paramIdx)
  {
  case kDelayMS: {
	  double delayValue = GetParam(kDelayMS)->Value();
	  dGen.setDelaySample(GetParam(kDelayMS)->Value() * GetSampleRate() / 1000);
	  break;
  }
  case kFeedbackPC:
	  dGen.setFeedback(GetParam(kFeedbackPC)->Value() / 100.0);
	  break;
  case kWetPC:
	  mWet = GetParam(kWetPC)->Value() / 100.0;
  }
}
