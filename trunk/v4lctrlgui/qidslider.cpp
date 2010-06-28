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


#include "qidslider.h"
#include <iostream>

using namespace std;

QIdSlider::QIdSlider(QWidget *parent, int id):QSlider(parent),id(id){
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
	cout << "init " << id << endl;
}

void QIdSlider::valueChangedSlot(int i){
	cout << "valueChanged" << endl;
}

int QIdSlider::getId(){
	return id;
}

