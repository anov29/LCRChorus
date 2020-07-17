#pragma once


class DelayGenerator
{
public:
	DelayGenerator();
	~DelayGenerator();
	double generateDelay(double* input);
	void setDelaySample(double delaySam); 
	void setFeedback(double feedback); 
	void setSampleRate(double sr);
	void resetDelay(); 
	void cookVars();

private:
	double mFeedback = 0.;
	double mDelaySam = 0.;

	double* mpBuffer = nullptr;
	int mReadIndex = 0;
	int mWriteIndex = 0;
	int mBufferSize = 0;
	int sampleRate = 44100; 
};

