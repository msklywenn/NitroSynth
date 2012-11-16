#ifndef _PSG_H_
#define _PSG_H_

class PSG {
	float fTime;
	int iFrequency;
	signed short iLastSample;

protected:
	int x;
	int iDuty;

	virtual signed short Generate() = 0;

public:
	PSG();

	void SetDuty(int d) { iDuty = d; }
	void SetFrequency(int f) { iFrequency = f * 8; }

	virtual void Reset();

	signed short Compute(float dt);
};

class Noise : public PSG {
	signed short Generate();
	void Reset();
public:
	Noise() : PSG() { Reset(); }
};

class WaveDuty : public PSG {
	signed short Generate();
	void Reset();
public:
	WaveDuty() : PSG() { Reset(); }
};

#endif // _PSG_H_