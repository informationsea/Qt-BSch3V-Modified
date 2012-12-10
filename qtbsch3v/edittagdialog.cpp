#include "edittagdialog.h"
#include "ui_edittagdialog.h"

#include "xbsch.h"
#include "xbschtag.h"

EditTagDialog::EditTagDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTagDialog)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
}

EditTagDialog::~EditTagDialog()
{
    delete ui;
}


void EditTagDialog::setText(const QString & text)
{
    ui->text->setText(text);
}

QString EditTagDialog::text ()
{
    return ui->text->text();
}

void EditTagDialog::setType(int n)
{
    ui->typeBI->setChecked(false);
    ui->typeLU->setChecked(false);
    ui->typeRD->setChecked(false);
    ui->typeSQ->setChecked(false);
    switch(n) {
    case SXBSchTag::TAG_LU:
        ui->typeLU->setChecked(true);
        break;
    case SXBSchTag::TAG_RD:
        ui->typeRD->setChecked(true);
        break;
    case SXBSchTag::TAG_BI:
        ui->typeBI->setChecked(true);
        break;
    case SXBSchTag::TAG_SQ:
    default:
        ui->typeSQ->setChecked(true);
        break;
    }
}

int EditTagDialog::type()
{
    if(ui->typeBI->isChecked()) {
        return SXBSchTag::TAG_BI;
    }
    if(ui->typeLU->isChecked()) {
        return SXBSchTag::TAG_LU;
    }
    if(ui->typeRD->isChecked()) {
        return SXBSchTag::TAG_RD;
    }
    if(ui->typeSQ->isChecked()) {
        return SXBSchTag::TAG_SQ;
    }
    Q_ASSERT(1);
    return SXBSchTag::TAG_SQ;
}
