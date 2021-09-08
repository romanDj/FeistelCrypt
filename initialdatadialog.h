#ifndef INITIALDATADIALOG_H
#define INITIALDATADIALOG_H

#include <QDialog>

namespace Ui {
class InitialDataDialog;
}

class InitialDataDialog : public QDialog
{
    Q_OBJECT

public:     
   explicit InitialDataDialog(QWidget *parent, void *D1, int L1, void *D2, int L2);
    ~InitialDataDialog();


private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_2_textChanged(const QString &arg1);    

private:
    Ui::InitialDataDialog *ui;
    QString __fastcall ToHex(const void* D, int L);
    bool __fastcall FromHex(const QString& S, void* D, int L);
    bool __fastcall CheckHex(const QString& S, int L);
    int Len1, Len2;
    void* Dk1;
    void* Dk2;
    bool Valid1, Valid2;
};

#endif // INITIALDATADIALOG_H
