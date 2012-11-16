#include "Envelope.h"
#include "InstrumentParameters.h"

#define MAX_VOLUME 512

void Envelope_Init(Envelope * pEnvelope)
{
	pEnvelope->iTime = 0;
	pEnvelope->iLastEnvelope = 0;
	pEnvelope->iVelocity = 0;
	pEnvelope->iState = RELEASE;
}

unsigned int Envelope_Compute(Envelope * pEnvelope, InstrumentParameters * pParameters)
{
	unsigned int iEnvelope = 0;

	int iAttack = pParameters->iAttack;
	int iDecay = pParameters->iDecay;
	int iSustain = pParameters->iSustain * MAX_VOLUME / 127;
	int iRelease = pParameters->iRelease;

	pEnvelope->iTime++;

	switch ( pEnvelope->iState )
	{
		case ATTACK:
		{
			iEnvelope = (pEnvelope->iTime * MAX_VOLUME) / iAttack;
			if ( pEnvelope->iTime >= iAttack ) {
				pEnvelope->iTime = 0;
				pEnvelope->iState = DECAY;
			}
			break;
		}

		case DECAY:
		{
			int s = MAX_VOLUME - iSustain;
			iEnvelope = iSustain + s - ((pEnvelope->iTime * s) / iDecay);
			if ( pEnvelope->iTime >= iDecay ) {
				pEnvelope->iTime = 0;
				pEnvelope->iState = SUSTAIN;
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
			if ( pEnvelope->iTime <= iRelease )
				iEnvelope = pEnvelope->iLastEnvelope -
					((pEnvelope->iTime * pEnvelope->iLastEnvelope) / iRelease);
			break;
		}
	}

	if ( pEnvelope->iState != RELEASE )
		pEnvelope->iLastEnvelope = iEnvelope;

	iEnvelope = iEnvelope * pEnvelope->iVelocity / 127;

	return iEnvelope / (MAX_VOLUME / 128);
}

void Envelope_NoteOn(Envelope * pEnvelope, int v)
{
	pEnvelope->iVelocity = v;
	pEnvelope->iTime = 0;
	pEnvelope->iState = ATTACK;
}

void Envelope_NoteOff(Envelope * pEnvelope)
{
	if ( pEnvelope->iState != RELEASE )
	{
		pEnvelope->iTime = 0;
		pEnvelope->iState = RELEASE;
	}
}
