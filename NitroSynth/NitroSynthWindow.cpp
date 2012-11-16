#include "NitroSynthWindow.h"

#include <Qt/QLabel.h>
#include <Qt/QSlider.h>
#include <Qt/QGroupBox.h>
#include <Qt/QTabWidget.h>
#include <Qt/QPushButton.h>
#include <Qt/QFileDialog.h>
#include <Qt/QMessageBox.h>

#include "NitroSynth.h"

NitroSynthWindow::NitroSynthWindow(NitroSynth* ns, void* window)
:	ns(ns),
	importing(false)
{
	create(WId(window));

	setGeometry(50, 50, 480, 320);
	setWindowTitle("NitroSynth");

	pTabWidget = new QTabWidget(this);
    pTabWidget->setGeometry(QRect(0, 10, 481, 274));

	for ( int i = 0 ; i < 8 ; i++ )
	{
		Tab* pTab = &pTabs[i];
		pTab->pWidget = new QWidget(pTabWidget);

		for ( int j = 0 ; j < 4 ; j++ )
		{
			static const int x[] = { 13, 245, 245, 245 };
			static const int y[] = { 70, 10, 90, 170 };
			static const char* pGroupName[] =
			{
				"Envelope", "Sweep", "Vibrato", "Tremolo"
			};

			pTab->pGroupBoxes[j] = new QGroupBox(QString::fromAscii(pGroupName[j]), pTab->pWidget);
			pTab->pGroupBoxes[j]->setGeometry(QRect(x[j], y[j], 221, j == 0 ? 111 : 71));
			pTab->pGroupBoxes[j]->raise();
		}

		for ( int j = 0 ; j < InstrumentParameters::LAST ; j++ )
		{
			if ( i > 5 && j == InstrumentParameters::DUTY )
			{
				pTab->pLabels[j] = 0;
				pTab->pSliders[j] = 0;
				pTab->pSliderValues[j] = 0;
				continue;
			}

			static const int x[] =
			{
				20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
				252, 252, 252, 252, 252, 252
			};
			static const int y[] = {
				10, 30, 50, 90, 110, 130, 150, 185, 205, 225,
				30, 50, 110, 130, 190, 210
			};
			static const char* pLabelName[] =
			{
				"Volume", "Panning", "Detune", "Attack", "Decay", "Sustain", "Release", "Pulse Width", "Portamento", "Arpeggio",
				"Length", "Offset",	"Amplitude", "Frequency", "Amplitude", "Frequency"
			};

			int index = i * 16 + j;
			float value = ns->getParameter(index);

			pTab->pLabels[j] = new QLabel(QString::fromAscii(pLabelName[j]), pTab->pWidget);
			pTab->pLabels[j]->setGeometry(QRect(x[j], y[j], 56, 20));
			pTab->pLabels[j]->raise();

			pTab->pSliders[j] = new QSlider(pTab->pWidget);
			pTab->pSliders[j]->setGeometry(QRect(x[j] + 60, y[j], 111, 19));
			pTab->pSliders[j]->setOrientation(Qt::Horizontal);
			pTab->pSliders[j]->setRange(0, 127);
			pTab->pSliders[j]->setValue(value * 127);
			pTab->pSliders[j]->raise();
			connect(pTab->pSliders[j], SIGNAL(valueChanged(int)), this, SLOT(slider(int)));

			char txt[32];
			ns->getParameterDisplay(index, txt);
			pTab->pSliderValues[j] = new QLabel(txt, pTab->pWidget);
			pTab->pSliderValues[j]->setGeometry(QRect(x[j] + 177, y[j], 35, 20));
			pTab->pSliderValues[j]->raise();
		}

		static const char* pTabName[] =
		{
			"1 Pulse", "2 Pulse", "3 Pulse", "4 Pulse", "5 Pulse", "6 Pulse", "7 Noise", "8 Noise"
		};
		pTabWidget->addTab(pTab->pWidget, QString::fromAscii(pTabName[i]));
	}

    pImportButton = new QPushButton(QString::fromUtf8("Import"), this);
    pImportButton->setGeometry(QRect(210, 290, 75, 23));
    pRecordButton = new QPushButton(QString::fromUtf8("Record"), this);
    pRecordButton->setGeometry(QRect(300, 290, 75, 23));
    pSaveButton = new QPushButton(QString::fromUtf8("Save"), this);
    pSaveButton->setGeometry(QRect(390, 290, 75, 23));
	pSaveButton->setEnabled(false);

	connect(pRecordButton, SIGNAL(clicked()), this, SLOT(recordButton()));
	connect(pSaveButton, SIGNAL(clicked()), this, SLOT(saveButton()));
	connect(pImportButton, SIGNAL(clicked()), this, SLOT(importButton()));
}

