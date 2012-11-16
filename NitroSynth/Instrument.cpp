#include "Instrument.h"
#include "Utils.h"

Instrument::Instrument(PSG* pSoundGenerator, InstrumentParameters* pParameters)
:	pParameters(pParameters),
	pSoundGenerator(pSoundGenerator),
	fTime(0.0f),
	iLastEnvelope(0),
	iLastFrequency(0),
	iLastNote(-1),
	iNbFrequencies(0),
	iFrequency(0),
	iArpeggioTimer(0),
	iEffectTimer(0),
	iSweepFrequency(0),
	iModulationAmplitude(0),
	iPortamentoFrequency(0),
	iNoteCounter(0)
{
	pFrequencies[0] = 1;
}

Instrument::~Instrument()
{
	delete pSoundGenerator;
}

void Instrument::Compute(float dt, int & iLeft, int & iRight)
{
	fTime += dt;

	if ( fTime > 1.0f / 60.0f )
	{
		fTime -= 1.0f/60.0f;

		int iVolume = pParameters->iVolume;
		if ( pParameters->Tremolo.iAmplitude != 0 )
		{
			int iSin = Sinus(iEffectTimer * 12868 * pParameters->Tremolo.iFrequency / 256) / 2 + 2048;
			int iAmplitude = pParameters->Tremolo.iAmplitude * pParameters->iVolume / 255;
			iVolume -= iSin * iAmplitude / 4096;
		}

		iLastEnvelope = oEnvelope.Compute(pParameters) * iVolume / 127;

		if ( pParameters->iArpeggio > 0 )
		{
			iArpeggioTimer++;
			if ( iArpeggioTimer >= pParameters->iArpeggio )
			{
				iArpeggioTimer = 0;
				iFrequency++;
				if ( iFrequency >= iNbFrequencies )
					iFrequency = 0;
			}
		}

		iEffectTimer++;

		int iSweep = 0;
		if ( pParameters->Sweep.iLength != 0 && iEffectTimer < pParameters->Sweep.iLength )
			iSweep = iSweepFrequency - (iEffectTimer * iSweepFrequency) / pParameters->Sweep.iLength;

		int iModulation = Sinus(iEffectTimer * 12868 * pParameters->Modulation.iFrequency / 256) * iModulationAmplitude / 4096;

		int iPortamento = 0;
		if ( pParameters->iPortamentoLength != 0 && iEffectTimer < pParameters->iPortamentoLength )
			iPortamento = iPortamentoFrequency - (iEffectTimer * iPortamentoFrequency) / pParameters->iPortamentoLength;

		iLastFrequency = pFrequencies[iFrequency] + iSweep + iModulation + iPortamento;
	}

	pSoundGenerator->SetDuty(pParameters->iDuty);
	pSoundGenerator->SetFrequency(iLastFrequency);

	signed short iSample = pSoundGenerator->Compute(dt);
	iSample = iSample * iLastEnvelope / 128;

	iRight += iSample * pParameters->iPanning / 128;
	iLeft += iSample * (128 - pParameters->iPanning) / 128;
}

void Instrument::NoteOn(int iNote, int iVelocity)
{
	iLastNote = iNote;
	int n = iNote + pParameters->iDetune;

	oEnvelope.NoteOn(iVelocity);

	if ( iEffectTimer < 3
		&& iNbFrequencies <= sizeof(pFrequencies)/sizeof(pFrequencies[0]) )
	{
		// Arpeggio !
		pFrequencies[iNbFrequencies++] = MidiNoteToFrequency(n);
	}
	else
	{
		pFrequencies[0] = MidiNoteToFrequency(n);
		iNbFrequencies = 1;
		iArpeggioTimer = 0;
		iEffectTimer = 0;
		iFrequency = 0;
		if ( iLastFrequency == 0 )
			iLastFrequency = pFrequencies[0];
		iPortamentoFrequency = iLastFrequency - pFrequencies[0];
		iSweepFrequency = MidiNoteToFrequency(n + pParameters->Sweep.iOffset) - pFrequencies[0];
		iModulationAmplitude = MidiNoteToFrequency(n + pParameters->Modulation.iAmplitude) - pFrequencies[0];
	}
	iNoteCounter++;
}

void Instrument::NoteOff(int iNote)
{
	iNoteCounter--;
	if ( iNoteCounter == 0 )
		oEnvelope.NoteOff();
}

InstrumentParameters::InstrumentParameters()
:	iVolume(64),
	iPanning(64),
	iDetune(0),
	iDuty(0),
	iPortamentoLength(0),
	iArpeggio(0)
{
	Envelope.iAttack = 2;
	Envelope.iDecay = 16;
	Envelope.iSustain = 64;
	Envelope.iRelease = 5;
	Tremolo.iAmplitude = 0;
	Tremolo.iFrequency = 0;
	Sweep.iOffset = 0;
	Sweep.iLength = 0;
	Modulation.iAmplitude = 0;
	Modulation.iFrequency = 0;
}