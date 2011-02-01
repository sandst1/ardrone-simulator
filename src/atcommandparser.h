#ifndef ATCOMMANDPARSER_H
#define ATCOMMANDPARSER_H

#include <QObject>

class ATCommandParser : public QObject
{
    Q_OBJECT
public:
    explicit ATCommandParser(QObject *parent = 0);

signals:

public slots:

};

#endif // ATCOMMANDPARSER_H
