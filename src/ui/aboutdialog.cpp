#include <QIcon>

#include "aboutdialog.h"
#include "config.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent), ui(new Ui::AboutDialog) {
    ui->setupUi(this);
    setWindowTitle(QString("About ") + cfg::SOFTWARE_NAME.c_str());
    ui->info_label->setText(ui->info_label->text().replace("{NAME_VERSION}", cfg::VERSION_STRING()));
    ui->logo->setPixmap(QPixmap(":/ui/icon.png").scaled(128, 128));
}

AboutDialog::~AboutDialog() { delete ui; }
