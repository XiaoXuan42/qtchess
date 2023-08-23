#include "engine-edit-dialog.hpp"
#include <QtWidgets/qfiledialog.h>

#include <QCheckBox>
#include <QDebug>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>
#include <QFileDialog>
#include <filesystem>

#include "engine-edit-options-dialog.hpp"
#include "engine/engine-config.hpp"
#include "ui_engine-edit-dialog.h"

EngineEditDialog::EngineEditDialog(EngineConfig config, QWidget* parent)
    : QDialog(parent),
      ui(new Ui::EngineEditDialog),
      m_config(config),
      m_registeredEngines(SettingsFactory::engines().names()) {
    ui->setupUi(this);

    QObject::connect(ui->ok, SIGNAL(clicked(bool)), this, SLOT(onOkClicked()));
    QObject::connect(ui->cancel, SIGNAL(clicked(bool)), this,
                     SLOT(onCancelClicked()));
    QObject::connect(ui->configure, SIGNAL(clicked(bool)), this,
                     SLOT(onConfigureClicked()));
    QObject::connect(ui->commandBtn, SIGNAL(clicked(bool)), this, SLOT(onCommandBtnClicked()));
    QObject::connect(ui->workdirBtn, SIGNAL(clicked(bool)), this, SLOT(onWorkDirBtnClicked()));

    ui->name->setText(config.name());
    ui->command->setText(config.command());
    ui->workdir->setText(config.workdir());
    ui->arguments->setText(config.arguments());

    m_registeredEngines.removeOne(config.name());
}

EngineEditDialog::~EngineEditDialog() { delete ui; }

EngineConfig EngineEditDialog::engineConfig() const { return m_config; }

void EngineEditDialog::onOkClicked() {
    if (settingsValid()) emit accept();
}

void EngineEditDialog::onCancelClicked() { emit reject(); }

void EngineEditDialog::onConfigureClicked() {
    if (settingsValid()) {
        QScopedPointer<EngineEditOptionsDialog> dialog(
            new EngineEditOptionsDialog(m_config, this));

        if (dialog->exec() == QDialog::Accepted)
            m_config = dialog->engineConfig();
    }
}

bool EngineEditDialog::settingsValid() {
    // Already registered. Cannot create such an engine.
    if (m_registeredEngines.contains(ui->name->text()))
        QMessageBox::warning(this, tr("Engine duplication"),
                             tr("Engine with such name already exists."));
    else if (ui->name->text().isEmpty()) {
        QMessageBox::warning(this, tr("Engine name empty"),
                             tr("Engine must have a name."));
    } else {
        m_config.setName(ui->name->text());
        m_config.setCommand(ui->command->text());
        m_config.setWorkdir(ui->workdir->text());
        m_config.setArguments(ui->arguments->text());

        // Try to start an engine.
        try {
            Engine(m_config).start();
            // It started!
            return true;
        } catch (std::runtime_error& error) {
            // It didn't start :-(
            QMessageBox::critical(this, tr("Engine executable error"),
                                  tr(error.what()));
        }
    }

    return false;
}

void EngineEditDialog::onCommandBtnClicked() {
    QString path;
    QString inputPath = ui->command->text();
    if (std::filesystem::is_directory(std::filesystem::path(inputPath.toStdU16String()))) {
        path = QFileDialog::getOpenFileName(this, tr("Open File"), inputPath);
    } else {
        path = QFileDialog::getOpenFileName(this, tr("Open File"));
    }
    if (!path.isEmpty()) {
        ui->command->setText(path);
    }
}

void EngineEditDialog::onWorkDirBtnClicked() {
    QString path;
    QString inputPath = ui->workdir->text();
    if (std::filesystem::is_directory(std::filesystem::path(inputPath.toStdU16String()))) {
        path = QFileDialog::getOpenFileName(this, tr("Open Directory"), inputPath);
    } else {
        path = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
    }
    if (!path.isEmpty()) {
        ui->workdir->setText(path);
    }
}
