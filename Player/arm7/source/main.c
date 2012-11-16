#include <nds.h>
#include <dswifi7.h>
#include <maxmod7.h>
#include "Player.h"

void VCountHandler()
{
	inputGetAndSend();
}

int main()
{
	irqInit();
	fifoInit();

	readUserSettings();
	initClockIRQ();

	SetYtrigger(80);

	installSystemFIFO();

	irqSet(IRQ_VCOUNT, VCountHandler);
	irqSet(IRQ_VBLANK, 0);
	irqEnable(IRQ_VBLANK|IRQ_VCOUNT|IRQ_NETWORK);

	powerOn(POWER_SOUND);
	REG_SOUNDCNT = SOUND_ENABLE | SOUND_VOL(127);

	static Player oPlayer;
	Player_Init(& oPlayer);

	while ( 1 )
	{
		swiWaitForVBlank();

		Player_Execute(& oPlayer);
	}

	return 0;
}
