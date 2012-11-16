#ifndef _INSTRUMENT_H_
#define _INSTRUMENT_H_

#include "Envelope.h"
#include "InstrumentParameters.h"

enum { 
	VOLUME,
	PANNING,
	DETUNE,
	ENVELOPE_ATTACK,
	ENVELOPE_DECAY,
	ENVELOPE_SUSTAIN,
	ENVELOPE_RELEASE,
	DUTY,
	PORTAMENTO_LENGTH,
	ARPEGGIO,
	SWEEP_LENGTH,
	SWEEP_OFFSET,
	MODULATION_AMPLITUDE,
	MODULATION_FREQUENCY,
	TREMOLO_AMPLITUDE,
	TREMOLO_FREQUENCY,
	LAST,
	NOTEON = 29,	// for export
	NOTEOFF = 30,	// for export
	END = 31		// for export
};

struct sInstrument {
	InstrumentParameters * pParameters;

	int iChannel;
	Envelope oEnvelope;

	float fTime;
	int iLastFrequency;

	unsigned int pFrequencies[8];
	unsigned int iNbFrequencies;
	unsigned int iFrequency;
	unsigned int iArpeggioTimer;
	int iEffectTimer;
	int iSweepFrequency;
	int iModulationAmplitude;
	int iPortamentoFrequency;
};

typedef struct sInstrument Instrument;

void Instrument_Init(Instrument * pInstrument, int iChannel, InstrumentParameters * pParameters);

void Instrument_Compute(Instrument * pInstrument);

void Instrument_NoteOn(Instrument * pInstrument, int iNote, int iVelocity);
void Instrument_NoteOff(Instrument * pInstrument);

void Instrument_SetParameters(Instrument * pInstrument, InstrumentParameters * p);

#endif // _INSTRUMENT_H_
