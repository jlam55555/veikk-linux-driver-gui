#include "unistd.h"
#include "veikkconfig.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a{argc, argv};

    // TODO: check if user is root; if not, issue warning
    if(geteuid()) {}

    MainWindow w;
    w.show();
    return a.exec();
}
