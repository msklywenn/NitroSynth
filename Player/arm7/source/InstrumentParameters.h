#ifndef _INSTRUMENTPARAMETERS_H_
#define _INSTRUMENTPARAMETERS_H_

struct sInstrumentParameters
{
	unsigned char iVolume;
	unsigned char iPanning;
	signed char iDetune;
	unsigned char iDuty;
	
	unsigned char iAttack;	// 1..255
	unsigned char iDecay;	// 1..255
	unsigned char iSustain;	// 0..127
	unsigned char iRelease;	// 1..255

	unsigned char iTremoloAmplitude;
	unsigned char iTremoloFrequency;

	unsigned char iPortamentoLength;

	unsigned char iSweepLength;
	signed short iSweepOffset;

	unsigned char iModulationAmplitude;
	unsigned char iModulationFrequency;

	unsigned char iArpeggio;
	unsigned char pad;
};

typedef struct sInstrumentParameters InstrumentParameters;

void InstrumentParameters_Init(InstrumentParameters * pParameters);

#endif // _INSTRUMENTPARAMETERS_H_
