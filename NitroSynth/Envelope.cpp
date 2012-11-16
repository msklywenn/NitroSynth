#include "Envelope.h"
#include "Instrument.h"

#define MAX_VOLUME 512

Envelope::Envelope()
:	iTime(0),
	iLastEnvelope(0),
	iVelocity(0),
	iState(RELEASE)
{
}

unsigned int Envelope::Compute(InstrumentParameters * pParameters)
{
	unsigned int iEnvelope = 0;

	int iAttack = pParameters->Envelope.iAttack;
	int iDecay = pParameters->Envelope.iDecay;
	int iSustain = pParameters->Envelope.iSustain * MAX_VOLUME / 127;
	int iRelease = pParameters->Envelope.iRelease;

	iTime++;

	switch ( iState )
	{
		case ATTACK:
		{
			iEnvelope = (iTime * MAX_VOLUME) / iAttack;
			if ( iTime >= iAttack ) {
				iTime = 0;
				iState = DECAY;
			}
			break;
		}

		case DECAY:
		{
			int s = MAX_VOLUME - iSustain;
			iEnvelope = iSustain + s - ((iTime * s) / iDecay);
			if ( iTime >= iDecay ) {
				iTime = 0;
				iState = SUSTAIN;
			}
			break;
		}

		case SUSTAIN:
		{
			iEnvelope = iSustain;
			break;
		}

		case RELEASE:
		{
			if ( iTime <= iRelease )
				iEnvelope = iLastEnvelope - ((iTime * iLastEnvelope) / iRelease);
			break;
		}
	}

	if ( iState != RELEASE )
		iLastEnvelope = iEnvelope;

	iEnvelope = iEnvelope * iVelocity / 127;

	return iEnvelope / (MAX_VOLUME / 128);
}

void Envelope::NoteOff()
{
	iTime = 0;
	iState = RELEASE;
}

void Envelope::NoteOn(int v)
{
	iVelocity = v;
	iTime = 0;
	iState = ATTACK;
}