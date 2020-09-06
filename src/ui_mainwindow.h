/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>
#include "src/openglwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    openGlWidget *glWidget;
    QToolButton *toolButtonFile;
    QToolButton *toolButtonSymmetry;
    QLabel *pointGroupTitle;
    QLabel *pointGroupLabel;
    QLabel *label_3;
    QLabel *chemicalFormulaLabel;
    QLabel *label_5;
    QLabel *molarMassLabel;
    QToolButton *toolButtonColor;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1080, 720);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        glWidget = new openGlWidget(centralWidget);
        glWidget->setObjectName(QStringLiteral("glWidget"));
        glWidget->setGeometry(QRect(250, 50, 800, 600));
        toolButtonFile = new QToolButton(centralWidget);
        toolButtonFile->setObjectName(QStringLiteral("toolButtonFile"));
        toolButtonFile->setGeometry(QRect(20, 50, 81, 41));
        toolButtonSymmetry = new QToolButton(centralWidget);
        toolButtonSymmetry->setObjectName(QStringLiteral("toolButtonSymmetry"));
        toolButtonSymmetry->setGeometry(QRect(20, 110, 81, 41));
        pointGroupTitle = new QLabel(centralWidget);
        pointGroupTitle->setObjectName(QStringLiteral("pointGroupTitle"));
        pointGroupTitle->setGeometry(QRect(15, 400, 150, 20));
        pointGroupLabel = new QLabel(centralWidget);
        pointGroupLabel->setObjectName(QStringLiteral("pointGroupLabel"));
        pointGroupLabel->setGeometry(QRect(150, 400, 80, 20));
        pointGroupLabel->setTextFormat(Qt::AutoText);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(15, 350, 150, 19));
        chemicalFormulaLabel = new QLabel(centralWidget);
        chemicalFormulaLabel->setObjectName(QStringLiteral("chemicalFormulaLabel"));
        chemicalFormulaLabel->setGeometry(QRect(150, 350, 80, 19));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(15, 450, 150, 19));
        molarMassLabel = new QLabel(centralWidget);
        molarMassLabel->setObjectName(QStringLiteral("molarMassLabel"));
        molarMassLabel->setGeometry(QRect(150, 450, 80, 19));
        toolButtonColor = new QToolButton(centralWidget);
        toolButtonColor->setObjectName(QStringLiteral("toolButtonColor"));
        toolButtonColor->setGeometry(QRect(20, 170, 81, 41));
        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        toolButtonFile->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Charger un fichier</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        toolButtonFile->setText(QApplication::translate("MainWindow", "Fichier", Q_NULLPTR));
        toolButtonSymmetry->setText(QApplication::translate("MainWindow", "Symmetry", Q_NULLPTR));
        pointGroupTitle->setText(QApplication::translate("MainWindow", "Point Group", Q_NULLPTR));
        pointGroupLabel->setText(QApplication::translate("MainWindow", "C<sub>1</sub>", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindow", "Chemical Formula", Q_NULLPTR));
        chemicalFormulaLabel->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
        label_5->setText(QApplication::translate("MainWindow", "Molar mass", Q_NULLPTR));
        molarMassLabel->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
        toolButtonColor->setText(QApplication::translate("MainWindow", "Color", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
