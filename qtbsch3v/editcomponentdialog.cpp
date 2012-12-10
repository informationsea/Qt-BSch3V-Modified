#include "editcomponentdialog.h"
#include "ui_editcomponentdialog.h"

#include "complib.h"

EditComponentDialog::EditComponentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditComponentDialog)
{
    ui->setupUi(this);

    setWindowModality(Qt::WindowModal);
}

EditComponentDialog::~EditComponentDialog()
{
    delete ui;
}

void EditComponentDialog::setCompIndex(const SCompIndex* pIndex)
{
    m_componentIndex = pIndex;
    ui->block->setMaximum(pIndex->block());
    on_block_valueChanged(1);
}

const SCompIndex* EditComponentDialog::compIndex() const
{
    return m_componentIndex;
}

void EditComponentDialog::setNote(const QString& text)
{
    ui->note->setPlainText(text);
}

void EditComponentDialog::setName(const QString& text)
{
    ui->name->setText(text);
}

void EditComponentDialog::setReference(const QString& text)
{
    ui->reference->setText(text);
}

void EditComponentDialog::setOrgName(const QString& text)
{
    ui->original_name->setText(text);
}

QString EditComponentDialog::note() const
{
    return ui->note->document()->toPlainText();
}

QString EditComponentDialog::name() const
{
    return ui->name->text();
}

QString EditComponentDialog::reference() const
{
    return ui->reference->text();
}

QString EditComponentDialog::orgName() const
{
    return ui->original_name->text();
}

void EditComponentDialog::setBlock(int block)
{
    ui->block->setValue(block+1);
}

int EditComponentDialog::block() const
{
    return ui->block->value()-1;
}

void EditComponentDialog::setNameVertical(bool vert)
{
    ui->name_vertical->setChecked(vert);
}

void EditComponentDialog::setReferenceVertical(bool vert)
{
    ui->reference_vertical->setChecked(vert);
}

bool EditComponentDialog::nameVertical() const
{
    return ui->name_vertical->checkState();
}
bool EditComponentDialog::referenceVertical() const
{
    return ui->reference_vertical->checkState();
}

void EditComponentDialog::on_block_valueChanged(int block)
{
    if(m_componentIndex == NULL) return;
    block -= 1;  //表示は1からだが、内部的には0からなのでデクリメント
    QString str;
    int index = 0;
    int maxpin = m_componentIndex->pinCount();
    if(maxpin > 0) {
        while(1) {
            str += QString::fromLocal8Bit(m_componentIndex->pin(index)->pinNum(block));
            index++;
            if(index >= maxpin || str.length()>30) break;
            str += ',';
        }
    }
    ui->label_pinnum->setText(str);
}
