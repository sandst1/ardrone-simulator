#ifndef NAVDATAGENERATOR_H
#define NAVDATAGENERATOR_H

#include <QThread>
#include <QTimer>
#include <QUdpSocket>

#include "ardrone_common.h"

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
    void startNavdataStream();

    void dataInNavSocket();
    void navdataTimerTick();

    void sendDroneStatusWithCmdMask();   

private:
    void sendNavdata();
    void prepareDatagram();
    void addNavdataDemoToDatagram();

    void initializeNavdataDemo();


    QUdpSocket* m_navdataSock;
    const quint16 m_navdataPort;
    QHostAddress m_hostAddr;

    DroneModel* m_droneModel;
    char m_navdataBuf[2048];

    navdata_t m_navdata;
    navdata_demo_t m_navdataDemo;

    QTimer* m_timer;

    bool m_initialized;



};

#endif // NAVDATAGENERATOR_H
