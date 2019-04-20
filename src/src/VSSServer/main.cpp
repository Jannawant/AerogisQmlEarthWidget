
#include <QtCore/QCoreApplication>
#include "../AerogisPlugin/VSSComReader.h"
#include "VSSServer.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "VSS Server\n\n";

    QString scom = "COM2", sport = "2345", conf = "aero.conf", earth = "aero.earth";

    if (argc == 5)
    {
       scom = argv[1];
       sport = argv[2];
       conf = argv[3];
       earth = argv[4];
    }
    else
    {
       std::cout << "usage: VSSServer COM2 2345 aero.conf aero.earth\n\n";
       if (argc != 1)
          return 1;
    }

    std::cout << "Starting with COM=" << scom.toStdString() << " ports=" << (sport + "-%1").arg(sport.toInt() + 3).toStdString() << " config=" << conf.toStdString() << " earth=" << earth.toStdString() << "\n";

    VSSComReader com(scom, 0, true);

    bool ok;
    VSSServer srv(sport.toInt(), conf, earth, com.mBuf1, com.mBuf2, com.mMut, ok, 0);
    if (!ok)
       return 2;

    srv.connect(&com, &VSSComReader::dataReceived, &srv, &VSSServer::onDataReceived);

    return a.exec();
}
