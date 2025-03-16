#ifndef NBTWIDGET_H
#define NBTWIDGET_H

#include <QAction>
#include <QListWidgetItem>
#include <QMenu>
#include <QTreeWidgetItem>
#include <QWidget>

#include "palette.h"

namespace Ui {
    class NbtWidget;
}

using namespace bl::palette;

// Not hold data.
class NBTTreeItem : public QTreeWidgetItem {
public:
    [[nodiscard]] inline QString getRawText() const {
        if (!root_) return ":";
        return QString() + root_->key().c_str() + ": " + root_->value_string().c_str();
    }

    bl::palette::abstract_tag *root_{nullptr};
};

// Hold data.
struct NBTListItem : public QListWidgetItem {
    QString getLabel() {
        auto dyn = this->namer_(root_);
        return dyn.size() == 0 ? default_label : dyn;
    }

    bl::palette::compound_tag *root_{nullptr};                                                                   // 原始数据
    std::function<QString(bl::palette::compound_tag *)> namer_{[](bl::palette::compound_tag *) { return ""; }};  // 动态标签
    QString default_label;                                                                                       // 外显标签
    QString raw_key;  // The original key of the key structure in leveldb
    ~NBTListItem() override { delete this->root_; }

    /*
     * Constructs an NBTListItem without a dynamic label and icon.
     */
    static NBTListItem *from(bl::palette::compound_tag *data, const QString &default_label, const QString &key = "") {
        auto *it = new NBTListItem();
        it->root_ = data;
        it->default_label = default_label;
        it->raw_key = key.isEmpty() ? default_label : key;
        it->setText(it->getLabel());
        return it;
    }
};

// It will hold data itself, so each time data is loaded, it will destroy the previous one and make a copy.
class NbtWidget : public QWidget {
Q_OBJECT
public:
    explicit NbtWidget(QWidget *parent = nullptr);

    ~NbtWidget() override;

    void loadNewData(const std::vector<NBTListItem *> &items);

    void setExtraLoadEvent(const std::function<void(compound_tag *)> &event) { this->extra_load_event_ = event; }

    void hideLoadDataBtn();

    std::string getCurrentPaletteRaw();

    std::vector<compound_tag *> getPaletteCopy();

    void foreachItem(const std::function<void(const std::string &label, compound_tag *data)> &func);

    void refreshLabel();

    void clearData();

    inline void setEnableModifyCache(bool enable) {
        if (!enable) this->clearModifyCache();
        this->enable_modify_cache_ = enable;
    }

    const std::unordered_map<std::string, std::string> &getModifyCache() { return this->modified_cache_; }

    void putModifyCache(const std::string &key, const std::string &value);

    inline void clearModifyCache() { this->modified_cache_.clear(); };

    inline NBTListItem *openedItem() { return this->current_opened_; }

    inline bool modifyAllowed() const { return this->modify_allowed_; }

private slots:

    void on_load_btn_clicked();

    void on_list_widget_itemDoubleClicked(QListWidgetItem *item);

    void on_save_btn_clicked();

    void prepareTreeWidgetMenu(const QPoint &pos);

    void prepareListWidgetMenu(const QPoint &pos);

    void saveNBTs(bool selectOnly);

    void on_tree_widget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_multi_select_checkbox_stateChanged(int arg1);

    void on_modify_checkbox_stateChanged(int arg1);

    void on_search_edit_textEdited(const QString &arg1);

    void on_list_widget_itemSelectionChanged();

private:
    void openNBTItem(bl::palette::compound_tag *root);

private:
    // Do not store data, only reference it.
    Ui::NbtWidget *ui;
    bool modify_allowed_{false};
    std::function<void(compound_tag *)> extra_load_event_{[](const compound_tag *) {}};
    std::unordered_map<std::string, std::string> modified_cache_;
    NBTListItem *current_opened_{nullptr};
    bool enable_modify_cache_{true};
};

#endif  // NBTWIDGET_H
