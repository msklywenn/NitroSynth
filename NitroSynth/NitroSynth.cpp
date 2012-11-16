#include "NitroSynth.h"
#ifndef NO_VST_GUI
#include "NitroSynthInterface.h"
#endif // NO_VST_GUI

#include <stdio.h>
#include <math.h>

NitroSynth::NitroSynth(audioMasterCallback pCallback)
:	AudioEffectX(pCallback, 0, 128),
	fSamplePeriod(1.0f/44100.0f),
	bRecording(false),
	iRecorderState(IDLE)
{
	if ( pCallback )
	{
		setNumInputs(0);
		setNumOutputs(2);
		canProcessReplacing();
		isSynth();
		setUniqueID('SRTN');
	}

	for ( int i = 0 ; i < 8 ; i++ )
		if ( i < 6 )
			pInstruments[i] = new Instrument(new WaveDuty(), & pParameters[i]);
		else
			pInstruments[i] = new Instrument(new Noise(), & pParameters[i]);

#ifndef NO_VST_GUI
	setEditor(new NitroSynthInterface(this));
#endif // NO_VST_GUI
}

NitroSynth::~NitroSynth()
{
	for ( int i = 0 ; i < 8 ; i++ )
		delete pInstruments[i];
	
#ifndef NO_VST_GUI
	if ( editor )
	{
		delete editor;
		editor = 0;
	}
#endif // NO_VST_GUI
}

void NitroSynth::processReplacing(float** pInput, float** pOutput, VstInt32 iSamples)
{
	float* pLeft = pOutput[0];
	float* pRight = pOutput[1];
	
	VstTimeInfo* pTimeInfo = getTimeInfo(0);

	for ( int i = 0 ; i < iSamples ; i++ )
	{
		while ( oEvents.size() != 0 && oEvents.front().iDeltaFrames == i )
		{
			MidiNote oEvent = oEvents.front();
			int iNote = oEvent.iNote * 16;

			if ( oEvent.bNoteOn )
				pInstruments[oEvent.iChannel]->NoteOn(iNote, oEvent.iVelocity);
			else
				pInstruments[oEvent.iChannel]->NoteOff(iNote);

			if ( bRecording )
			{
				double fTime = (pTimeInfo->samplePos + i) / pTimeInfo->sampleRate;
				int iFrame = (int)(fTime * 1000.0 / 16.0);

				Command oCommand;
				oCommand.iChannel = oEvent.iChannel;
				oCommand.iFrame = iFrame;
				oCommand.iCommand = oEvent.bNoteOn ? InstrumentParameters::NOTEON : InstrumentParameters::NOTEOFF;
				oCommand.iValue = (iNote) | (oEvent.iVelocity << 16);

				oCommands.push_back(oCommand);
			}

			oEvents.pop_front();
		}

		int iLeft = 0;
		int iRight = 0;
		for ( int i = 0 ; i < 8 ; i++ )
			pInstruments[i]->Compute(fSamplePeriod, iLeft, iRight);

		*pLeft++ = iLeft / (float)0x7FFF;
		*pRight++ = iRight / (float)0x7FFF;
	}
}

void NitroSynth::StartRecording()
{
	bRecording = true;
	oCommands.clear();
}

void NitroSynth::StopRecording()
{
	bRecording = false;
}

VstInt32 NitroSynth::processEvents(VstEvents * pEvents)
{
	for ( int i = 0 ; i < pEvents->numEvents ; i++ )
	{
		if ( pEvents->events[i]->type == kVstMidiType )
		{
			VstMidiEvent* pEvent = (VstMidiEvent *) pEvents->events[i];
			char* pMidiData = pEvent->midiData;

			int iChannel = pMidiData[0] & 0x0F;
			
			if ( iChannel < 8 )
			{
				int iCommand = pMidiData[0] & 0xF0;

				if ( iCommand == 0x90 || iCommand == 0x80 )
				{		
					MidiNote oEvent;
					oEvent.iChannel = iChannel;
					oEvent.iNote = (int)(pMidiData[1] & 0x7F);
					oEvent.iVelocity = (int)(pMidiData[2] & 0x7F);
					oEvent.bNoteOn = (iCommand == 0x90);
					oEvent.iDeltaFrames = pEvent->deltaFrames;

					oEvents.push_back(oEvent);
				}
			}
		}
	}

	return 1;
}

