#ifndef NBTWIDGET_H
#define NBTWIDGET_H

#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <QWidget>
#include <QAction>

#include "palette.h"
#include <QMenu>

namespace Ui {
    class NbtWidget;
}


//不持有数据
class NBTTreeItem : public QTreeWidgetItem {
public:

    [[nodiscard]] inline QString getRawText() const {
        if (!root_)return ":";
        return QString() + root_->key().c_str() + ": " + root_->value_string().c_str();
    }

    bl::palette::abstract_tag *root_{nullptr};


    bool prevent_item_change_event{false};
};
//持有数据

struct NBTListItem : public QListWidgetItem {


    void refreshText() {
        auto label = this->namer_(root_);
        label = label.size() == 0 ? default_label : label;
        this->setText(label);
    }

    bl::palette::compound_tag *root_{nullptr};
    std::function<QString(bl::palette::compound_tag *)> namer_;
    QString default_label;

    ~NBTListItem() override {
        delete this->root_;
    }
};


//自身会持有数据，所以每次加载数据会析构之前的并复制一份
class NbtWidget : public QWidget {

Q_OBJECT
public:
    explicit NbtWidget(QWidget *parent = nullptr);

    ~NbtWidget() override;

    void load_new_data(const std::vector<bl::palette::compound_tag *> &data,
                       const std::function<QString(bl::palette::compound_tag *)> &namer);

    void hideLoadDataBtn();


private slots:

    void on_load_btn_clicked();

    void on_list_widget_itemDoubleClicked(QListWidgetItem *item);

//    void on_tree_widget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_tree_widget_itemChanged(QTreeWidgetItem *item, int column);


    void on_save_btn_clicked();

    void prepareTreeWidgetMenu(const QPoint &pos);

    void prepareListWidgetMenu(const QPoint &pos);


    void on_tree_widget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_save_current_clicked();

private:
    void loadNBTItem(bl::palette::compound_tag *root);

private:
    //不存数据，只引用数据
//    std::vector<bl::palette::compound_tag *> nbts_;
    Ui::NbtWidget *ui;
    std::function<QString(bl::palette::compound_tag *, int index)> list_label_generator_;
};

#endif  // NBTWIDGET_H
