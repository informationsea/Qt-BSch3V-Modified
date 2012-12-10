#ifndef EDITLABELDIALOG_H
#define EDITLABELDIALOG_H

#include <QDialog>

namespace Ui
{
class EditLabelDialog;
}

class EditLabelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditLabelDialog(QWidget *parent = 0);
    ~EditLabelDialog();

    void setText(const QString & text);
    QString text ();

private:
    Ui::EditLabelDialog *ui;
};

#endif // EDITLABELDIALOG_H
