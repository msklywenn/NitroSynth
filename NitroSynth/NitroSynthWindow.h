#ifndef _NITROSYNTHWINDOW_H_
#define _NITROSYNTHWINDOW_H_

#include <Qt/QWidget.h>

#include "Instrument.h"

class QLabel;
class QSlider;
class QGroupBox;
class QTabWidget;
class QPushButton;
class NitroSynth;

class NitroSynthWindow : public QWidget
{
	Q_OBJECT

	struct Tab
	{
		QWidget* pWidget;
		QLabel* pLabels[InstrumentParameters::LAST];
		QLabel* pSliderValues[InstrumentParameters::LAST];
		QSlider* pSliders[InstrumentParameters::LAST];
		QGroupBox* pGroupBoxes[4];
	};

	QTabWidget* pTabWidget;
	Tab pTabs[8];

	QPushButton* pImportButton;
	QPushButton* pRecordButton;
	QPushButton* pSaveButton;

	NitroSynth* ns;
	bool importing;

private slots:
	void importButton();
	void recordButton();
	void saveButton();
	void slider(int);

public:
	NitroSynthWindow(NitroSynth* ns, void* window);
	~NitroSynthWindow();
};

#endif // _NITROSYNTHWINDOW_H_