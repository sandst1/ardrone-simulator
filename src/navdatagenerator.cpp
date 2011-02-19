/*
ARDrone Simulator
Copyright (C) 2011  Topi Santakivi <topi.santakivi@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

This file is partly based on the file dronecontrol.cpp,
which is a part of Kate Alhola's MARDrone:
http://mardrone.garage.maemo.org/
https://garage.maemo.org/plugins/ggit/browse.php/?p=mardrone
*/
#include <QMetaType>
#include "ardrone_common.h"
#include "dronemodel.h"
#include "navdatagenerator.h"

#include <arpa/inet.h>

#define NAVDATA_PORT 5554
#define TIMER_INTERVAL 500 // ms


// original hexdump from: http://stahlworks.com/dev/index.php?tool=csc01
static void hexdump(void *pAddressIn, long  lSize)
{
 char szBuf[100];
 long lIndent = 1;
 long lOutLen, lIndex, lIndex2, lOutLen2;
 long lRelPos;
 struct { char *pData; unsigned long lSize; } buf;
 unsigned char *pTmp,ucTmp;
 unsigned char *pAddress = (unsigned char *)pAddressIn;

   buf.pData   = (char *)pAddress;
   buf.lSize   = lSize;

   while (buf.lSize > 0)
   {
      pTmp     = (unsigned char *)buf.pData;
      lOutLen  = (int)buf.lSize;
      if (lOutLen > 16)
          lOutLen = 16;

      // create a 64-character formatted output line:
      sprintf(szBuf, " >                            "
                     "                      "
                     "    %08lX", pTmp-pAddress);
      lOutLen2 = lOutLen;

      for(lIndex = 1+lIndent, lIndex2 = 53-15+lIndent, lRelPos = 0;
          lOutLen2;
          lOutLen2--, lIndex += 2, lIndex2++
         )
      {
         ucTmp = *pTmp++;

         sprintf(szBuf + lIndex, "%02X ", (unsigned short)ucTmp);
         if(!isprint(ucTmp))  ucTmp = '.'; // nonprintable char
         szBuf[lIndex2] = ucTmp;

         if (!(++lRelPos & 3))     // extra blank after 4 bytes
         {  lIndex++; szBuf[lIndex+2] = ' '; }
      }

      if (!(lRelPos & 3)) lIndex--;

      szBuf[lIndex  ]   = '<';
      szBuf[lIndex+1]   = ' ';

      qDebug("%s\n", szBuf);

      buf.pData   += lOutLen;
      buf.lSize   -= lOutLen;
   }
}


NavdataGenerator::NavdataGenerator(QObject *parent) :
    QThread(parent), m_navdataPort(NAVDATA_PORT)
{
    m_navdata.header            = NAVDATA_HEADER;
    m_navdata.ardrone_state     = ARDRONE_NAVDATA_BOOTSTRAP;
    m_navdata.sequence          = 0;
    m_navdata.vision_defined    = 0;

    start();
}

void NavdataGenerator::run()
{
    qDebug("NavdataGenerator::run");
    m_droneModel = new DroneModel();
    m_navdataSock = new QUdpSocket();
    m_navdataSock->bind(QHostAddress::Any, NAVDATA_PORT);
    connect(m_navdataSock, SIGNAL(readyRead()), this, SLOT(dataInNavSocket()));

    qRegisterMetaType<QHostAddress>("QHostAddress");

    m_hostAddr = QHostAddress::Null;

    m_initialized = false;

    exec();
}

void NavdataGenerator::startNavdataStream()
{
    qDebug("NavdataGenerator::startNavdataStream");

    m_navdata.ardrone_state = ARDRONE_NAVDATA_DEMO_MASK;
    initializeNavdataDemo();

    m_timer = new QTimer();
    m_timer->setInterval(TIMER_INTERVAL);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(navdataTimerTick()));
    m_timer->start();
}

