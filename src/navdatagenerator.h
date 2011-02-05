#ifndef NAVDATAGENERATOR_H
#define NAVDATAGENERATOR_H

#include <QThread>
#include <QTimer>
#include <QUdpSocket>

#include "navdata_common.h"

class DroneModel;

// Pushes the navdata into an UDP socket, port 5554
class NavdataGenerator : public QThread
{
    Q_OBJECT
public:
    explicit NavdataGenerator(QObject *parent = 0);

    void run();

signals:
    void initializeDrone(QHostAddress);

public slots:
    void dataInNavSocket();
    void navdataTimerTick();

    void sendDroneStatusWithCmdMask();

private:
    void sendNavdata();
    void prepareDatagram();
    QUdpSocket* m_navdataSock;
    const quint16 m_navdataPort;
    QHostAddress m_hostAddr;

    DroneModel* m_droneModel;
    QByteArray m_navdataBuf;

    navdata_t m_navdata;

    QTimer* m_timer;



};

#endif // NAVDATAGENERATOR_H
