#pragma once

#include <QtWidgets/QWidget>
#include "ui_XPlayer2.h"

class XPlayer2 : public QWidget
{
    Q_OBJECT

public:
    XPlayer2(QWidget *parent = Q_NULLPTR);

public:
    Ui::XPlayer2Class ui;
};
