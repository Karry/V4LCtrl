/***************************************************************************
 *   Copyright (C) 2009 by Lukas Karas aka Karry                           *
 *   lukas.karas@centrum.cz                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef V4LCTRLGUI_H
#define V4LCTRLGUI_H

#include "../v4lctrl/libv4lctrl.h"
#include "controlwrappers.h"

#include "ui_mainwindow.h"

#include <map>
#include <QMainWindow>
#include <QCloseEvent>

using namespace std;

class v4lCtrlGUI:public QMainWindow
{
	Q_OBJECT

public:
	v4lCtrlGUI();
	~v4lCtrlGUI();

protected:
	QString device;
	
	void closeEvent(QCloseEvent *event);
	void closeActions();

	
private 
	slots:
		void controlChangedSlot(unsigned long int, unsigned long int);
		void open();
		void about();
		void refresh();
		void readSettings();
		void writeSettings();
		int closeDevice();
		void changeDevice(QString);
		void startTimer(bool);

private:
	struct v4ldevice *vd;
				
	Ui_MainWindow *ui;
	QAction *openAct;
	QAction *exitAct;
	QAction *aboutAct;
	QAction *aboutQtAct;
	QTimer *timer;
			
	int openDevice();
	map<unsigned long int, Wrapper*> controlWrappers;
	
};

#endif
