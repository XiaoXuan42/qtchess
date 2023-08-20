#include <QApplication>

#include "gui/main-window.hpp"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w(nullptr);
    w.show();

    return a.exec();
}
