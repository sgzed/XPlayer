/********************************************************************************
** Form generated from reading UI file 'vsqt.ui'
**
** Created by: Qt User Interface Compiler version 5.14.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VSQT_H
#define UI_VSQT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VsqtClass
{
public:

    void setupUi(QWidget *VsqtClass)
    {
        if (VsqtClass->objectName().isEmpty())
            VsqtClass->setObjectName(QString::fromUtf8("VsqtClass"));
        VsqtClass->resize(600, 400);

        retranslateUi(VsqtClass);

        QMetaObject::connectSlotsByName(VsqtClass);
    } // setupUi

    void retranslateUi(QWidget *VsqtClass)
    {
        VsqtClass->setWindowTitle(QCoreApplication::translate("VsqtClass", "Vsqt", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VsqtClass: public Ui_VsqtClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VSQT_H
