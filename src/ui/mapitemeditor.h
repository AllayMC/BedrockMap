#pragma once

#include "level/global.h"
#include "ui/nbtwidget.h"
#include <QWidget>

namespace Ui {
class MapItemEditor;
}

class MainWindow;

class MapItemEditor : public QWidget {
    Q_OBJECT

public:
    explicit MapItemEditor(MainWindow* mw, QWidget* parent = nullptr);

    ~MapItemEditor() override;

    void load_map_data(const bl::general_kv_nbts& data);

    void paintEvent(QPaintEvent* event) override;

    void clearData() { this->map_nbt_editor_->clearData(); }

private slots:

    void on_export_map_btn_clicked();

    void on_change_map_btn_clicked();

    void on_save_map_btn_clicked();

private:
    NbtWidget*         map_nbt_editor_{nullptr};
    Ui::MapItemEditor* ui;
    MainWindow*        mw_{nullptr};
    QImage             img;
};
