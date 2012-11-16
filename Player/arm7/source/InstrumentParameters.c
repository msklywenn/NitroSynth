#include "InstrumentParameters.h"

void InstrumentParameters_Init(InstrumentParameters * pParameters)
{
	pParameters->iVolume = 64;
	pParameters->iPanning = 64;
	pParameters->iDetune = 0;
	pParameters->iDuty = 0;
	pParameters->iPortamentoLength = 0;
	pParameters->iAttack = 2;
	pParameters->iDecay = 16;
	pParameters->iSustain = 64;
	pParameters->iRelease = 5;
	pParameters->iTremoloAmplitude = 0;
	pParameters->iTremoloFrequency = 0;
	pParameters->iSweepOffset = 0;
	pParameters->iSweepLength = 0;
	pParameters->iModulationAmplitude = 0;
	pParameters->iModulationFrequency = 0;
	pParameters->iArpeggio = 0;
}
