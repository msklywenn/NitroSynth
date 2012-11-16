#ifndef _INSTRUMENT_H_
#define _INSTRUMENT_H_

#include "PSG.h"
#include "Envelope.h"

#pragma pack(push, 1)
struct InstrumentParameters
{
	typedef enum { 
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

	unsigned char iVolume;
	unsigned char iPanning;
	signed char iDetune;
	unsigned char iDuty;

	struct {
		unsigned char iAttack;	// 1..255
		unsigned char iDecay;	// 1..255
		unsigned char iSustain;	// 0..127
		unsigned char iRelease;	// 1..255
	} Envelope;

	struct {
		unsigned char iAmplitude;
		unsigned char iFrequency;
	} Tremolo;

	unsigned char iPortamentoLength;

	struct {
		unsigned char iLength;
		signed short iOffset;
	} Sweep;

	struct {
		unsigned char iAmplitude;
		unsigned char iFrequency;
	} Modulation;

	unsigned char iArpeggio;
	unsigned char pad;

	InstrumentParameters();
};
#pragma pack(pop)

class Instrument {
	InstrumentParameters* pParameters;

	PSG* pSoundGenerator;
	Envelope oEnvelope;

	float fTime;
	unsigned int iLastEnvelope;
	int iLastFrequency;
	int iLastNote;

	unsigned int pFrequencies[8];
	unsigned int iNbFrequencies;
	unsigned int iFrequency;
	unsigned int iArpeggioTimer;
	int iEffectTimer;
	int iSweepFrequency;
	int iModulationAmplitude;
	int iPortamentoFrequency;

	int iNoteCounter;

public:
	Instrument(PSG* pSoundGenerator, InstrumentParameters* pParameters);
	~Instrument();

	void Compute(float dt, int & iLeft, int & iRight);

	void NoteOn(int iNote, int iVelocity);
	void NoteOff(int iNote);

	void SetParameters(InstrumentParameters* p) { pParameters = p; }
};

#endif // _INSTRUMENT_H_