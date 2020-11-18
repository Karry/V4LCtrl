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

#include "controlwrappers.h"

using namespace std;


void Wrapper::changeValue(unsigned long int){}


SliderWrapper::SliderWrapper(unsigned long int id, QSlider *source, QLabel *status):id(id),status(status),source(source){
	connect(source, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
}

void SliderWrapper::valueChangedSlot(int val){
	if (status){
		char dummy[100];
		sprintf((char*)&dummy, "%d (%d/%d)",source->value(), source->minimum(), source->maximum());
		status->setText( dummy );
	}
	
	emit valueChangedId((unsigned long int)id, (unsigned long int)val);
}

void SliderWrapper::changeValue(unsigned long int val){
	source->setValue(val);
}



CheckBoxWrapper::CheckBoxWrapper(unsigned long int id, QCheckBox *source):id(id), source(source){
	connect(source, SIGNAL(stateChanged(int)), this, SLOT(valueChangedSlot(int)));
}

void CheckBoxWrapper::valueChangedSlot(int val){
	val = val? 1 : 0;
	emit valueChangedId((unsigned long int)id, (unsigned long int)val);
}

void CheckBoxWrapper::changeValue(unsigned long int val){
	source->setCheckState( (Qt::CheckState)(val? 2:0) );
}

ComboBoxWrapper::ComboBoxWrapper(unsigned long int id, QComboBox *source):id(id), source(source){
 	connect(source, SIGNAL(currentIndexChanged(int)), this, SLOT(valueChangedSlot(int)));
}

void ComboBoxWrapper::changeValue(unsigned long int newMenuIndex){
	for (int index = 0; index < source->count(); index++) {
		QVariant var = source->itemData(index);
		if (var.isValid()){
			bool ok=false;
			qulonglong menuIndex = var.toULongLong(&ok);
			if (ok && menuIndex==newMenuIndex && index!=source->currentIndex()){
				source->setCurrentIndex(index);
			}
		}
	}
}

void ComboBoxWrapper::valueChangedSlot(int){
	QVariant var = source->currentData(Qt::UserRole);
	if (var.isValid()){
		bool ok=false;
		qulonglong menuIndex = var.toULongLong(&ok);
		if (ok){
			emit valueChangedId(id, menuIndex);
		}
	}
}
