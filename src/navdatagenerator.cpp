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

#define NAVDATA_PORT 5554
#define TIMER_INTERVAL 50 // ms

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

    m_timer = new QTimer();
    m_timer->setInterval(TIMER_INTERVAL);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(navdataTimerTick()));

    qRegisterMetaType<QHostAddress>("QHostAddress");

    m_initialized = false;

    exec();
}

void NavdataGenerator::startNavdataStream()
{
    qDebug("NavdataGenerator::startNavdataStream");

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
    QByteArray dgram(m_navdataBuf.data());
    m_navdataSock->writeDatagram(dgram.data(), dgram.size(), m_hostAddr, NAVDATA_PORT);
}


void NavdataGenerator::prepareDatagram()
{
    qDebug("NavdataGenerator::prepareDatagram");
    //m_navdataBuf.clear();

    memcpy(m_navdataBuf.data(), &m_navdata, sizeof(m_navdata));
}
