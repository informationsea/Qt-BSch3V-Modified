#ifndef EDITCOMPONENTDIALOG_H
#define EDITCOMPONENTDIALOG_H

#include <QDialog>

class SCompIndex;

namespace Ui
{
class EditComponentDialog;
}

class EditComponentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditComponentDialog(QWidget *parent = 0);
    ~EditComponentDialog();

    void setCompIndex(const SCompIndex* pIndex);
    const SCompIndex* compIndex() const;

    void setNote(const QString& text);
    void setName(const QString& text);
    void setReference(const QString& text);
    void setOrgName(const QString& text);
    QString note() const;
    QString name() const;
    QString reference() const;
    QString orgName() const;
    void setBlock(int block);
    int block() const;
    void setNameVertical(bool vert);
    void setReferenceVertical(bool vert);
    bool nameVertical() const;
    bool referenceVertical() const;

private slots:
    void on_block_valueChanged(int arg1);

private:
    Ui::EditComponentDialog *ui;

    const SCompIndex *m_componentIndex;
};

#endif // EDITCOMPONENTDIALOG_H
