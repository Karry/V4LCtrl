/***************************************************************************
 *   Copyright (C) 2009 by Lukáš Karas aka Karry                           *
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

#include "selectdevicedialog.h"
#include "cfunct.h"
#include <iostream>
#include <QtGui>

using namespace std;

SelectDeviceDialog::SelectDeviceDialog(QString device){
	ui = new Ui_Dialog();
	ui->setupUi((QDialog*)this);
	
	ui->lineEdit->setText(device);
	connect(ui->okButton, SIGNAL(clicked()), this, SLOT(accept()));	
}

void SelectDeviceDialog::accept(){
	QString str = ui->lineEdit->text();
	//cout << "emit deviceSelected(" << qStringToChar(str) << ")"<< endl;
	emit deviceSelected(str); 
	emit done(0);
}
