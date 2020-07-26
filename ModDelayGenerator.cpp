#include "ModDelayGenerator.h"



ModDelayGenerator::ModDelayGenerator()
{
	minDelay_Sec = 0;
	maxDelay_Sec = 0; 
	chorusOffset = 0;

	lfo.setFrequency(0);
	lfo.setWaveform(OscillatorGenerator::Waveform::sine);
	lfo.setMode(OscillatorGenerator::Mode::normal);
	lfo.setPolarity(OscillatorGenerator::Polarity::unipolar);

	delayLine.setDelaySample(0);
}

ModDelayGenerator::~ModDelayGenerator()
{
}

void ModDelayGenerator::updateLFO(float modFrequency_HZ, OscillatorGenerator::Waveform lfoType)
{
	lfo.setWaveform(lfoType);
	lfo.setFrequency(modFrequency_HZ);
}

void ModDelayGenerator::updateDL(float feedBack_pct)
{
	if (modType != Mod::vibrato) 
	{
		delayLine.setFeedback(feedBack_pct);
	}
}

void ModDelayGenerator::invertLFO(bool invert)
{
	lfo.invert(invert);
}

void ModDelayGenerator::setModType(Mod modType)
{
	this->modType = modType;
	switch (modType)
	{
	case ModDelayGenerator::flanger:
		minDelay_Sec = 0;
		maxDelay_Sec = 7;
		delayLine.setWetness(50.0);
		// no need to set feedback here as user will already have input it
		break;
	case ModDelayGenerator::vibrato:
		minDelay_Sec = 0;
		maxDelay_Sec = 7;
		delayLine.setWetness(100.0);
		delayLine.setFeedback(0.0);
		break;
	case ModDelayGenerator::chorus:
		minDelay_Sec = 5;
		maxDelay_Sec = 30;
		delayLine.setWetness(50.0);
		delayLine.setFeedback(0.0); // todo: ??
		break;
	default:
		minDelay_Sec = 0;
		maxDelay_Sec = 7;
		delayLine.setWetness(50.0);
		// no need to set feedback here as user will already have input it
		break;
	}

}

void ModDelayGenerator::setModDepth(float modDepth_pct)
{
	this->modDepth = modDepth_pct / 100; 
}

void ModDelayGenerator::setChorusOffset(float chorusOffset)
{
	this->chorusOffset = chorusOffset; 
}

void ModDelayGenerator::setPhase(Phase p)
{
	this->phaseType = p;
}

void ModDelayGenerator::setSampleRate(float sample)
{
	lfo.setSampleRate(sample);
	delayLine.setSampleRate(sample);
}

float ModDelayGenerator::calculateDelayOffset(float LFOSample) {
	if (modType == flanger || modType == vibrato)
	{
		return (modDepth * LFOSample * (maxDelay_Sec - minDelay_Sec)) + minDelay_Sec;
	}
	else if (modType == chorus)
	{
		float start = minDelay_Sec + chorusOffset;
		return modDepth * (LFOSample * (maxDelay_Sec - minDelay_Sec)) + start;
	}
}

double ModDelayGenerator::generate(double * input)
{
	// 1. Get LFO values, normal and quad phase
	double y = 0;
	double y_q = 0;
	lfo.generate(&y, &y_q); 

	// 2. Generate delay offset, for now ignore quad 
	float delay = 0.0;
	if (phaseType == Phase::quad) 
	{
		delay = calculateDelayOffset(y_q);
	}
	else 
	{
		delay = calculateDelayOffset(y); 
	}
	
	// 3. Set the delay
	delayLine.setDelaySample(delay);

	// 4. Get the delay output 
	return delayLine.generateDelay(input);
}