void NitroSynth::setSampleRate(float fSampleRate)
{
	fSamplePeriod = 1.0f / fSampleRate;
}

bool NitroSynth::getEffectName(char * pText)
{
	strncpy(pText, "NitroSynth", kVstMaxEffectNameLen);
	return true;
}

bool NitroSynth::getProductString(char * pText)
{
	strncpy(pText, "NitroSynth", kVstMaxProductStrLen);
	return true;
}

bool NitroSynth::getVendorString(char * pText)
{
	strncpy(pText, "MsK`", kVstMaxVendorStrLen);
	return true;
}

VstInt32 NitroSynth::getVendorVersion()
{
	return 1;
}

void NitroSynth::setParameter(VstInt32 index, float value)
{
	int iChannel = index / 16;
	int iParameter = index % 16;
	InstrumentParameters * pParameter = & pParameters[iChannel];

 	int iValue = -1;

	if ( iParameter < InstrumentParameters::LAST )
	{
		switch ( iParameter )
		{
			case InstrumentParameters::VOLUME:
				iValue = pParameter->iVolume = (int) (value * 127.0f);
				break;

			case InstrumentParameters::PANNING:
				iValue = pParameter->iPanning = (int) (value * 128.0f);
				break;

			case InstrumentParameters::DETUNE:
				iValue = pParameter->iDetune = (int) ((value-0.5f) * 2.0f * 32.0f);
				break;

			case InstrumentParameters::ENVELOPE_ATTACK:
				if ( value <= 1.2f/127.0f )
					iValue = pParameter->Envelope.iAttack = 1;
				else
					iValue = pParameter->Envelope.iAttack = (int)(value * 127.0f);
				break;

			case InstrumentParameters::ENVELOPE_DECAY:
				if ( value <= 1.2f/127.0f )
					iValue = pParameter->Envelope.iDecay = 1;
				else
					iValue = pParameter->Envelope.iDecay = (int)(value * 127.0f);
				break;

			case InstrumentParameters::ENVELOPE_SUSTAIN:
				iValue = pParameter->Envelope.iSustain = (int)(value * 127.0f);
				break;

			case InstrumentParameters::ENVELOPE_RELEASE:
				if ( value <= 1.2f/127.0f )
					iValue = pParameter->Envelope.iRelease = 1;
				else
					iValue = pParameter->Envelope.iRelease = (int)(value * 127.0f);
				break;

			case InstrumentParameters::DUTY:
				iValue = pParameter->iDuty = (int)(value * 7.0f);
				break;

			case InstrumentParameters::PORTAMENTO_LENGTH:
				iValue = pParameter->iPortamentoLength = (int)(value * 128.0f);
				break;

			case InstrumentParameters::SWEEP_LENGTH:
				iValue = pParameter->Sweep.iLength = (int)(value * 127.0f);
				break;

			case InstrumentParameters::SWEEP_OFFSET:
				iValue = pParameter->Sweep.iOffset = (int)((value-0.5f) * 2.0f * 32.0f * 16.0f);
				break;

			case InstrumentParameters::MODULATION_AMPLITUDE:
				iValue = pParameter->Modulation.iAmplitude = (int)(value * 127.0f);
				break;

			case InstrumentParameters::MODULATION_FREQUENCY:
				iValue = pParameter->Modulation.iFrequency = (int)(value * 128.0f);
				break;

			case InstrumentParameters::TREMOLO_AMPLITUDE:
				iValue = pParameter->Tremolo.iAmplitude = (int)(value * 255.0f);
				break;

			case InstrumentParameters::TREMOLO_FREQUENCY:
				iValue = pParameter->Tremolo.iFrequency = (int)(value * 128.0f);
				break;

			case InstrumentParameters::ARPEGGIO:
				iValue = pParameter->iArpeggio = (int)(value * 32.0f);
				break;
		}
	}

	if ( bRecording && iValue != -1 )
	{
		VstTimeInfo* pTimeInfo = getTimeInfo(0);
		if ( pTimeInfo )
		{
			double iTime = pTimeInfo->samplePos / pTimeInfo->sampleRate;
			int iFrame = (int)(iTime * 1000.0 / 16.0);

			Command oCommand;
			oCommand.iChannel = iChannel;
			oCommand.iFrame = iFrame;
			oCommand.iCommand = iParameter;
			oCommand.iValue = iValue;

			oCommands.push_back(oCommand);
		}
	}
}

