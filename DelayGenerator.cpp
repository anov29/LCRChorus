#include "DelayGenerator.h"
#include "LinInterp.h"
#include "IControl.h"

DelayGenerator::DelayGenerator()
{
}


DelayGenerator::~DelayGenerator()
{
}

double DelayGenerator::generateDelay(double* input) 
{
	// first we read our delayed output
	double yn = mpBuffer[mReadIndex];

	// if delay < 1 sampe, interpolate between intpu x(n) and x(n-1)
	if (mReadIndex == mWriteIndex && mDelaySam < 1.00)
	{
		// interpolate x(n) with x(n-1), set yn = input 
		yn = *input; 
	}

	// read the location one behind yn at y(n-1)
	int readIndex_1 = mReadIndex - 1; 
	if (readIndex_1 < 0) readIndex_1 = mBufferSize - 1; // wrap around buffer 

	// get y(n-1)
	float yn_1 = mpBuffer[readIndex_1]; 

	// interpolate: (0, yn) and (1, yn_1) by the amount fracDelay
	float fracDelay = mDelaySam - (int)mDelaySam;

	// linerp: x1, x2, y1, x
	float linTerp = LinInterp::dLinTerp(0, 1, yn, yn_1, fracDelay); 

	// if the delay is 0 samples we just feed it the input
	if (mDelaySam == 0)
	{
		yn = *input;
		return *input;
	}
	else
	{
		yn = linTerp; 
	}

	// now we write to out delay buffer
	mpBuffer[mWriteIndex] = *input + mFeedback * yn;

	// then we increment the write index, wrapping if it goes out of bounds.
	++mWriteIndex;
	if (mWriteIndex >= mBufferSize)
	{
		mWriteIndex = 0;
	}

	// same with the read index
	++mReadIndex;
	if (mReadIndex >= mBufferSize)
	{
		mReadIndex = 0;
	}

	// now return the sample 
	return yn; 
}

void DelayGenerator::setDelaySample(double delaySam) 
{
	this->mDelaySam = delaySam; 
	mReadIndex = mWriteIndex - (int)mDelaySam;
	if (mReadIndex < 0)
	{
		mReadIndex += mBufferSize;
	}
}

void DelayGenerator::setFeedback(double feedback)
{
	this->mFeedback = feedback;
}

void DelayGenerator::setSampleRate(double sampleR) 
{
	this->sampleRate = sampleR;
	resetDelay(); 
}

void DelayGenerator::resetDelay() 
{
	mBufferSize = 2 * sampleRate;
	if (mpBuffer)
	{
		delete[] mpBuffer;
	}

	mpBuffer = new double[mBufferSize];

	if (mpBuffer)
	{
		memset(mpBuffer, 0, mBufferSize * sizeof(double));
	}

	mWriteIndex = 0;
	mReadIndex = 0;

}

