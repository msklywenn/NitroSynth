#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Instrument.h"

typedef struct {
	unsigned int iFrame		: 24;
	unsigned int iChannel	: 3;
	unsigned int iCommand	: 5; // see Instrument.h
	int iValue;
} Command;

typedef struct {
	Instrument pInstruments[8];
	InstrumentParameters pParameters[8];
	Command * pCommand;
	int iTimer;
} Player;

void Player_Init(Player * pPlayer);
void Player_Play(Player * pPlayer, unsigned char * pData);
void Player_Stop(Player * pPlayer);
void Player_Execute(Player * pPlayer);

#endif // _PLAYER_H_
