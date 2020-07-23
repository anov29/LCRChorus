#ifndef __DFLANGER__
#define __DFLANGER__

#include "IPlug_include_in_plug_hdr.h"
#include "ModDelayGenerator.h"

class DFlanger : public IPlug
{
public:
  DFlanger(IPlugInstanceInfo instanceInfo);
  ~DFlanger();

  ModDelayGenerator modDelayGen; 

  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void Reset();
  void createGraphics();

private:
	double mFeedback = 0.;
	double mDelaySam = 0.;
	double mWet = 0.;

	double* mpBuffer = NULL;
	int mReadIndex = 0;
	int mWriteIndex = 0;
	int mBufferSize = 0;
};

#endif
