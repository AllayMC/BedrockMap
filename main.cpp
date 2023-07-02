#include <QApplication>
#include <QCache>
#include <QDebug>
#include <QDesktopWidget>
#include <QIcon>
#include <QImage>

#include "asynclevelloader.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QFile stylesheet(":/res/MaterialDark.qss");
    stylesheet.open(QFile::ReadOnly);
    QString setSheet = QLatin1String(stylesheet.readAll());
    QApplication a(argc, argv);
    QFont f = a.font();
    f.setFamily("微软雅黑");
    f.setPointSize(10);
    a.setFont(f);
    MainWindow w;
    //w.setStyleSheet(setSheet);
    auto const rec = QApplication::desktop()->screenGeometry();
    auto const height = std::max(static_cast<int>(rec.height() * 0.667), 720);
    auto const width = std::max(static_cast<int>(rec.width() * 0.667), 1368);
    w.setGeometry((rec.width() - width) / 2, (rec.height() - height) / 2, width, height);
    w.setWindowTitle("LevelMap v0.1");
    QIcon icon(":/res/map.ico");  // 图标文件的资源路径
    w.setWindowIcon(icon);        // 设置窗口图标
    w.show();
    return a.exec();
}
