#include "NitroSynth.h"

AudioEffect* createEffectInstance(audioMasterCallback pCallback)
{
	return new NitroSynth(pCallback);
}