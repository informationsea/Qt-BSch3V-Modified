#ifndef EDITTAGDIALOG_H
#define EDITTAGDIALOG_H

#include <QDialog>

namespace Ui
{
class EditTagDialog;
}

class EditTagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditTagDialog(QWidget *parent = 0);
    ~EditTagDialog();

    void setText(const QString & text);
    QString text ();
    void setType(int n);
    int type();

private:
    Ui::EditTagDialog *ui;
};

#endif // EDITTAGDIALOG_H
