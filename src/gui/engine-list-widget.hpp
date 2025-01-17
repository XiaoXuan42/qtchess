#ifndef ENGINE_LIST_WIDGET_HPP
#define ENGINE_LIST_WIDGET_HPP

#include <QTableWidget>
#include <vector>

#include "engine/engine-config.hpp"

class EngineListWidget : public QTableWidget {
    Q_OBJECT
public:
    explicit EngineListWidget(QWidget* parent = 0);
    ~EngineListWidget() = default;

    /*! \brief Returns selected settings instance */
    QString engineName() const;

    /*! \brief Tests whether there is something currently selected */
    bool selected() const;

public slots:
    void update();
private slots:
    void onClicked(int Row, int Column);

private:
    void setUpTableBasic();
    void appendRow(const EngineConfig &config);

    std::vector<EngineConfig> m_engineConfigs;
    int m_currentId;
};

#endif  // ENGINE_LIST_WIDGET_HPP
