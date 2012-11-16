#ifndef _ENVELOPE_H_
#define _ENVELOPE_H_

struct InstrumentParameters;

class Envelope
{
	typedef enum State
	{
		ATTACK, DECAY, SUSTAIN, RELEASE
	};

	int iTime;
	int iLastEnvelope;

	int iVelocity;

	State iState;

public:
	Envelope();
	
	unsigned int Compute(InstrumentParameters * pParameters);

	void NoteOn(int velocity);
	void NoteOff();
};

#endif // _ENVELOPE_H_