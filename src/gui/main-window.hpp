#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP
#include <QMainWindow>

#include "game/state.hpp"
#include "gui/settings/settings-dialog.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent);
    ~MainWindow();
private slots:
    void onMoveMade(Move move);
    void onSettingsShow();
    void onEnginesShow();
    void onSettingsClose();
    void onBoardReset();
    void onPositionChanged();
    void onPositionSet(size_t);
    void onSetFen();
    void closeEvent(QCloseEvent *);

private:
    void stateChanged();

    Ui::MainWindow *m_ui;
    State m_state;
    // Settings dialog
    SettingsDialog *m_settingsDialog;
};

#endif  // MAIN_WINDOW_HPP
