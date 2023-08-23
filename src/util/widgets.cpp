#include "util/widgets.hpp"


void CloseDockWidget::closeEvent(QCloseEvent *event) {
    emit closed();
    QDockWidget::closeEvent(event);
}