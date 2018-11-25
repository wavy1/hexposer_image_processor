#include <QtWidgets/QApplication>
#include "HexposerServerCommunicator.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QApplication::setApplicationDisplayName(HexposerServerCommunicator::tr("Fortune Server"));
    HexposerServerCommunicator server;
    server.show();
    return app.exec();
}
