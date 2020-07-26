#ifndef __LCRCHORUS__
#define __LCRCHORUS__

#include "IPlug_include_in_plug_hdr.h"
#include "ModDelayGenerator.h"

class LCRChorus : public IPlug
{
public:
  LCRChorus(IPlugInstanceInfo instanceInfo);
  ~LCRChorus();

  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void Reset();
  void createGraphics();

private:
	ModDelayGenerator modDelayLeft;
	ModDelayGenerator modDelayCenter;
	ModDelayGenerator modDelayRight;

	void updateModules();

	double mFeedback = 0.;
	double mDelaySam = 0.;
	double mWet = 0.;

	double* mpBuffer = NULL;
	int mReadIndex = 0;
	int mWriteIndex = 0;
	int mBufferSize = 0;
};

#endif
