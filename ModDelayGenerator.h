#pragma once

#include "OscillatorGenerator.h"
#include "DelayGenerator.h"

class ModDelayGenerator
{
public:
	ModDelayGenerator();
	~ModDelayGenerator();

	enum Mod { flanger, vibrato, chorus };
	void setModType(Mod modType); 
	void setModDepth(float modDepth_pct);
	void setChorusOffset(float chorusOffset); 
	void updateLFO(float modFrequency_HZ, OscillatorGenerator::Waveform lfoType);
	void updateDL(float feedBack_pct);
	float calculateDelayOffset(float LFOSample);
	void setSampleRate(float sample); 
	double generate(double* input); 

private:
	OscillatorGenerator lfo;
	DelayGenerator delayLine; 

	Mod modType = Mod::flanger; 

	// depend on type of mod
	float minDelay_Sec;
	float maxDelay_Sec; 
	float chorusOffset; 
	float modDepth; 

	void cookModType();

};

