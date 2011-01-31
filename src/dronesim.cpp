#include "dronesim.h"

DroneSim::DroneSim(QObject *parent) :
    QObject(parent)
{
}

void DroneSim::start()
{
    m_dronethread = new DroneThread(this);
    m_dronethread->start();
}


DroneThread::DroneThread(DroneSim *parent)
{
    m_navSock = new QUdpSocket(this);
    m_cmdSock = new QUdpSocket(this);
    m_parent = parent;

    // Navigation data goes in port 5554
    m_navSock->bind(QHostAddress::Any, 5554);
    // commands via port 5556
    m_cmdSock->bind(QHostAddress::Any, 5556);

}

void DroneThread::run()
{
    connect(m_navSock, SIGNAL(readyRead()), this, SLOT(dataInNavSock()));

    m_state = state_bootstrap;

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(stateTimerTick()));
    m_timer->start();

    exec();
}

void DroneThread::stateTimerTick()
{
    qDebug("DroneThread::stateTimerTick");
}

void DroneThread::dataInNavSock()
{
    qDebug("DroneThread::dataInNavSock");
    qint64 l;
    char buf[2048];
    QHostAddress host;
    quint16 port;
    while(m_navSock->hasPendingDatagrams()) l=m_navSock->readDatagram(buf,sizeof(buf),&host,&port);
    qDebug() << "DroneThread::navDataReady state=" << m_state <<" l=" << l << "read=" << buf << "from"  << host;

    switch (m_state)
    {
        case state_bootstrap:
            // Expect some package, answer with at least status
            m_state = state_init;
        break;

        case state_init:
            // Expecting AT*CONFIG=\"general:navdata_demo\",\"TRUE\"\r
            // Process command
            // Send status with AR_DRONE_COMMAND_MASK = TRUE

            m_state = state_navdata_demo;
        break;

        case state_navdata_demo:
            // Wait for ACK_CONTROL_MODE

            // When comes, go to state_ready
        break;

        case state_ready:
            // Ready for AT commands
        break;

        case state_flying:
        break;

        default:
        break;
    }


}

void DroneThread::receivedCmd()
{
    qDebug("DroneThread::receivedCmd");
}
