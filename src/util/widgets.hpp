#ifndef UTIL_WIDGETS_HPP
#define UTIL_WIDGETS_HPP

#include <QDockWidget>

class CloseDockWidget : public QDockWidget {
    Q_OBJECT

public:
    explicit CloseDockWidget(QWidget *parent = nullptr,
                         Qt::WindowFlags flags = Qt::WindowFlags())
        : QDockWidget(parent, flags) {}
    explicit CloseDockWidget(const QString &title, QWidget *parent = nullptr)
        : QDockWidget(title, parent) {}

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void closed();
};

#endif