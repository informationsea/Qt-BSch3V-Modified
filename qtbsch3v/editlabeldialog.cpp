#include "editlabeldialog.h"
#include "ui_editlabeldialog.h"

EditLabelDialog::EditLabelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditLabelDialog)
{
    ui->setupUi(this);

    setWindowModality(Qt::WindowModal);
}

EditLabelDialog::~EditLabelDialog()
{
    delete ui;
}


void EditLabelDialog::setText(const QString & text)
{
    ui->labelText->setText(text);
}

QString EditLabelDialog::text()
{
    return ui->labelText->text();
}
