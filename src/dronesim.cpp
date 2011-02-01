#include "atcommandparser.h"
#include "dronesim.h"
#include "navdatagenerator.h"

#define NAVDATA_TIMER_INTERVAL 30 // ms

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
    m_navdataPort(5554), m_cmdPort(5556)
{
    m_navSock = new QUdpSocket(this);
    m_cmdSock = new QUdpSocket(this);
    m_parent = parent;

    // Navigation data goes in port 5554
    m_navSock->bind(QHostAddress::Any, m_navdataPort);
    // commands via port 5556
    m_cmdSock->bind(QHostAddress::Any, m_cmdPort);

}

void NetworkThread::run()
{
    connect(m_navSock, SIGNAL(readyRead()), this, SLOT(dataInNavSock()));
    connect(m_cmdSock, SIGNAL(readyRead()), this, SLOT(dataInCmdSock()));

    m_state = state_bootstrap;

    // Create the AT Command Parser
    m_ATCmdParser = new ATCommandParser(this);

    // Create the navdata generator
    m_navdataGen = new NavdataGenerator(this);

    // Start the event loop
    exec();
}

void NetworkThread::dataInNavSock()
{
    qDebug("NetworkThread::dataInNavSock");
    qint64 l;
    quint16 port;
    while(m_navSock->hasPendingDatagrams()) l=m_navSock->readDatagram(m_navSockBuf,sizeof(m_navSockBuf),&m_hostAddr,&port);
    qDebug() << "NetworkThread::dataInNavSock state=" << m_state <<" l=" << l << "read=" << m_navSockBuf << "from"  << m_hostAddr;

    // Tell the navdata generator to what address it needs to send the data
    m_navdataGen->setHostIP(m_hostAddr);

    updateState();
}

void NetworkThread::dataInCmdSock()
{
    qDebug("NetworkThread::dataInCmdSock");
    qint64 l;
    quint16 port;
    while(m_navSock->hasPendingDatagrams()) l=m_navSock->readDatagram(m_cmdSockBuf,sizeof(m_cmdSockBuf),&m_hostAddr,&port);

    qDebug() << "NetworkThread::dataInCmdSock state=" << m_state <<" l=" << l << "read=" << m_navSockBuf << "from"  << m_hostAddr;

    updateState();
}

void NetworkThread::updateState()
{
    switch (m_state)
    {
        case state_bootstrap:
            // Expect some package in m_navSockBuf,
            // answer with at least status
            m_state = state_init;
        break;

        case state_init:
            // Expecting AT*CONFIG=\"general:navdata_demo\",\"TRUE\"\r
            // in m_cmdSockBuf

            // Process command
            // Send status with AR_DRONE_COMMAND_MASK = TRUE

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

