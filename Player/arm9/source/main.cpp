#include <nds.h>
#include <stdio.h>

extern unsigned char basicloop[];

int main()
{
	powerOn(POWER_ALL);

	videoSetMode(MODE_0_2D);

	consoleDemoInit();

	BG_PALETTE[0] = RGB15(0, 0, 0);

	while ( 1 )
	{
		swiWaitForVBlank();
		scanKeys();

		if ( keysDown() & KEY_X )
		{
			fifoSendValue32(FIFO_USER_01, (u32)(& basicloop[0]));
			BG_PALETTE[0] = RGB15(15, 0, 0);
		}
		else if ( keysHeld() & KEY_B )
		{
			fifoSendValue32(FIFO_USER_01, 0);
			BG_PALETTE[0] = RGB15(0, 0, 0);
		}
	}

	return 0;
}
