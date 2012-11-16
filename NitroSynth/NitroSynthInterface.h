#ifndef _NITROSYNTHINTERFACE_H_
#define _NITROSYNTHINTERFACE_H_

#include "aeffeditor.h"

class QApplication;
class NitroSynthWindow;

class NitroSynthInterface : public AEffEditor
{
	QApplication* pApplication;
	NitroSynthWindow* pWindow;

	bool open(void* ptr);
	void close();
	void idle();

public:
	NitroSynthInterface(AudioEffectX* pEffect);
	~NitroSynthInterface();
};

#endif // _NITROSYNTHINTERFACE_H_