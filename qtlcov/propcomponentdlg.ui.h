#ifndef PROPCOMPONENTDLG_H
#define PROPCOMPONENTDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_propcomponentdlg
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QLabel *textLabel1;
    QLineEdit *editName;
    QLabel *textLabel2;
    QLineEdit *editRef;
    QHBoxLayout *hboxLayout1;
    QVBoxLayout *vboxLayout1;
    QGridLayout *gridLayout;
    QLineEdit *editHeight;
    QLabel *textLabel4;
    QLabel *textLabel3;
    QLineEdit *editWidth;
    QSpacerItem *spacerItem;
    QVBoxLayout *vboxLayout2;
    QHBoxLayout *hboxLayout2;
    QLabel *textLabel5;
    QLineEdit *editBlock;
    QCheckBox *checkUsePtn;
    QCheckBox *checkNotUseBitPtn;
    QSpacerItem *spacerItem1;
    QVBoxLayout *vboxLayout3;
    QPushButton *buttonOK;
    QPushButton *buttonCancel;
    QSpacerItem *spacerItem2;

    void setupUi(QDialog *propcomponentdlg) {
        propcomponentdlg->setObjectName(QString::fromUtf8("propcomponentdlg"));
        propcomponentdlg->resize(QSize(388, 128).expandedTo(propcomponentdlg->minimumSizeHint()));
        propcomponentdlg->setModal(true);
        vboxLayout = new QVBoxLayout(propcomponentdlg);
        vboxLayout->setSpacing(6);
        vboxLayout->setMargin(9);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setMargin(0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        textLabel1 = new QLabel(propcomponentdlg);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));

        hboxLayout->addWidget(textLabel1);

        editName = new QLineEdit(propcomponentdlg);
        editName->setObjectName(QString::fromUtf8("editName"));
        editName->setMaxLength(15);

        hboxLayout->addWidget(editName);

        textLabel2 = new QLabel(propcomponentdlg);
        textLabel2->setObjectName(QString::fromUtf8("textLabel2"));

        hboxLayout->addWidget(textLabel2);

        editRef = new QLineEdit(propcomponentdlg);
        editRef->setObjectName(QString::fromUtf8("editRef"));
        editRef->setMaximumSize(QSize(100, 32767));
        editRef->setMaxLength(16);

        hboxLayout->addWidget(editRef);


        vboxLayout->addLayout(hboxLayout);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setMargin(0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setSpacing(6);
        vboxLayout1->setMargin(0);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setMargin(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        editHeight = new QLineEdit(propcomponentdlg);
        editHeight->setObjectName(QString::fromUtf8("editHeight"));
        editHeight->setMaximumSize(QSize(80, 32767));

        gridLayout->addWidget(editHeight, 1, 1, 1, 1);

        textLabel4 = new QLabel(propcomponentdlg);
        textLabel4->setObjectName(QString::fromUtf8("textLabel4"));

        gridLayout->addWidget(textLabel4, 1, 0, 1, 1);

        textLabel3 = new QLabel(propcomponentdlg);
        textLabel3->setObjectName(QString::fromUtf8("textLabel3"));

        gridLayout->addWidget(textLabel3, 0, 0, 1, 1);

        editWidth = new QLineEdit(propcomponentdlg);
        editWidth->setObjectName(QString::fromUtf8("editWidth"));
        editWidth->setMaximumSize(QSize(80, 32767));
        editWidth->setMaxLength(3);

        gridLayout->addWidget(editWidth, 0, 1, 1, 1);


        vboxLayout1->addLayout(gridLayout);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout1->addItem(spacerItem);


        hboxLayout1->addLayout(vboxLayout1);

        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setSpacing(6);
        vboxLayout2->setMargin(0);
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setSpacing(6);
        hboxLayout2->setMargin(0);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        textLabel5 = new QLabel(propcomponentdlg);
        textLabel5->setObjectName(QString::fromUtf8("textLabel5"));

        hboxLayout2->addWidget(textLabel5);

        editBlock = new QLineEdit(propcomponentdlg);
        editBlock->setObjectName(QString::fromUtf8("editBlock"));
        editBlock->setMaximumSize(QSize(80, 32767));

        hboxLayout2->addWidget(editBlock);


        vboxLayout2->addLayout(hboxLayout2);

        checkUsePtn = new QCheckBox(propcomponentdlg);
        checkUsePtn->setObjectName(QString::fromUtf8("checkUsePtn"));

        vboxLayout2->addWidget(checkUsePtn);

        checkNotUseBitPtn = new QCheckBox(propcomponentdlg);
        checkNotUseBitPtn->setObjectName(QString::fromUtf8("checkNotUseBitPtn"));

        vboxLayout2->addWidget(checkNotUseBitPtn);

        spacerItem1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout2->addItem(spacerItem1);


        hboxLayout1->addLayout(vboxLayout2);

        vboxLayout3 = new QVBoxLayout();
        vboxLayout3->setSpacing(6);
        vboxLayout3->setMargin(0);
        vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
        buttonOK = new QPushButton(propcomponentdlg);
        buttonOK->setObjectName(QString::fromUtf8("buttonOK"));

        vboxLayout3->addWidget(buttonOK);

        buttonCancel = new QPushButton(propcomponentdlg);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

        vboxLayout3->addWidget(buttonCancel);

        spacerItem2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout3->addItem(spacerItem2);


        hboxLayout1->addLayout(vboxLayout3);


        vboxLayout->addLayout(hboxLayout1);

        textLabel1->setBuddy(editName);
        textLabel2->setBuddy(editRef);
        textLabel4->setBuddy(editHeight);
        textLabel3->setBuddy(editWidth);
        QWidget::setTabOrder(editName, editRef);
        QWidget::setTabOrder(editRef, editWidth);
        QWidget::setTabOrder(editWidth, editHeight);
        QWidget::setTabOrder(editHeight, editBlock);
        QWidget::setTabOrder(editBlock, checkUsePtn);
        QWidget::setTabOrder(checkUsePtn, checkNotUseBitPtn);
        QWidget::setTabOrder(checkNotUseBitPtn, buttonOK);
        QWidget::setTabOrder(buttonOK, buttonCancel);
        retranslateUi(propcomponentdlg);
        QObject::connect(buttonCancel, SIGNAL(clicked()), propcomponentdlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(propcomponentdlg);
    } // setupUi

    void retranslateUi(QDialog *propcomponentdlg) {
        propcomponentdlg->setWindowTitle(QApplication::translate("propcomponentdlg", "Component Property", 0, QApplication::UnicodeUTF8));
        textLabel1->setText(QApplication::translate("propcomponentdlg", "Name", 0, QApplication::UnicodeUTF8));
        textLabel2->setText(QApplication::translate("propcomponentdlg", "Reference", 0, QApplication::UnicodeUTF8));
        textLabel4->setText(QApplication::translate("propcomponentdlg", "Height", 0, QApplication::UnicodeUTF8));
        textLabel3->setText(QApplication::translate("propcomponentdlg", "Width", 0, QApplication::UnicodeUTF8));
        textLabel5->setText(QApplication::translate("propcomponentdlg", "Block", 0, QApplication::UnicodeUTF8));
        checkUsePtn->setText(QApplication::translate("propcomponentdlg", "Use Pattern", 0, QApplication::UnicodeUTF8));
        checkNotUseBitPtn->setText(QApplication::translate("propcomponentdlg", "Not Use Bit Pattern", 0, QApplication::UnicodeUTF8));
        buttonOK->setText(QApplication::translate("propcomponentdlg", "OK", 0, QApplication::UnicodeUTF8));
        buttonCancel->setText(QApplication::translate("propcomponentdlg", "Cancel", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(propcomponentdlg);
    } // retranslateUi

};

namespace Ui
{
class propcomponentdlg: public Ui_propcomponentdlg {};
} // namespace Ui

#endif // PROPCOMPONENTDLG_H