float NitroSynth::getParameter(VstInt32 index)
{
	int iChannel = index / 16;
	int iParameter = index % 16;
	InstrumentParameters* pParameter = & pParameters[iChannel];

	if ( iParameter < InstrumentParameters::LAST )
	{
		switch ( iParameter )
		{
			case InstrumentParameters::VOLUME:
				return pParameter->iVolume / 127.0f;

			case InstrumentParameters::PANNING:
				return pParameter->iPanning / 128.0f;

			case InstrumentParameters::DETUNE:
				return pParameter->iDetune / 32.0f / 2.0f + 0.5f;

			case InstrumentParameters::ENVELOPE_ATTACK:
				return pParameter->Envelope.iAttack / 127.0f;

			case InstrumentParameters::ENVELOPE_DECAY:
				return pParameter->Envelope.iDecay / 127.0f;

			case InstrumentParameters::ENVELOPE_SUSTAIN:
				return pParameter->Envelope.iSustain / 127.0f;

			case InstrumentParameters::ENVELOPE_RELEASE:
				return pParameter->Envelope.iRelease / 127.0f;

			case InstrumentParameters::DUTY:
				return pParameter->iDuty / 7.0f;

			case InstrumentParameters::PORTAMENTO_LENGTH:
				return pParameter->iPortamentoLength / 128.0f;

			case InstrumentParameters::SWEEP_LENGTH:
				return pParameter->Sweep.iLength / 127.0f;

			case InstrumentParameters::SWEEP_OFFSET:
				return pParameter->Sweep.iOffset / 16.0f / 32.0f / 2.0f + 0.5f;

			case InstrumentParameters::MODULATION_AMPLITUDE:
				return pParameter->Modulation.iAmplitude / 127.0f;

			case InstrumentParameters::MODULATION_FREQUENCY:
				return pParameter->Modulation.iFrequency / 128.0f;

			case InstrumentParameters::TREMOLO_AMPLITUDE:
				return pParameter->Tremolo.iAmplitude / 255.0f;

			case InstrumentParameters::TREMOLO_FREQUENCY:
				return pParameter->Tremolo.iFrequency / 128.0f;

			case InstrumentParameters::ARPEGGIO:
				return pParameter->iArpeggio / 32.0f;
		}
	}
	else if ( index == 127 )
	{
		return iRecorderState / 2.0f;
	}

	return 0.0f;
}

