#ifndef EDITPTNTEXTDLG_H
#define EDITPTNTEXTDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_editptntextdlg
{
public:
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout1;
    QLabel *label;
    QLineEdit *editText;
    QHBoxLayout *hboxLayout2;
    QCheckBox *checkUseLabelFont;
    QPushButton *buttonFont;
    QSpacerItem *spacerItem;
    QGroupBox *groupBox;
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout3;
    QRadioButton *radioLeft;
    QRadioButton *radioHCenter;
    QRadioButton *radioRight;
    QWidget *layoutWidget1;
    QHBoxLayout *hboxLayout4;
    QRadioButton *radioTop;
    QRadioButton *radioVCenter;
    QRadioButton *radioBottom;
    QVBoxLayout *vboxLayout1;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QSpacerItem *spacerItem1;

    void setupUi(QDialog *editptntextdlg) {
        editptntextdlg->setObjectName(QString::fromUtf8("editptntextdlg"));
        editptntextdlg->resize(QSize(406, 160).expandedTo(editptntextdlg->minimumSizeHint()));
        editptntextdlg->setMinimumSize(QSize(400, 160));
        hboxLayout = new QHBoxLayout(editptntextdlg);
        hboxLayout->setSpacing(6);
        hboxLayout->setMargin(9);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(6);
        vboxLayout->setMargin(0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setMargin(0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        label = new QLabel(editptntextdlg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        hboxLayout1->addWidget(label);

        editText = new QLineEdit(editptntextdlg);
        editText->setObjectName(QString::fromUtf8("editText"));

        hboxLayout1->addWidget(editText);


        vboxLayout->addLayout(hboxLayout1);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setSpacing(6);
        hboxLayout2->setMargin(0);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        checkUseLabelFont = new QCheckBox(editptntextdlg);
        checkUseLabelFont->setObjectName(QString::fromUtf8("checkUseLabelFont"));

        hboxLayout2->addWidget(checkUseLabelFont);

        buttonFont = new QPushButton(editptntextdlg);
        buttonFont->setObjectName(QString::fromUtf8("buttonFont"));

        hboxLayout2->addWidget(buttonFont);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem);


        vboxLayout->addLayout(hboxLayout2);

        groupBox = new QGroupBox(editptntextdlg);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        groupBox->setMaximumSize(QSize(16777215, 80));
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 50, 254, 20));
        hboxLayout3 = new QHBoxLayout(layoutWidget);
        hboxLayout3->setSpacing(6);
        hboxLayout3->setMargin(0);
        hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
        radioLeft = new QRadioButton(layoutWidget);
        radioLeft->setObjectName(QString::fromUtf8("radioLeft"));
        QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(radioLeft->sizePolicy().hasHeightForWidth());
        radioLeft->setSizePolicy(sizePolicy1);
        radioLeft->setMinimumSize(QSize(80, 0));

        hboxLayout3->addWidget(radioLeft);

        radioHCenter = new QRadioButton(layoutWidget);
        radioHCenter->setObjectName(QString::fromUtf8("radioHCenter"));
        QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(radioHCenter->sizePolicy().hasHeightForWidth());
        radioHCenter->setSizePolicy(sizePolicy2);
        radioHCenter->setMinimumSize(QSize(80, 0));

        hboxLayout3->addWidget(radioHCenter);

        radioRight = new QRadioButton(layoutWidget);
        radioRight->setObjectName(QString::fromUtf8("radioRight"));
        QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(radioRight->sizePolicy().hasHeightForWidth());
        radioRight->setSizePolicy(sizePolicy3);
        radioRight->setMinimumSize(QSize(80, 0));

        hboxLayout3->addWidget(radioRight);

        layoutWidget1 = new QWidget(groupBox);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(10, 20, 254, 20));
        hboxLayout4 = new QHBoxLayout(layoutWidget1);
        hboxLayout4->setSpacing(6);
        hboxLayout4->setMargin(0);
        hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
        radioTop = new QRadioButton(layoutWidget1);
        radioTop->setObjectName(QString::fromUtf8("radioTop"));
        QSizePolicy sizePolicy4(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(radioTop->sizePolicy().hasHeightForWidth());
        radioTop->setSizePolicy(sizePolicy4);
        radioTop->setMinimumSize(QSize(80, 0));

        hboxLayout4->addWidget(radioTop);

        radioVCenter = new QRadioButton(layoutWidget1);
        radioVCenter->setObjectName(QString::fromUtf8("radioVCenter"));
        radioVCenter->setMinimumSize(QSize(80, 0));

        hboxLayout4->addWidget(radioVCenter);

        radioBottom = new QRadioButton(layoutWidget1);
        radioBottom->setObjectName(QString::fromUtf8("radioBottom"));
        radioBottom->setMinimumSize(QSize(80, 0));

        hboxLayout4->addWidget(radioBottom);


        vboxLayout->addWidget(groupBox);


        hboxLayout->addLayout(vboxLayout);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setSpacing(6);
        vboxLayout1->setMargin(0);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        okButton = new QPushButton(editptntextdlg);
        okButton->setObjectName(QString::fromUtf8("okButton"));

        vboxLayout1->addWidget(okButton);

        cancelButton = new QPushButton(editptntextdlg);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        vboxLayout1->addWidget(cancelButton);

        spacerItem1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout1->addItem(spacerItem1);


        hboxLayout->addLayout(vboxLayout1);

        QWidget::setTabOrder(editText, checkUseLabelFont);
        QWidget::setTabOrder(checkUseLabelFont, buttonFont);
        QWidget::setTabOrder(buttonFont, radioTop);
        QWidget::setTabOrder(radioTop, radioVCenter);
        QWidget::setTabOrder(radioVCenter, radioBottom);
        QWidget::setTabOrder(radioBottom, radioLeft);
        QWidget::setTabOrder(radioLeft, radioHCenter);
        QWidget::setTabOrder(radioHCenter, radioLeft);
        QWidget::setTabOrder(radioLeft, okButton);
        QWidget::setTabOrder(okButton, cancelButton);
        retranslateUi(editptntextdlg);
        QObject::connect(okButton, SIGNAL(clicked()), editptntextdlg, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), editptntextdlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(editptntextdlg);
    } // setupUi

    void retranslateUi(QDialog *editptntextdlg) {
        editptntextdlg->setWindowTitle(QApplication::translate("editptntextdlg", "Edit pattern text", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("editptntextdlg", "Text", 0, QApplication::UnicodeUTF8));
        checkUseLabelFont->setText(QApplication::translate("editptntextdlg", "Use Label font", 0, QApplication::UnicodeUTF8));
        buttonFont->setText(QApplication::translate("editptntextdlg", "Font...", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("editptntextdlg", "Alignment", 0, QApplication::UnicodeUTF8));
        radioLeft->setText(QApplication::translate("editptntextdlg", "LEFT", 0, QApplication::UnicodeUTF8));
        radioHCenter->setText(QApplication::translate("editptntextdlg", "H CENTER", 0, QApplication::UnicodeUTF8));
        radioRight->setText(QApplication::translate("editptntextdlg", "RIGHT", 0, QApplication::UnicodeUTF8));
        radioTop->setText(QApplication::translate("editptntextdlg", "TOP", 0, QApplication::UnicodeUTF8));
        radioVCenter->setText(QApplication::translate("editptntextdlg", "V CENTER", 0, QApplication::UnicodeUTF8));
        radioBottom->setText(QApplication::translate("editptntextdlg", "BOTTOM", 0, QApplication::UnicodeUTF8));
        okButton->setText(QApplication::translate("editptntextdlg", "OK", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("editptntextdlg", "Cancel", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(editptntextdlg);
    } // retranslateUi

};

namespace Ui
{
class editptntextdlg: public Ui_editptntextdlg {};
} // namespace Ui

#endif // EDITPTNTEXTDLG_H
