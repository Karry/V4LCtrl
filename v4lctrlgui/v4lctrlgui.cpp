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

#include "linux/videodev2.h"
#include "libv4l1-videodev.h"
#include "libv4l2.h"

#include "v4lctrlgui.h"
#include "cfunct.h"
#include "controlwrappers.h"
#include "selectdevicedialog.h"
#include "errordialog.h"

#include <QtGui>
#include <QMessageBox>

#include <QTextEdit>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>

#include <iostream>
#include <cstring>
#include <string>

using namespace std;

v4lCtrlGUI::v4lCtrlGUI(){
	ui = new Ui_MainWindow();
	ui->setupUi(this);
	
	connect(ui->actionAboutApp, SIGNAL(triggered()), this, SLOT(about()));
	connect(ui->actionAboutQT, SIGNAL(triggered()), qApp, SLOT(aboutQt()));	
	connect(ui->actionRefresh, SIGNAL(triggered()), this, SLOT(refresh()));	
	connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));	
	connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(closeDevice()));	
	connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));	
	

  readSettings();
	
	vd = (v4ldevice*)malloc(sizeof(struct v4ldevice));
	openDevice();
	
	timer = new QTimer( this );
	connect( timer, SIGNAL(timeout()), this, SLOT(refresh()) );
	timer->start( 1000 ); // 1 second single-shot timer
	connect( ui->actionRefreshAutomaticaly, SIGNAL(triggered(bool)), this, SLOT(startTimer(bool)) );
}

void v4lCtrlGUI::startTimer(bool b){
	if (b)
		timer->start(1000);
	else
		timer->stop();
}
void v4lCtrlGUI::controlChangedSlot(unsigned long int id, unsigned long int val){
	//cout << "controlChangedSlot: " << id << ", " << val << endl;
	setCotrol(vd, id, val);
}

void v4lCtrlGUI::refresh(){
	//cout << "refresh" << endl;
	map<unsigned long int, Wrapper*>::iterator iter = controlWrappers.begin();
	map<unsigned long int, Wrapper*>::iterator endIter = controlWrappers.end();
	for(;iter != endIter; iter++){
		pair<unsigned long int, Wrapper*> pair = *iter;

		unsigned long int value;
		int result;
		if ((result = getCotrol(vd, pair.first, &value)) < 0){
			cout << "get ctrl id: " << pair.first << " returned " << result << endl;
		}else{
			(pair.second)->changeValue(value);
		}
	}
}


int v4lCtrlGUI::closeDevice(){
	ui->tabWidget->setEnabled(false);
				
	ui->device_label->setText( QString("") );
	ui->driver_label->setText( QString("") );
	ui->card_label->setText( QString("") );
	ui->bus_info_label->setText( QString("") );
	ui->capabilities_label->setText( QString("") );
	
	// delete supported formats
	for (int ii=0; ii < ui->treeWidget->topLevelItemCount(); ii++)
		delete ui->treeWidget->topLevelItem(ii);
		
	// remove all components from controlsGridLayout
	QLayout *layout = ui->controlsGridLayout;
	if (layout) 
		while (layout->count() > 0 )
			layout->itemAt(0)->widget()->setParent(NULL);
	

	// remove all wrappers
	map<unsigned long int, Wrapper*>::iterator iter = controlWrappers.begin();
	map<unsigned long int, Wrapper*>::iterator endIter = controlWrappers.end();
	
	for(;iter != endIter; iter++){
		pair<unsigned long int, Wrapper*> pair = *iter;
		delete (pair.second);
		controlWrappers.erase(iter);
	}
		
	cout << "close previous device ("<< vd->fd << ")" << endl;
	int result = v4ldevice_close(vd);
	cout << "result ("<< result << ")" << endl;
	return result;
}

