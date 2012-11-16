#ifndef _NITROSYNTH_H_
#define _NITROSYNTH_H_

#include "audioeffectx.h"

#include "PSG.h"
#include "Instrument.h"

#include <vector>
#include <queue>

#pragma pack(push, 1)
struct Command
{
	unsigned int iFrame		: 24;
	unsigned int iChannel	: 3;
	unsigned int iCommand	: 5; // 0-14 = parameters, 29 = note on, 30 = note off, 31 = song end
	int iValue;
};
#pragma pack(pop)

struct MidiNote
{
	int iChannel;
	int iNote;
	int iVelocity;
	bool bNoteOn;
	int iDeltaFrames;
};

class NitroSynth : public AudioEffectX
{
	float fSamplePeriod;
	enum ParamRecorderState { IDLE, RECORD, SAVE } iRecorderState;
	bool bRecording;
	std::vector<Command> oCommands;
	std::deque<MidiNote> oEvents;

	Instrument* pInstruments[8];
	InstrumentParameters pParameters[8];

	void processReplacing(float ** pInput, float ** pOutput, VstInt32 iSamples);
	VstInt32 processEvents(VstEvents * pEvents);
	void setSampleRate(float fSampleRate);

	bool getProductString(char * pText);
	bool getEffectName(char * pText);
	bool getVendorString(char * pText);
	VstInt32 getVendorVersion();

	void getParameterName(VstInt32 index, char* label);

public:
	NitroSynth(audioMasterCallback pCallback);
	~NitroSynth();

	void StartRecording();
	void StopRecording();
	bool IsRecording() const { return bRecording; }
	bool HasRecord() const { return oCommands.size() > 0; }

	bool SaveTo(const char* pFilename) const;
	bool ImportInstruments(const char* pFilename);

	void setParameter(VstInt32 index, float value);
	float getParameter(VstInt32 index);
	void getParameterDisplay(VstInt32 index, char* text);
};

#endif // _NITROSYNTH_H_