NitroSynthWindow::~NitroSynthWindow()
{
	for ( int j = 0 ; j < 8 ; j++ )
	{
		Tab* pTab = &pTabs[j];

		for ( int i = 0 ; i < 14 ; i++ )
		{
			if ( pTab->pLabels[i] ) delete pTab->pLabels[i];
			if ( pTab->pSliders[i] ) delete pTab->pSliders[i];
			if ( pTab->pSliderValues[i] ) delete pTab->pSliderValues[i];
		}

		for ( int i = 0 ; i < 4 ; i++ )
		{
			delete pTab->pGroupBoxes[i];
		}

		delete pTab->pWidget;
	}

	delete pTabWidget;

	delete pRecordButton;
	delete pSaveButton;
}

void NitroSynthWindow::recordButton()
{
	if ( ns->IsRecording() )
	{
		if ( ns->HasRecord() )
			pSaveButton->setEnabled(true);
		ns->StopRecording();
		pRecordButton->setText("Record");
	}
	else
	{
		ns->StartRecording();
		pRecordButton->setText("Stop");
	}
}

void NitroSynthWindow::importButton()
{
	QString str = QFileDialog::getOpenFileName(this, "Import instruments from");

	if ( ns->ImportInstruments(str.toAscii()) )
	{
		importing = true;
		// Update sliders in all instruments
		for ( int i = 0 ; i < 8 ; i++ )
		{
			Tab* pTab = &pTabs[i];

			for ( int j = 0 ; j < InstrumentParameters::LAST ; j++ )
			{
				if ( !(i > 5 && j == InstrumentParameters::DUTY) )
				{
					int index = i * 16 + j;
					float value = ns->getParameter(index);
					pTab->pSliders[j]->setValue(value * 127);

					char txt[32];
					ns->getParameterDisplay(index, txt);
					pTab->pSliderValues[j]->setText(txt);
				}
			}
		}
		importing = false;
	}
}

void NitroSynthWindow::saveButton()
{
	QString str = QFileDialog::getSaveFileName(this, "Save to");

	if ( ! ns->SaveTo(str.toAscii()) )
		QMessageBox::warning(this, "Warning", QString("Could not save to \"") + str + QString("\"."));
}

void NitroSynthWindow::slider(int value)
{
	if ( importing )
		return;

	QSlider* slider = (QSlider*) sender();
	QWidget* widget = (QWidget*) slider->parent();

	int channel = -1;
	for ( int i = 0 ; i < 8 ; i++ )
	{
		if ( pTabs[i].pWidget == widget )
		{
			channel = i;
			break;
		}
	}

	if ( channel == -1 )
	{
		QMessageBox::warning(this, "WTF?!", "Unknown channel");
		return;
	}

	int param = -1;
	for ( int i = 0 ; i < InstrumentParameters::LAST ; i++ )
	{
		if ( pTabs[channel].pSliders[i] == slider )
		{
			param = i;
			break;
		}
	}

	if ( param == -1 )
	{
		QMessageBox::warning(this, "WTF?!", "Unknown parameter");
		return;
	}

	int index = param + channel * 16;
	ns->setParameter(index, value / 127.0f);

	char txt[32];
	ns->getParameterDisplay(index, txt);
	pTabs[channel].pSliderValues[param]->setText(txt);
}