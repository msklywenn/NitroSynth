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

#pragma pack(push, 1)
struct Command
{
	unsigned int iFrame		: 24;
	unsigned int iChannel	: 3;
	unsigned int iCommand	: 5; // 0-14 = parameters, 29 = note on, 30 = note off, 31 = song end
	int iValue;
};
#pragma pack(pop)

#include <stdio.h>
#include <string.h>
#include <windows.h>

const char * GetCommandName(int c)
{
	static const char * pParameterName[InstrumentParameters::LAST] = { "vol", "pan", "dtune", "attck", "decay", "sustn", "rlz", "duty", "porta", "arp", "swlen",  "swofs", "mdamp", "mdfrq", "tmamp", "tmfrq" };
	if ( c < InstrumentParameters::LAST )
		return pParameterName[c];
	if ( c == InstrumentParameters::NOTEOFF )
		return "note off";
	if ( c == InstrumentParameters::NOTEON )
		return "note on";
	if ( c == InstrumentParameters::END )
		return "end";
	return "WTF?";
}

int main(int argc, char ** argv)
{
	if ( argc == 2 )
	{
		FILE * f = fopen(argv[1], "rb");
		if ( f )
		{
			fseek(f, 0, SEEK_END);
			int size = ftell(f);
			fseek(f, 0, SEEK_SET);

			unsigned char * buf = new unsigned char[size];

			fread(buf, 1, size, f);

			char txtpath[MAX_PATH];
			strcpy(txtpath, argv[1]);
			strcat(txtpath, ".txt");
			FILE * txt = fopen(txtpath, "w");

			InstrumentParameters * pParameters = (InstrumentParameters *) buf;
			Command * pCommands = (Command *) (buf+sizeof(InstrumentParameters)*8);

			for ( int i = 0 ; i < 8 ; i++ )
			{
				fprintf(txt, "Instrument %d\n", i);
				fprintf(txt, "    Volume %d\n", pParameters[i].iVolume);
				fprintf(txt, "    Panning %d\n", pParameters[i].iPanning-64);
				fprintf(txt, "    Detune %d\n", pParameters[i].iDetune);
				fprintf(txt, "    Duty %d\n", pParameters[i].iDuty);
				fprintf(txt, "    Attack %d\n", pParameters[i].Envelope.iAttack);
				fprintf(txt, "    Decay %d\n", pParameters[i].Envelope.iDecay);
				fprintf(txt, "    Sustain %d\n", pParameters[i].Envelope.iSustain);
				fprintf(txt, "    Release %d\n", pParameters[i].Envelope.iRelease);
				fprintf(txt, "    Tremolo amp %d\n", pParameters[i].Tremolo.iAmplitude);
				fprintf(txt, "    Tremolo frq %d\n", pParameters[i].Tremolo.iFrequency);
				fprintf(txt, "    Sweep len %d\n", pParameters[i].Sweep.iLength);
				fprintf(txt, "    Sweep ofs %3.2f\n", pParameters[i].Sweep.iOffset / 16.0f);
				fprintf(txt, "    Modulation amp %d\n", pParameters[i].Modulation.iAmplitude);
				fprintf(txt, "    Modulation frq %d\n", pParameters[i].Modulation.iFrequency);
				fprintf(txt, "    Portamento %d\n", pParameters[i].iPortamentoLength);
				fprintf(txt, "    Arpeggio %d\n", pParameters[i].iArpeggio);
			}

			fputs("\n", txt);
			fputs("Commands\n", txt);
			fputs("  [delta t] cha    command value\n\n", txt);
			
			while ( pCommands->iCommand != InstrumentParameters::END )
			{
				fprintf(txt, "  [%7d] %3d %10s ", pCommands->iFrame, pCommands->iChannel, GetCommandName(pCommands->iCommand));
				if ( pCommands->iCommand < InstrumentParameters::LAST )
					fprintf(txt, "%10d\n", pCommands->iValue);
				else if ( pCommands->iCommand == InstrumentParameters::NOTEON ) //(iNote * 16 /*+ pEvent->detune*/) | (iVelocity << 16);
					fprintf(txt, "%3.2f %d\n", (pCommands->iValue & 0xFFFF) / 16.0f, (pCommands->iValue >> 16) & 0x7F);
				else
					fprintf(txt, "\n");
				pCommands++;
			}

			delete[] buf;
			fclose(f);
		}
	}

	return 0;
}