void NitroSynth::getParameterDisplay(VstInt32 index, char* text)
{
	int iChannel = index / 16;
	int iParameter = index % 16;
	InstrumentParameters * pParameter = & pParameters[iChannel];
	
	if ( iParameter < InstrumentParameters::LAST )
	{
		switch ( iParameter )
		{
			case InstrumentParameters::VOLUME:
				//sprintf(text, "%d", pParameter->iVolume);
				sprintf(text, "%d%%", (int)(pParameter->iVolume * (100.0/127.0)));
				break;

			case InstrumentParameters::PANNING:
				sprintf(text, "%d", pParameter->iPanning - 64);
				break;

			case InstrumentParameters::DETUNE:
				//sprintf(text, "%d", pParameter->iDetune);
				sprintf(text, "%3.2f", pParameter->iDetune / 16.0f);
				break;

			case InstrumentParameters::ENVELOPE_ATTACK:
				sprintf(text, "%d", pParameter->Envelope.iAttack);
				break;

			case InstrumentParameters::ENVELOPE_DECAY:
				sprintf(text, "%d", pParameter->Envelope.iDecay);
				break;

			case InstrumentParameters::ENVELOPE_SUSTAIN:
				sprintf(text, "%d", pParameter->Envelope.iSustain);
				break;

			case InstrumentParameters::ENVELOPE_RELEASE:
				sprintf(text, "%d", pParameter->Envelope.iRelease);
				break;

			case InstrumentParameters::DUTY:
				//sprintf(text, "%d", pParameter->iDuty);
				sprintf(text, "%d%%", (int)(pParameter->iDuty * (100.0/7.0)));
				break;

			case InstrumentParameters::PORTAMENTO_LENGTH:
				sprintf(text, "%d", pParameter->iPortamentoLength);
				break;

			case InstrumentParameters::SWEEP_LENGTH:
				sprintf(text, "%d", pParameter->Sweep.iLength);
				break;

			case InstrumentParameters::SWEEP_OFFSET:
				sprintf(text, "%3.2f", pParameter->Sweep.iOffset / 16.0f);
				break;

			case InstrumentParameters::MODULATION_AMPLITUDE:
				sprintf(text, "%d", pParameter->Modulation.iAmplitude);
				break;

			case InstrumentParameters::MODULATION_FREQUENCY:
				sprintf(text, "%d", pParameter->Modulation.iFrequency);
				break;

			case InstrumentParameters::TREMOLO_AMPLITUDE:
				sprintf(text, "%d", pParameter->Tremolo.iAmplitude);
				break;

			case InstrumentParameters::TREMOLO_FREQUENCY:
				sprintf(text, "%d", pParameter->Tremolo.iFrequency);
				break;

			case InstrumentParameters::ARPEGGIO:
				sprintf(text, "%d", pParameter->iArpeggio);
				break;
		}
	}
	else
	{
		strncpy(text, " ", kVstMaxParamStrLen);
	}
}

void NitroSynth::getParameterName(VstInt32 index, char* label)
{
	int iChannel = index / 16;
	int iParameter = index % 16;
	
	if ( iParameter < InstrumentParameters::LAST )
	{
		static const char * pParameterName[InstrumentParameters::LAST] =
		{
			"vol", "pan", "dtune", "attck", "decay", "sustn", "rlz", "duty", "porta", "arp", "swlen",  "swofs", "mdamp", "mdfrq", "tmamp", "tmfrq"
		};
		sprintf_s(label, kVstMaxParamStrLen, "%d-%s", iChannel, pParameterName[iParameter]);
	}
	else
	{
		strncpy(label, " ", kVstMaxParamStrLen);
	}
}

bool NitroSynth::SaveTo(const char* pFilename) const
{
	FILE* f = fopen(pFilename, "wb");

	if ( f )
	{
		for ( int i = 0 ; i < 8 ; i++ )
			fwrite(&pParameters[i], sizeof(InstrumentParameters), 1, f);

		std::vector<Command>::const_iterator it = oCommands.begin();
		int iLastFrame = 0;

		while ( it != oCommands.end() )
		{
			Command oCommand = *it;
			oCommand.iFrame -= iLastFrame;
			fwrite(& oCommand, sizeof(Command), 1, f);

			iLastFrame = it->iFrame;
			
			++it;
		}

		Command oCommand;
		oCommand.iChannel = 0;
		oCommand.iCommand = InstrumentParameters::END;
		oCommand.iFrame = 0;
		oCommand.iValue = 0;
		fwrite(& oCommand, sizeof(Command), 1, f);
		
		fclose(f);

		return true;
	}

	return false;
}

bool NitroSynth::ImportInstruments(const char* pFilename)
{
	FILE* f = fopen(pFilename, "rb");

	if ( f ) 
	{
		for ( int i = 0 ; i < 8 ; i++ )
		{
			if ( ! fread(&pParameters[i], sizeof(InstrumentParameters), 1, f) )
			{
				fclose(f);
				return false;
			}
		}

		fclose(f);

		return true;
	}

	return false;
}