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

#ifndef IDSIGNALMAPPER_H
#define IDSIGNALMAPPER_H

#include <QtGui>
#include <linux/types.h>

using namespace std;

class Wrapper:public QObject{
	Q_OBJECT
	public:
		virtual void changeValue(unsigned long int);
};


class SliderWrapper:public Wrapper{
	Q_OBJECT
			
	public:
		SliderWrapper(unsigned long int, QSlider*, QLabel*);
		virtual void changeValue(unsigned long int);
		
	public slots:
		void valueChangedSlot(int);
		
	signals:
		void valueChangedId(unsigned long int id, unsigned long int newVal);
		
	private:
		unsigned long int id;
		QLabel *status;
		QSlider *source;
};

class CheckBoxWrapper:public Wrapper{
	Q_OBJECT
			
	public:
		CheckBoxWrapper(unsigned long int, QCheckBox*);
		virtual void changeValue(unsigned long int);
		
	public slots:
		void valueChangedSlot(int);		
		
	signals:
		void valueChangedId(unsigned long int id, unsigned long int newVal);
		
	private:
		unsigned long int id;
		QCheckBox *source;
};


#endif
