#include "gui/main-window.hpp"

#include <QtCore/qnamespace.h>
#include <QtCore/qobject.h>

#include <QInputDialog>
#include <QMessageBox>
#include <algorithm>

#include "game/board.hpp"
#include "gui/engine/engine-widget.hpp"
#include "gui/settings/engine-settings-dialog.hpp"
#include "gui/settings/settings-dialog.hpp"
#include "settings/settings-factory.hpp"
#include "ui_main-window.h"
#include "util/widgets.hpp"

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
    QObject::connect(m_ui->actionReset, SIGNAL(triggered()), this,
                     SLOT(onBoardReset()));
    QObject::connect(m_ui->actionSetFen, SIGNAL(triggered()), this,
                     SLOT(onSetFen()));
    QObject::connect(m_ui->actionQuit, SIGNAL(triggered()), this,
                     SLOT(close()));
    QObject::connect(m_ui->actionEngineConfigs, SIGNAL(triggered()), this,
                     SLOT(onConfigEngine()));

    // Connect text widget signals
    QObject::connect(m_ui->GameTextWidget, &MoveTreeWidget::moveSelected, this,
                     &MainWindow::onPositionSet);
    onEngineListChanged(SettingsFactory::engines().names());
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
        QObject::connect(m_settingsDialog, &SettingsDialog::finished, [this]() {
            this->m_settingsDialog->deleteLater();
            this->m_settingsDialog = nullptr;
        });
    }
    m_settingsDialog->show();
}

void MainWindow::onBoardReset() {
    m_state.reset();
    stateChanged();
}

void MainWindow::onPositionChanged() { stateChanged(); }

void MainWindow::onPositionSet(size_t uid) {
    m_state.setByTreeNode(TreeNode::fromUid(uid));
    stateChanged();
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
    std::for_each(m_engineWidgets.begin(), m_engineWidgets.end(), [this](EngineWidget *p) {
        p->setBoard(this->m_state.getBoard());
    });
}

void MainWindow::onConfigEngine() {
    auto dialog = new EngineSettingsDialog(this);
    QObject::connect(dialog, &EngineSettingsDialog::engineSettingsUpdate, this,
                     &MainWindow::onEngineListChanged);
    QObject::connect(dialog, &EngineSettingsDialog::finished,
                     [dialog]() { dialog->deleteLater(); });
    dialog->show();
}

void MainWindow::onEngineListChanged(QStringList engineList) {
    bool startRemove = false;
    std::vector<QAction *> toRemove;
    for (auto action : m_ui->menuEngine->actions()) {
        if (action->isSeparator()) {
            startRemove = true;
            continue;
        }
        if (startRemove) {
            toRemove.push_back(action);
        }
    }
    std::for_each(toRemove.begin(), toRemove.end(), [&](QAction *pAction) {
        m_ui->menuEngine->removeAction(pAction);
        pAction->deleteLater();
    });
    for (auto &engineName : engineList) {
        QAction *newAction = new QAction(this);
        newAction->setObjectName(engineName);
        newAction->setText(engineName);
        m_ui->menuEngine->addAction(newAction);
        QObject::connect(newAction, &QAction::triggered, [engineName, this]() {
            this->createEnginePanel(engineName);
        });
    }
}

void MainWindow::createEnginePanel(const QString &name) {
    auto *dock = new CloseDockWidget(this);
    auto engineConfig = SettingsFactory::engines().config(name);
    EnginePtr pEngine = std::make_unique<Engine>(engineConfig);
    auto *enginePanel = new EngineWidget(std::move(pEngine), name, this);
    dock->setWidget(enginePanel);
    this->addDockWidget(Qt::RightDockWidgetArea, dock);
    this->m_engineWidgets.insert(enginePanel);
    QObject::connect(dock, &CloseDockWidget::closed,
                     [dock, enginePanel, this]() {
                         enginePanel->deleteLater();
                         dock->deleteLater();
                         this->m_engineWidgets.erase(enginePanel);
                     });
    dock->show();
}
