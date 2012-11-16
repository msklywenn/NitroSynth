#include "Instrument.h"
#include "Utils.h"
#include <nds.h>

#define SOUND_PSG_DUTY(duty) ((duty)<<24)

void Instrument_Init(Instrument* pInstrument, int iChannel, InstrumentParameters* pParameters)
{
	pInstrument->pParameters = pParameters;
	pInstrument->iChannel = iChannel;
	pInstrument->iLastFrequency = 0;
	pInstrument->pFrequencies[0] = 1;
	pInstrument->iFrequency = 0;
	pInstrument->iNbFrequencies = 0;
	pInstrument->iEffectTimer = 0;
	pInstrument->iSweepFrequency = 0;
	pInstrument->iModulationAmplitude = 0;
	pInstrument->iPortamentoFrequency = 0;
	pInstrument->iArpeggioTimer = 0;

	Envelope_Init(&pInstrument->oEnvelope);
}

void Instrument_Compute(Instrument* pInstrument)
{
	InstrumentParameters* pParameters = pInstrument->pParameters;

	int iVolume = pParameters->iVolume;

	if ( pParameters->iTremoloAmplitude != 0 )
	{
		int iSin = Sinus(pInstrument->iEffectTimer * 12868 * pParameters->iTremoloFrequency / 256) / 2 + 2048;
		int iAmplitude = pParameters->iTremoloAmplitude * pParameters->iVolume / 255;
		iVolume -= iSin * iAmplitude / 4096;
	}

	iVolume = Envelope_Compute(& pInstrument->oEnvelope, pParameters) * iVolume / 127;

	if ( pParameters->iArpeggio > 0 )
	{
		pInstrument->iArpeggioTimer++;
		if ( pInstrument->iArpeggioTimer >= pParameters->iArpeggio )
		{
			pInstrument->iArpeggioTimer = 0;
			pInstrument->iFrequency++;
			if ( pInstrument->iFrequency >= pInstrument->iNbFrequencies )
				pInstrument->iFrequency = 0;
		}
	}

	pInstrument->iEffectTimer++;

	int iSweep = 0;
	if ( pParameters->iSweepLength != 0
			&& pInstrument->iEffectTimer < pParameters->iSweepLength )
		iSweep = pInstrument->iSweepFrequency - (pInstrument->iEffectTimer * pInstrument->iSweepFrequency) / pParameters->iSweepLength;

	int iModulation = Sinus(pInstrument->iEffectTimer * 12868 * pParameters->iModulationFrequency / 256) * pInstrument->iModulationAmplitude / 4096;

	int iPortamento = 0;
	if ( pParameters->iPortamentoLength != 0 && pInstrument->iEffectTimer < pParameters->iPortamentoLength )
		iPortamento = pInstrument->iPortamentoFrequency - (pInstrument->iEffectTimer * pInstrument->iPortamentoFrequency) / pParameters->iPortamentoLength;

	pInstrument->iLastFrequency = pInstrument->pFrequencies[pInstrument->iFrequency] + iSweep + iModulation + iPortamento;

	if ( pInstrument->iLastFrequency <= 0 )
		pInstrument->iLastFrequency = 1;

	SCHANNEL_CR(pInstrument->iChannel) = SCHANNEL_ENABLE
		| SOUND_VOL(iVolume)
		| SOUND_PAN(pParameters->iPanning)
		| SOUND_FORMAT_PSG
		| SOUND_PSG_DUTY(pParameters->iDuty)
		;
	SCHANNEL_TIMER(pInstrument->iChannel) = SOUND_FREQ(pInstrument->iLastFrequency * 8);
}

void Instrument_NoteOn(Instrument* pInstrument, int iNote, int iVelocity)
{
	int n = iNote + pInstrument->pParameters->iDetune;

	Envelope_NoteOn(& pInstrument->oEnvelope, iVelocity);

	if ( pInstrument->iEffectTimer < 3
		&& pInstrument->iNbFrequencies <= sizeof(pInstrument->pFrequencies)/sizeof(pInstrument->pFrequencies[0]) )
	{
		// Arpeggio !
		pInstrument->pFrequencies[pInstrument->iNbFrequencies++] = MidiNoteToFrequency(n);
	}
	else
	{
		pInstrument->pFrequencies[0] = MidiNoteToFrequency(n);
		pInstrument->iNbFrequencies = 1;
		pInstrument->iArpeggioTimer = 0;
		pInstrument->iEffectTimer = 0;
		pInstrument->iFrequency = 0;
		if ( pInstrument->iLastFrequency == 0 )
			pInstrument->iLastFrequency = pInstrument->pFrequencies[0];
		pInstrument->iPortamentoFrequency = pInstrument->iLastFrequency - pInstrument->pFrequencies[0];
		pInstrument->iSweepFrequency = MidiNoteToFrequency(n + pInstrument->pParameters->iSweepOffset) - pInstrument->pFrequencies[0];
		pInstrument->iModulationAmplitude = MidiNoteToFrequency(n + pInstrument->pParameters->iModulationAmplitude) - pInstrument->pFrequencies[0];
	}
}

void Instrument_NoteOff(Instrument* pInstrument)
{
	Envelope_NoteOff(& pInstrument->oEnvelope);
}

