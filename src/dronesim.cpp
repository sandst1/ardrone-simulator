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
#include "atcommandparser.h"
#include "dronesim.h"
#include "navdatagenerator.h"

#define NAVDATA_TIMER_INTERVAL 30 // ms
#define COMMAND_PORT 5556

DroneSim::DroneSim(QObject *parent) :
    QObject(parent)
{    
}

void DroneSim::start()
{
    m_NetworkThread = new NetworkThread(this);
    m_NetworkThread->start();
}


NetworkThread::NetworkThread(DroneSim *parent) :
    m_cmdPort(COMMAND_PORT)
{
    m_cmdSock = new QUdpSocket(this);
    m_parent = parent;

    // Commands via port 5556
    m_cmdSock->bind(QHostAddress::Any, m_cmdPort);

}

void NetworkThread::run()
{
    connect(m_cmdSock, SIGNAL(readyRead()), this, SLOT(dataInCmdSock()));

    m_state = state_bootstrap;

    // Create the AT Command Parser
    m_ATCmdParser = new ATCommandParser();

    // Create the navdata generator
    m_navdataGen = new NavdataGenerator();

    connect(m_navdataGen, SIGNAL(initializeDrone(const QHostAddress&)), this, SLOT(initializeDrone(const QHostAddress&)));

    // Start the event loop
    exec();
}

void NetworkThread::initializeDrone(QHostAddress hostAddr)
{
    qDebug("NetworkThread::initializeDrone");

    m_hostAddr = hostAddr;

    // Set state from state_bootstrap to state_init so
    // we start to wait for the navdata_demo command
    m_state = state_init;

}

void NetworkThread::dataInCmdSock()
{
    qDebug("NetworkThread::dataInCmdSock");
    qint64 l;
    quint16 port;
    while(m_cmdSock->hasPendingDatagrams()) l=m_cmdSock->readDatagram(m_cmdSockBuf,sizeof(m_cmdSockBuf),&m_hostAddr,&port);

    qDebug() << "NetworkThread::dataInCmdSock state=" << m_state <<" l=" << l << "read=" << m_cmdSockBuf << "from"  << m_hostAddr;

    updateState();
}

void NetworkThread::updateState()
{
    qDebug("NetworkThread::updateState, state %d", m_state);
    switch (m_state)
    {
        //case state_bootstrap:
            // Data has arrived to the navdata socket,
            // Respond with at least status
            //m_state = state_init;
        //break;

        case state_init:
            // Expecting AT*CONFIG=\"general:navdata_demo\",\"TRUE\"\r
            // in m_cmdSockBuf

            qDebug("state_init, checking m_cmdSockBuf");

            // For now, just do a simplified string compare
            // TODO: USE ATCmdParser for checking the value of m_cmdSockBuf!
            if ( QString(m_cmdSockBuf).contains("general:navdata_demo") )
            {
                qDebug("AT*CONFIG=\"general:navdata_demo\" found");
                // Tell the Navdata Generator to reply with the ARDRONE_COMMAND_MASK on
                m_navdataGen->sendDroneStatusWithCmdMask();
            }

            m_state = state_navdata_demo;
        break;

        case state_navdata_demo:
            // Wait for ACK_CONTROL_MODE in m_cmdSockBuf
            // When it comes, go to state_ready

            // Time to start the navdata generator

            // if ( ACK_CONTROL_MODE ) in m_cmdSockBuf
            //     m_state = state_ready;
        break;

        case state_ready:
            // Listening for AT commands in m_CmdSockBuf

            // call m_ATCmdPArser for finding out which command we received
        break;

        case state_flying:
        break;

        default:
        break;
    }
}

