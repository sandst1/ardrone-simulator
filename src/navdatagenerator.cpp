#include "navdatagenerator.h"
#include "dronemodel.h"

NavdataGenerator::NavdataGenerator(QObject *parent) :
    QThread(parent), m_navdataPort(5554)
{
}

void NavdataGenerator::run()
{
    m_droneModel = new DroneModel(this);
}

void NavdataGenerator::setHostIP(const QHostAddress &ipaddr)
{
    m_hostAddr = ipaddr;
}
