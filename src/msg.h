#pragma once

#include <QMessageBox>

void WARN(const QString& msg);

void INFO(const QString& msg);

bool CHECK_CONDITION(bool c, const QString& msg);

void CHECK_RESULT(bool c, const QString& succ, const QString& fail);

void CHECK_DATA_SAVE(bool c);