int v4lCtrlGUI::openDevice(){
	struct v4l2_capability  capability;
	struct v4l2_fmtdesc     fmtdesc;
	struct v4l2_queryctrl   qctrl;
	unsigned int i;
	bool v4l2_device=false;
	char dummy[256];
	
	// close previous device
	closeDevice();
	
	// open device
	char *tmp;
	tmp = qStringToChar(device);
	cout << "try open " << tmp << " device" << endl;
	int result = v4ldevice_open(vd, tmp, 0);
	delete tmp;
	
	if (result < 0){
		qWarning() << "Unable to open device";
		ErrorDialog *dialog = new ErrorDialog(QString("Unable to open device"));
		dialog->show();
		return result;
	}
	
	// test device if it is v4l2 device
	if (-1 != doioctl(vd,VIDIOC_QUERYCAP,dummy,sizeof(dummy))) {
		v4l2_device = true;
	}

	if (-1 != doioctl(vd,VIDIOCGCAP,dummy,sizeof(dummy))) {
		qWarning() << "Only v4l2 devices are supported!";
	}

	if (!v4l2_device) {
		tmp = qStringToChar(device);
		sprintf((char*)&dummy, "%s: not an video4linux2 device\n", tmp);
		cerr << (char*)&dummy << endl;
		ErrorDialog *dialog = new ErrorDialog(QString((char*)&dummy));
		delete tmp;
		closeDevice();
		dialog->show();
		return -1;
	}
		
	// add general info
	printf("general info\n");
	memset(&capability,0,sizeof(capability));
	if (-1 == doioctl(vd,VIDIOC_QUERYCAP,&capability, sizeof(capability)))
		return -1;
	
	ui->tabWidget->setEnabled(true);
	ui->device_label->setText( device );
	
	memcpy(&dummy, &(capability.driver),sizeof(capability.driver));
	dummy[16] = 0;
	ui->driver_label->setText( (char *)&dummy );
	
	memcpy(&dummy, &(capability.card),sizeof(capability.card));
	dummy[32] = 0;
	ui->card_label->setText( (char *)&dummy );
	
	memcpy(&dummy, &(capability.bus_info),sizeof(capability.bus_info));
	dummy[32] = 0;
	ui->bus_info_label->setText( (char *)&dummy );
	
	sprintf((char*)&dummy, "0x%x", capability.capabilities);
	ui->capabilities_label->setText( (char *)&dummy );

	// add supported formats list
	if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
		//printf("video capture\n");
		for (i = 0;; i++) {
			memset(&fmtdesc,0,sizeof(fmtdesc));
			fmtdesc.index = i;
			fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == doioctl(vd,VIDIOC_ENUM_FMT,&fmtdesc,sizeof(fmtdesc)))
				break;
			
			QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);						
			sprintf((char*)&dummy, "VIDEO_CAPTURE:%d", i);
			ui->treeWidget->topLevelItem(i)->setText(0, (char *)&dummy );
			
			new QTreeWidgetItem(treeItem);
			ui->treeWidget->topLevelItem(i)->child(0)->setText(0, "index:");
			sprintf((char*)&dummy, "%d", i);
			ui->treeWidget->topLevelItem(i)->child(0)->setText(1, (char *)&dummy);
			
			new QTreeWidgetItem(treeItem);
			ui->treeWidget->topLevelItem(i)->child(1)->setText(0, "description:");
			memcpy(&dummy, &(fmtdesc.description),sizeof(fmtdesc.description));
			dummy[sizeof(fmtdesc.description)] = 0;			
			ui->treeWidget->topLevelItem(i)->child(1)->setText(1, (char *)&dummy);
			
			new QTreeWidgetItem(treeItem);
			ui->treeWidget->topLevelItem(i)->child(2)->setText(0, "pixelformat:");
			sprintf((char*)&dummy, "0x%x [%c%c%c%c]", fmtdesc.pixelformat,
							(fmtdesc.pixelformat & 0x000000ff) >> 0, 
							(fmtdesc.pixelformat & 0x0000ff00) >> 8, 
							(fmtdesc.pixelformat & 0x00ff0000) >> 16, 
							(fmtdesc.pixelformat & 0xff000000) >> 24 
						 );
			ui->treeWidget->topLevelItem(i)->child(2)->setText(1, (char *)&dummy);
			
		}
	}
	
	// add controls
	memset(&qctrl,0,sizeof(qctrl));
	
	/* find first CID */
	i = V4L2_CID_BASE;
	for (qctrl.id = V4L2_CID_BASE; qctrl.id < V4L2_CID_LASTP1;qctrl.id++){
		if (!doioctl(vd,VIDIOC_QUERYCTRL,&qctrl,sizeof(qctrl))){
			i = qctrl.id;
			break;
		}
	}
	qctrl.id = i;
	
	for (i = 0; i < V4L2_CTRL_ID_MASK; i++) {
		if (i>0){
			printf("\n    VIDIOC_QUERYCTRL (%d | V4L2_CTRL_FLAG_NEXT_CTRL)\n",qctrl.id);
			qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
		}else
			printf("\n    VIDIOC_QUERYCTRL (%d)\n",qctrl.id);
				
		if (doioctl(vd,VIDIOC_QUERYCTRL,&qctrl,sizeof(qctrl)) < 0){
			printf("        break\n");
			break;
		}
		if (qctrl.flags & V4L2_CTRL_FLAG_DISABLED)
			continue;
	
		QLabel *description = new QLabel(ui->scrollAreaWidgetContents);
		description->setObjectName("control_description");
		ui->controlsGridLayout->addWidget(description, i, 0, 1, 1);
	
		memcpy(&dummy, &(qctrl.name),sizeof(qctrl.name));
		dummy[sizeof(qctrl.name)] = 0;
		description->setText( dummy );
		cout << (char*)&dummy << endl;
		
		if (qctrl.type == V4L2_CTRL_TYPE_BOOLEAN){
			QCheckBox *checkBox = new QCheckBox(ui->scrollAreaWidgetContents);
			checkBox->setObjectName( "checkBox" );
			ui->controlsGridLayout->addWidget(checkBox, i, 1, 1, 1);
			
			CheckBoxWrapper *wrapper = new CheckBoxWrapper( qctrl.id, checkBox);
			connect(wrapper, SIGNAL(valueChangedId(unsigned long int,unsigned long int)), this, SLOT(controlChangedSlot(unsigned long int, unsigned long int)));
			
			controlWrappers.insert( pair<unsigned long int, Wrapper*>(qctrl.id,wrapper) );

		}else if (qctrl.type == V4L2_CTRL_TYPE_INTEGER){
			QLabel *curVal = new QLabel(ui->scrollAreaWidgetContents);
			curVal->setObjectName("control_curVal");
			ui->controlsGridLayout->addWidget(curVal, i, 2, 1, 1);
			
			sprintf((char*)&dummy, "%d (%d/%d)",qctrl.default_value , qctrl.minimum, qctrl.maximum);
			curVal->setText( dummy );
			
			QSlider *horizontalSlider = new QSlider(ui->scrollAreaWidgetContents);
			horizontalSlider->setObjectName("horizontalSlider");
			horizontalSlider->setMinimum( qctrl.minimum );
			horizontalSlider->setMaximum( qctrl.maximum );
			horizontalSlider->setValue( qctrl.default_value );
			horizontalSlider->setSingleStep( qctrl.step );
			horizontalSlider->setOrientation(Qt::Horizontal);
			ui->controlsGridLayout->addWidget(horizontalSlider, i, 1, 1, 1);			
			
			SliderWrapper *wrapper = new SliderWrapper( qctrl.id, horizontalSlider, curVal);
			connect(wrapper, SIGNAL(valueChangedId(unsigned long int,unsigned long int)), this, SLOT(controlChangedSlot(unsigned long int, unsigned long int)));

			controlWrappers.insert( pair<unsigned long int, Wrapper*>(qctrl.id,wrapper) );
			
		}else{
			cout << "Unsupported type of control. Try v4lctrlcli." << endl;
		}
	}
	refresh();
	

	//ui->scrollAreaWidgetContents
	return 0;
}


void v4lCtrlGUI::closeEvent(QCloseEvent *event){
	closeActions();
	
	event->accept();
	//event->ignore();
}

void v4lCtrlGUI::closeActions(){
	writeSettings();
}

void v4lCtrlGUI::open(){
	SelectDeviceDialog *dialog = new SelectDeviceDialog(device);
	dialog->setModal(true);
	dialog->setVisible(true);
	
	connect(dialog, SIGNAL(deviceSelected(QString)), this, SLOT(changeDevice(QString)) );
}

void v4lCtrlGUI::changeDevice(QString dev){
	device = dev;
	openDevice();
}

void v4lCtrlGUI::about(){
		QMessageBox::about(this, tr("About v4lCtrlGUI"),
					tr("FIXME :)"));
}

void v4lCtrlGUI::readSettings(){
	QSettings settings("Karry", "V4L Control GUI");
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(400, 400)).toSize();
	device = settings.value("device","/dev/video0").toString();
	
	resize(size);
	move(pos);
}

void v4lCtrlGUI::writeSettings(){
	QSettings settings("Karry", "V4L Control GUI");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
}

v4lCtrlGUI::~v4lCtrlGUI(){
	delete ui;
	closeDevice();
	//v4ldevice_close(vd);
}

