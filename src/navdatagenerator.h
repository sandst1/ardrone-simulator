#ifndef NAVDATAGENERATOR_H
#define NAVDATAGENERATOR_H

#include <QThread>
#include <QUdpSocket>

class DroneModel;

// Pushes the navdata into an UDP socket, port 5554
class NavdataGenerator : public QThread
{
    Q_OBJECT
public:
    explicit NavdataGenerator(QObject *parent = 0);

    void run();

    void setHostIP(const QHostAddress& ipaddr);

signals:

public slots:

private:
    QUdpSocket* m_navdataSock;
    const quint16 m_navdataPort;
    QHostAddress m_hostAddr;

    DroneModel* m_droneModel;

};

#endif // NAVDATAGENERATOR_H
