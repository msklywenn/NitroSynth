#include "NitroSynthInterface.h"

#include <Qt/QApplication.h>
#include <Qt/QFile.h>
#include "NitroSynthWindow.h"

static const char* stylesheet =
"QWidget {\
	background-color: #d9ecff;\
}\
\
QLabel, QSlider {\
	background: none;\
}\
\
QPushButton {\
	border: 1px solid #626a73;\
	border-radius: 7px;\
	background-color: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0, stop: 0 #98a5b2, stop: 1 #b8c8d9);\
}\
\
QPushButton:pressed {\
	background-color: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0, stop: 0 #b8c8d9, stop: 1 #98a5b2);\
}\
\
QPushButton:hover {\
	border-color: #98a5b2;\
}\
\
QSlider::groove:horizontal {\
	border: 1px solid #626a73;\
	height: 8px;\
	background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #cee0f2, stop:1 #d9ecff);\
	margin: 2px 0;\
	border-radius: 5px;\
}\
\
QSlider::handle:horizontal {\
	border: 1px solid #626a73;\
	background: qlineargradient(x1:0, y1:1, x2:0, y2:0, stop:0 #d9ecff, stop:1 #98a5b2);\
	width: 15px;\
	margin: -0.72px 0;\
	border-radius: 6px;\
}\
\
QGroupBox {\
	background-color: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0, stop: 0 #cee0f2, stop: 1 #d9ecff);\
	border: 1px solid #626a73;\
	border-radius: 7px;\
	margin-top: 2ex;\
}\
\
QGroupBox::title {\
	subcontrol-origin: margin;\
	subcontrol-position: top center;\
	padding: 0 5px;\
}\
\
QTabWidget::pane {\
    border-top: 1px solid #626a73;\
    position: absolute;\
    top: -0.78em;\
	padding-top: 0.78em;\
}\
\
QTabWidget::tab-bar {\
    alignment: center;\
}\
\
QTabBar::tab {\
    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #b8c8d9, stop: 1.0 #98a5b2);\
    border: 1px solid #626a73;\
    border-radius: 5px;\
    min-width: 8ex;\
    padding: 4px;\
}\
\
QTabBar::tab:selected, QTabBar::tab:hover {\
    background: #d9ecff;\
}\
\
QTabBar::tab:selected {\
    border-color: #98a5b2;\
}";

NitroSynthInterface::NitroSynthInterface(AudioEffectX* pEffect)
:	AEffEditor(pEffect),
	pApplication(0),
	pWindow(0)
{
	int argc = 0;
	pApplication = new QApplication(argc, 0, true);
	pApplication->setStyleSheet(stylesheet);
}

NitroSynthInterface::~NitroSynthInterface()
{
	delete pApplication;
}

bool NitroSynthInterface::open(void* window)
{
	pWindow = new NitroSynthWindow((NitroSynth*) effect, window);
	pWindow->show();
	systemWindow = window;
	return true;
}

void NitroSynthInterface::close()
{
	pWindow->hide();
	delete pWindow;
	systemWindow = 0;
}

void NitroSynthInterface::idle()
{
	pApplication->processEvents();
}