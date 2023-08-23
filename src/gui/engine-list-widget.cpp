#include "gui/engine-list-widget.hpp"

#include <QtCore/qnamespace.h>
#include <QtWidgets/qtablewidget.h>
#include <QHeaderView>

#include <QDebug>

#include "settings/settings-factory.hpp"

EngineListWidget::EngineListWidget(QWidget* parent)
    : QTableWidget(parent), m_currentId(-1) {
    QObject::connect(this, SIGNAL(cellClicked(int, int)), this,
                     SLOT(onClicked(int, int)));
    QObject::connect(&SettingsFactory::engines(), &AbstractSettings::changed,
                     this, &EngineListWidget::update);
    setUpTableBasic();
    update();
}

void EngineListWidget::setUpTableBasic() {
    setColumnCount(4);
    setHorizontalHeaderLabels({"Name", "Command", "Cwd", "Arguments"});
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

QString EngineListWidget::engineName() const {
    return m_engineConfigs[m_currentId].name();
}

bool EngineListWidget::selected() const { return m_currentId != -1; }

void EngineListWidget::appendRow(const EngineConfig& config) {
    insertRow(rowCount());
    const int curRow = rowCount() - 1;
    auto createItem = [](const QString& str, bool editable) {
        auto newItem = new QTableWidgetItem(str);
        auto flags = newItem->flags();
        flags.setFlag(Qt::ItemIsDragEnabled, false);
        flags.setFlag(Qt::ItemIsDropEnabled, false);
        flags.setFlag(Qt::ItemIsEditable, editable);
        newItem->setFlags(flags);
        return newItem;
    };
    setItem(curRow, 0, createItem(config.name(), false));
    setItem(curRow, 1, createItem(config.command(), false));
    setItem(curRow, 2, createItem(config.workdir(), false));
    setItem(curRow, 3, createItem(config.arguments(), false));
}

void EngineListWidget::update() {
    m_engineConfigs = SettingsFactory::engines().configs();
    m_currentId = -1;
    setRowCount(0);  // clear all rows
    for (auto& config : m_engineConfigs) {
        appendRow(config);
    }
}

void EngineListWidget::onClicked(int row, int column) { m_currentId = row; }
