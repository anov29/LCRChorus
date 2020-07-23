#pragma once


class DelayGenerator
{
public:
	DelayGenerator();
	~DelayGenerator();
	double generateDelay(double* input);
	void setDelaySample(double delaySam_ms); 
	void setFeedback(double feedback_pct); 
	void setSampleRate(double sr);
	void resetDelay(); 
	void setWetness(float wetness_pct);

private:
	double mFeedback = 0.;
	double mDelaySam = 0.;
	float wetLevel; 

	double* mpBuffer = nullptr;
	int mReadIndex = 0;
	int mWriteIndex = 0;
	int mBufferSize = 0;
	int sampleRate = 44100; 
};

