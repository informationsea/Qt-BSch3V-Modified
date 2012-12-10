#ifndef PROPPINDLG_H
#define PROPPINDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_proppindlg
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox1;
    QGridLayout *gridLayout1;
    QRadioButton *radioZero;
    QRadioButton *radioCk;
    QRadioButton *radioS;
    QRadioButton *radioNeg;
    QRadioButton *radioNegCk;
    QRadioButton *radioNorm;
    QVBoxLayout *vboxLayout;
    QPushButton *buttonOK;
    QPushButton *buttonCancel;
    QSpacerItem *spacerItem;
    QCheckBox *checkHide;
    QHBoxLayout *hboxLayout;
    QHBoxLayout *hboxLayout1;
    QLabel *textLabel2;
    QLineEdit *editNumber;
    QHBoxLayout *hboxLayout2;
    QLabel *textLabel2_2;
    QSpinBox *spinBlock;
    QHBoxLayout *hboxLayout3;
    QLabel *textLabel1;
    QLineEdit *editName;

    void setupUi(QDialog *proppindlg) {
        proppindlg->setObjectName(QString::fromUtf8("proppindlg"));
        proppindlg->resize(QSize(411, 200).expandedTo(proppindlg->minimumSizeHint()));
        proppindlg->setMinimumSize(QSize(370, 200));
        proppindlg->setModal(true);
        gridLayout = new QGridLayout(proppindlg);
        gridLayout->setSpacing(6);
        gridLayout->setMargin(9);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox1 = new QGroupBox(proppindlg);
        groupBox1->setObjectName(QString::fromUtf8("groupBox1"));
        gridLayout1 = new QGridLayout(groupBox1);
        gridLayout1->setSpacing(6);
        gridLayout1->setMargin(9);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        radioZero = new QRadioButton(groupBox1);
        radioZero->setObjectName(QString::fromUtf8("radioZero"));
        radioZero->setIcon(QIcon());
        radioZero->setIconSize(QSize(60, 20));

        gridLayout1->addWidget(radioZero, 1, 2, 1, 1);

        radioCk = new QRadioButton(groupBox1);
        radioCk->setObjectName(QString::fromUtf8("radioCk"));
        radioCk->setIcon(QIcon());
        radioCk->setIconSize(QSize(60, 20));
        radioCk->setAutoRepeat(false);

        gridLayout1->addWidget(radioCk, 0, 2, 1, 1);

        radioS = new QRadioButton(groupBox1);
        radioS->setObjectName(QString::fromUtf8("radioS"));
        radioS->setIcon(QIcon());
        radioS->setIconSize(QSize(60, 20));
        radioS->setAutoRepeat(false);

        gridLayout1->addWidget(radioS, 1, 1, 1, 1);

        radioNeg = new QRadioButton(groupBox1);
        radioNeg->setObjectName(QString::fromUtf8("radioNeg"));
        radioNeg->setIcon(QIcon());
        radioNeg->setIconSize(QSize(60, 20));
        radioNeg->setAutoRepeat(false);

        gridLayout1->addWidget(radioNeg, 0, 1, 1, 1);

        radioNegCk = new QRadioButton(groupBox1);
        radioNegCk->setObjectName(QString::fromUtf8("radioNegCk"));
        radioNegCk->setIcon(QIcon());
        radioNegCk->setIconSize(QSize(60, 20));
        radioNegCk->setAutoRepeat(false);

        gridLayout1->addWidget(radioNegCk, 1, 0, 1, 1);

        radioNorm = new QRadioButton(groupBox1);
        radioNorm->setObjectName(QString::fromUtf8("radioNorm"));
        radioNorm->setIcon(QIcon());
        radioNorm->setIconSize(QSize(60, 20));
        radioNorm->setAutoRepeat(false);

        gridLayout1->addWidget(radioNorm, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox1, 3, 0, 1, 1);

        vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(6);
        vboxLayout->setMargin(0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        buttonOK = new QPushButton(proppindlg);
        buttonOK->setObjectName(QString::fromUtf8("buttonOK"));
        QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(buttonOK->sizePolicy().hasHeightForWidth());
        buttonOK->setSizePolicy(sizePolicy);

        vboxLayout->addWidget(buttonOK);

        buttonCancel = new QPushButton(proppindlg);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));
        QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(buttonCancel->sizePolicy().hasHeightForWidth());
        buttonCancel->setSizePolicy(sizePolicy1);

        vboxLayout->addWidget(buttonCancel);

        spacerItem = new QSpacerItem(20, 21, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

        vboxLayout->addItem(spacerItem);


        gridLayout->addLayout(vboxLayout, 3, 1, 1, 1);

        checkHide = new QCheckBox(proppindlg);
        checkHide->setObjectName(QString::fromUtf8("checkHide"));

        gridLayout->addWidget(checkHide, 2, 0, 1, 2);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setMargin(0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setMargin(0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        textLabel2 = new QLabel(proppindlg);
        textLabel2->setObjectName(QString::fromUtf8("textLabel2"));
        textLabel2->setAlignment(Qt::AlignCenter);

        hboxLayout1->addWidget(textLabel2);

        editNumber = new QLineEdit(proppindlg);
        editNumber->setObjectName(QString::fromUtf8("editNumber"));
        QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
        sizePolicy2.setHorizontalStretch(255);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(editNumber->sizePolicy().hasHeightForWidth());
        editNumber->setSizePolicy(sizePolicy2);

        hboxLayout1->addWidget(editNumber);


        hboxLayout->addLayout(hboxLayout1);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setSpacing(6);
        hboxLayout2->setMargin(0);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        textLabel2_2 = new QLabel(proppindlg);
        textLabel2_2->setObjectName(QString::fromUtf8("textLabel2_2"));
        textLabel2_2->setAlignment(Qt::AlignCenter);

        hboxLayout2->addWidget(textLabel2_2);

        spinBlock = new QSpinBox(proppindlg);
        spinBlock->setObjectName(QString::fromUtf8("spinBlock"));

        hboxLayout2->addWidget(spinBlock);


        hboxLayout->addLayout(hboxLayout2);


        gridLayout->addLayout(hboxLayout, 1, 0, 1, 2);

        hboxLayout3 = new QHBoxLayout();
        hboxLayout3->setSpacing(6);
        hboxLayout3->setMargin(0);
        hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
        textLabel1 = new QLabel(proppindlg);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
        textLabel1->setAlignment(Qt::AlignCenter);

        hboxLayout3->addWidget(textLabel1);

        editName = new QLineEdit(proppindlg);
        editName->setObjectName(QString::fromUtf8("editName"));
        QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
        sizePolicy3.setHorizontalStretch(255);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(editName->sizePolicy().hasHeightForWidth());
        editName->setSizePolicy(sizePolicy3);

        hboxLayout3->addWidget(editName);


        gridLayout->addLayout(hboxLayout3, 0, 0, 1, 2);

        QWidget::setTabOrder(editName, editNumber);
        QWidget::setTabOrder(editNumber, spinBlock);
        QWidget::setTabOrder(spinBlock, checkHide);
        QWidget::setTabOrder(checkHide, radioNorm);
        QWidget::setTabOrder(radioNorm, radioCk);
        QWidget::setTabOrder(radioCk, radioNeg);
        QWidget::setTabOrder(radioNeg, radioNegCk);
        QWidget::setTabOrder(radioNegCk, radioS);
        QWidget::setTabOrder(radioS, radioZero);
        QWidget::setTabOrder(radioZero, buttonOK);
        QWidget::setTabOrder(buttonOK, buttonCancel);
        retranslateUi(proppindlg);
        QObject::connect(buttonCancel, SIGNAL(clicked()), proppindlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(proppindlg);
    } // setupUi

    void retranslateUi(QDialog *proppindlg) {
        proppindlg->setWindowTitle(QApplication::translate("proppindlg", "Pin Property", 0, QApplication::UnicodeUTF8));
        groupBox1->setTitle(QApplication::translate("proppindlg", "PinType", 0, QApplication::UnicodeUTF8));
        radioZero->setText(QApplication::translate("proppindlg", "", 0, QApplication::UnicodeUTF8));
        radioCk->setText(QApplication::translate("proppindlg", "", 0, QApplication::UnicodeUTF8));
        radioS->setText(QApplication::translate("proppindlg", "", 0, QApplication::UnicodeUTF8));
        radioNeg->setText(QApplication::translate("proppindlg", "", 0, QApplication::UnicodeUTF8));
        radioNegCk->setText(QApplication::translate("proppindlg", "", 0, QApplication::UnicodeUTF8));
        radioNorm->setText(QApplication::translate("proppindlg", "", 0, QApplication::UnicodeUTF8));
        buttonOK->setText(QApplication::translate("proppindlg", "OK", 0, QApplication::UnicodeUTF8));
        buttonCancel->setText(QApplication::translate("proppindlg", "Cancel", 0, QApplication::UnicodeUTF8));
        checkHide->setText(QApplication::translate("proppindlg", "Hide Number", 0, QApplication::UnicodeUTF8));
        textLabel2->setText(QApplication::translate("proppindlg", "Number", 0, QApplication::UnicodeUTF8));
        textLabel2_2->setText(QApplication::translate("proppindlg", "Block", 0, QApplication::UnicodeUTF8));
        textLabel1->setText(QApplication::translate("proppindlg", "Name", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(proppindlg);
    } // retranslateUi

};

namespace Ui
{
class proppindlg: public Ui_proppindlg {};
} // namespace Ui

#endif // PROPPINDLG_H
