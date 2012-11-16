#ifndef _MIDI_H_
#define _MIDI_H_

unsigned short MidiNoteToFrequency(int note);
int Abs(int x);
int Sinus(int x); // fixed point .12
void MemCopy(void * pDestination, void * pSource, int iSize);

#endif // _MIDI_H_
