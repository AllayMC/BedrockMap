#include "msg.h"

void WARN(const QString& msg) {
    QMessageBox::warning(
        nullptr,
        "警告",
        msg,
        QMessageBox::Yes,
        QMessageBox::Yes
    );
}

void INFO(const QString& msg) {
    QMessageBox::warning(
        nullptr,
        "信息",
        msg,
        QMessageBox::Yes,
        QMessageBox::Yes
    );
}

bool CHECK_CONDITION(bool c, const QString& msg) {
    if (!c) WARN(msg);
    return c;
}

void CHECK_RESULT(bool c, const QString& succ, const QString& fail) {
    if (c) {
        INFO(succ);
    } else {
        WARN(fail);
    }
}

void CHECK_DATA_SAVE(bool c) {
    CHECK_RESULT(c, "Successfully saved data", "Unable to save data");
}
