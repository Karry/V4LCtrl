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

/* These headers are not needed by us, but by linux/videodev2.h,
   which is broken on some systems and doesn't include them itself :( */
#include <sys/time.h>
#include <linux/types.h>
#include <linux/ioctl.h>
/* end broken header workaround includes */
#include <linux/videodev.h>
#include <linux/videodev2.h>