void NavdataGenerator::dataInNavSocket()
{
    qDebug("NavdataGenerator::dataInNavSocket");

    qint64 l;
    char buf[2048];
    while( m_navdataSock->hasPendingDatagrams() )
        l=m_navdataSock->readDatagram(buf,sizeof(buf),&m_hostAddr);
    qDebug() << "NavdataGenerator::dataInNavSocket l=" << l << "read=" << buf << "from"  << m_hostAddr;

    // Check if it's time to go through the drone initialization protocol
    if ( !m_initialized && m_navdata.ardrone_state == ARDRONE_NAVDATA_BOOTSTRAP )
    {
        m_initialized = true;
        qDebug(">>> NavdataGenerator::emitting initializeDrone <<<");
        emit this->initializeDrone(m_hostAddr);

        qDebug("NavdataGenerator::answering to Navdata port with status");
        prepareDatagram();
        sendNavdata();
    }
}

void NavdataGenerator::navdataTimerTick()
{
    qDebug("NavdataGenerator::navdataTimerTick");
    prepareDatagram();
    addNavdataDemoToDatagram();
    sendNavdata();
    qDebug("NavdataGenerator::navdataTimerTick OUT");
}

void NavdataGenerator::sendDroneStatusWithCmdMask()
{
    qDebug("NavdataGenerator::sendDroneStatusWithCmdMask");
    m_navdata.ardrone_state |= ARDRONE_COMMAND_MASK;
    m_navdata.sequence++;

    prepareDatagram();
    sendNavdata();
}


void NavdataGenerator::sendNavdata()
{
    qDebug("NavdataGenerator::sendNavdata");
    QByteArray dgram = QString(m_navdataBuf).toLatin1();

    QString tmp(m_navdataBuf);
    qDebug() << "RAWNAVDATA: " << m_navdataBuf;

    qDebug("Datagram size: %d", dgram.size());

    //m_navdataSock->writeDatagram(dgram.data(), dgram.size(), m_hostAddr, NAVDATA_PORT);
    //m_navdataSock->writeDatagram(dgram.data(), dgram.size(), m_hostAddr, NAVDATA_PORT);

    qint64 rsize = m_navdataSock->writeDatagram(m_navdataBuf, sizeof(navdata_t)+sizeof(navdata_demo_t),m_hostAddr, NAVDATA_PORT);

    hexdump(m_navdataBuf, 64);

    qDebug("%d bytes written", rsize);
}


void NavdataGenerator::prepareDatagram()
{    
    qDebug("NavdataGenerator::prepareDatagram");
    //m_navdataBuf.clear();
    memset(&m_navdataBuf, 0, sizeof(char)*2048);

    memcpy(&m_navdataBuf, (char*)(&m_navdata), sizeof(navdata_t));

    m_copyPtr = (char*)(&m_navdataBuf);
    m_copyPtr += sizeof(navdata_t);
}

void NavdataGenerator::addNavdataDemoToDatagram()
{
    qDebug() << "NavdataGenerator::addNavdataDemoToDatagram";

    qDebug("sizeof navdata_t %d, sizeof_navdata_demo %d, char %d", sizeof(navdata_t), sizeof(navdata_demo_t), sizeof(char));
    memcpy(m_copyPtr, (char*)(&m_navdataDemo), sizeof(navdata_demo_t));
}

void NavdataGenerator::initializeNavdataDemo()
{
    m_navdataDemo.tag = NAVDATA_DEMO_TAG;
    m_navdataDemo.size = sizeof(navdata_demo_t);
    m_navdataDemo.ctrl_state = ARDRONE_NAVDATA_DEMO_MASK;
    m_navdataDemo.vbat_flying_percentage = 100;
    m_navdataDemo.theta = 1.24;
    m_navdataDemo.phi = 2.7;
    m_navdataDemo.psi = 99.6;
    m_navdataDemo.altitude = 22.1;
    m_navdataDemo.vx = 0.6;
    m_navdataDemo.vy = 77.3;
    m_navdataDemo.vz = 8.3;
    m_navdataDemo.num_frames = 54;
}
