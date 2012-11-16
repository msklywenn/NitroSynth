#include "Player.h"
#include "Utils.h"
#include <nds.h>

void Player_Handler(u32 iCommand, void* pUserdata)
{
	Player* pPlayer = (Player *) pUserdata;
	if ( iCommand == 0 )
		Player_Stop(pPlayer);
	else
		Player_Play(pPlayer, (unsigned char *) iCommand);
}

void Player_Init(Player* pPlayer)
{
	int i;

	for	( i = 0 ; i < 8 ; i++ )
	{
		InstrumentParameters_Init(& pPlayer->pParameters[i]);
		Instrument_Init(& pPlayer->pInstruments[i],	i + 8, & pPlayer->pParameters[i]);
	}

	pPlayer->pCommand = 0;
	pPlayer->iTimer = 0;

	fifoSetValue32Handler(FIFO_USER_01, Player_Handler, pPlayer);
}

void Player_Play(Player* pPlayer, unsigned char* pData)
{
	MemCopy(pPlayer->pParameters, pData, sizeof(InstrumentParameters)*8);
	pPlayer->pCommand = (Command *) (pData + sizeof(InstrumentParameters)*8);
	pPlayer->iTimer = 0;
}

void Player_Stop(Player* pPlayer)
{
	pPlayer->pCommand = 0;

	int i;
	for ( i = 0 ; i < 8 ; i++ )
		Instrument_NoteOff(& pPlayer->pInstruments[i]);
}

void Player_Execute(Player* pPlayer)
{
	int i;

	if ( pPlayer->pCommand != 0 )
	{
		int iCommand = pPlayer->pCommand->iCommand;

		while ( iCommand != END && pPlayer->iTimer == pPlayer->pCommand->iFrame )
		{
			Instrument* pInstrument = & pPlayer->pInstruments[pPlayer->pCommand->iChannel];
			InstrumentParameters* pParameters = & pPlayer->pParameters[pPlayer->pCommand->iChannel];
			int iValue = pPlayer->pCommand->iValue;

			switch ( iCommand )
			{
				case VOLUME:
					pParameters->iVolume = iValue;
					break;

				case PANNING:
					pParameters->iPanning = iValue;
					break;

				case DETUNE:
					pParameters->iDetune = iValue;
					break;

				case ENVELOPE_ATTACK:
					pParameters->iAttack = iValue;
					break;

				case ENVELOPE_DECAY:
					pParameters->iDecay = iValue;
					break;

				case ENVELOPE_SUSTAIN:
					pParameters->iSustain = iValue;
					break;

				case ENVELOPE_RELEASE:
					pParameters->iRelease = iValue;
					break;

				case DUTY:
					pParameters->iDuty = iValue;
					break;

				case PORTAMENTO_LENGTH:
					pParameters->iPortamentoLength = iValue;
					break;

				case SWEEP_LENGTH:
					pParameters->iSweepLength = iValue;
					break;

				case SWEEP_OFFSET:
					pParameters->iSweepOffset = iValue;
					break;

				case MODULATION_AMPLITUDE:
					pParameters->iModulationAmplitude = iValue;
					break;

				case MODULATION_FREQUENCY:
					pParameters->iModulationFrequency = iValue;
					break;

				case TREMOLO_AMPLITUDE:
					pParameters->iTremoloAmplitude = iValue;
					break;

				case TREMOLO_FREQUENCY:
					pParameters->iTremoloFrequency = iValue;
					break;

				case ARPEGGIO:
					pParameters->iArpeggio = iValue;
					break;

				case NOTEON:
					Instrument_NoteOn(pInstrument,
							iValue & 0xFFFF,
							iValue >> 16);
					break;

				case NOTEOFF:
					Instrument_NoteOff(pInstrument);
					break;
			}

			pPlayer->pCommand++;
			pPlayer->iTimer = 0;

			iCommand = pPlayer->pCommand->iCommand;
		}

		if ( pPlayer->pCommand->iCommand == END )
		{
			for ( i = 0 ; i < 8 ; i++ )
				Instrument_NoteOff(& pPlayer->pInstruments[i]);
			pPlayer->pCommand = 0;
		}

		pPlayer->iTimer++;
	}

	for ( i = 0 ; i < 8 ; i++ )
		Instrument_Compute(& pPlayer->pInstruments[i]);
}
