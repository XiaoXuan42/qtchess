#include "gui/main-window.hpp"

#include <QInputDialog>
#include <QMessageBox>

#include "game/board.hpp"
#include "gui/settings/engine-settings-dialog.hpp"
#include "gui/settings/settings-dialog.hpp"
#include "settings/settings-factory.hpp"
#include "ui_main-window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(new Ui::MainWindow), m_settingsDialog(nullptr) {
    m_ui->setupUi(this);
    setWindowTitle("QtChess");
    // Setup widgets
    m_ui->Board->setGameState(&m_state);
    m_ui->GameTextWidget->setState(&m_state);

    auto &layout = SettingsFactory::layout();
    restoreGeometry(
        layout.get(LayoutSettings::MAIN_WINDOW_GEOMETRY).toByteArray());
    restoreState(layout.get(LayoutSettings::MAIN_WINDOW_STATE).toByteArray());

    // Connect board signals
    QObject::connect(m_ui->Board, SIGNAL(moveMade(Move)), this,
                     SLOT(onMoveMade(Move)));
    QObject::connect(m_ui->actionFlip, SIGNAL(triggered()), m_ui->Board,
                     SLOT(flip()));
    // Connect action signals
    QObject::connect(m_ui->actionSettings, SIGNAL(triggered()), this,
                     SLOT(onSettingsShow()));
    QObject::connect(m_ui->actionEngines, SIGNAL(triggered()), this,
                     SLOT(onEnginesShow()));
    QObject::connect(m_ui->actionReset, SIGNAL(triggered()), this,
                     SLOT(onBoardReset()));
    QObject::connect(m_ui->actionSetFen, SIGNAL(triggered()), this,
                     SLOT(onSetFen()));
    QObject::connect(m_ui->actionQuit, SIGNAL(triggered()), this,
                     SLOT(close()));

    // Connect text widget signals
    QObject::connect(m_ui->GameTextWidget, &MoveTreeWidget::moveSelected, this,
                     &MainWindow::onPositionSet);
}

MainWindow::~MainWindow() { delete m_ui; }

void MainWindow::onMoveMade(Move move) {
    MoveType moveType = MoveType::MOVE_NONSPECIAL;
    if (m_state.addMove(move)) {
        onPositionChanged();
    }
}

void MainWindow::onSettingsShow() {
    if (!m_settingsDialog) {
        m_settingsDialog = new SettingsDialog(this);
        QObject::connect(m_settingsDialog, SIGNAL(rejected()), this,
                         SLOT(onSettingsClose()));
    }
    m_settingsDialog->show();
}

void MainWindow::onSettingsClose() {
    delete m_settingsDialog;
    m_settingsDialog = nullptr;
}

void MainWindow::onEnginesShow() {
    QScopedPointer<EngineSettingsDialog>(new EngineSettingsDialog(this))
        ->exec();
}

void MainWindow::onBoardReset() { m_state.reset(); stateChanged(); }

void MainWindow::onPositionChanged() {
    stateChanged();
}

void MainWindow::onPositionSet(size_t uid) {
    m_state.setByTreeNode(TreeNode::fromUid(uid));
}

void MainWindow::onSetFen() {
    Board board;
    QInputDialog dialog(this);
    dialog.setWindowTitle("Enter position");
    dialog.setLabelText("FEN string:");

    while (dialog.exec() == QDialog::Accepted) {
        // Invalid FEN.
        if (!board.setFen(dialog.textValue())) {
            QMessageBox::information(
                this, "Fen string invalid",
                tr("Passed string '%1' is not a valid FEN string")
                    .arg(dialog.textValue()));
        } else {
            m_state.reset(board);
            stateChanged();
            break;
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *) {
    auto &layout = SettingsFactory::layout();
    layout.set(LayoutSettings::MAIN_WINDOW_GEOMETRY, saveGeometry());
    layout.set(LayoutSettings::MAIN_WINDOW_STATE, saveState());
    layout.save();
}

void MainWindow::stateChanged() {
    m_ui->Board->redraw();
    m_ui->GameTextWidget->redraw();
    m_ui->engineWidget->setBoard(m_state.getBoard());
}