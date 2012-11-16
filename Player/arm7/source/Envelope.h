#ifndef _ENVELOPE_H_
#define _ENVELOPE_H_

#include "InstrumentParameters.h"

typedef enum
{
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE
}
EnvelopeState;

typedef struct
{
	int iTime;
	int iLastEnvelope;

	int iVelocity;

	EnvelopeState iState;
}
Envelope;

void Envelope_Init(Envelope * pEnvelope);
unsigned int Envelope_Compute(Envelope * pEnvelope, InstrumentParameters * pParameters);
void Envelope_NoteOn(Envelope * pEnvelope, int velocity);
void Envelope_NoteOff(Envelope * pEnvelope);

#endif // _ENVELOPE_H_
