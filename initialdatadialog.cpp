#include "initialdatadialog.h"
#include "ui_initialdatadialog.h"
#include <QPushButton>
#include <QString>

InitialDataDialog::InitialDataDialog(QWidget *parent, void* D1, int L1, void* D2, int L2) :
    QDialog(parent),
    ui(new Ui::InitialDataDialog)
{
    ui->setupUi(this);

    Valid1 = true;
    Len1 = L1;
    Dk1 = D1;
    ui->lineEdit->setText(ToHex(D1, L1));

    Valid2 = true;
    Len2 = L2;
    Dk2 = D2;
    ui->lineEdit_2->setText(ToHex(D2, L2));

    // Ok callback
    QPushButton *resetButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    connect(resetButton, &QPushButton::clicked, [=](){
        FromHex(ui->lineEdit->text(), D1, L1);
        FromHex(ui->lineEdit_2->text(), D2, L2);
    });
}

InitialDataDialog::~InitialDataDialog()
{
    delete ui;
}


//---------------------------------------------------------------------------
//---- Шестнадцатиричное представление
QString __fastcall InitialDataDialog::ToHex(const void* D, int L)
{
    QString S = ""; // Начинаем с пустой строки
    unsigned char* d = (unsigned char*)D;   // Поле рассатриваем как массив беззнаковых байт

    for (int i=0; i<L; i++) // Цикл по длине поля
    {
        if (i) S += ' ';        // Если не первый шаг, добавляем разделительный пробел
        S += QString("%1").arg(d[i], 2, 16, QChar('0')); // Добавляем очередной байт
    }
    return S;   // Возвращаем полученную строку
}

//---------------------------------------------------------------------------
//---- Из шестнадцатиричного представления
bool __fastcall InitialDataDialog::FromHex(const QString& S, void* D, int L)
{
    if (!CheckHex(S,L))
    {
        return false;
    }
    int k = 0;
    unsigned char* d = (unsigned char*)D;

    for (int i=0; i < L; i++)
    {
        if(S.length() <= k){
            break;
        }
        QString T = "0x";
        for (int j=0; j < 2; j++)
        {
            if(S.length() <= k){
                break;
            }
            while (S[k] == ' ')
            {
                k++;
            }
            T += S[k];
            k++;
        }
        bool ok;
        d[i] = T.toInt(&ok, 16);
    }
    return true;
}

//---------------------------------------------------------------------------
//---- Проверка шестнадцатиричного представления
bool __fastcall InitialDataDialog::CheckHex(const QString& S, int L)
{
    try {
        int N = S.length();
        int K = 0;

        if(N != 2*L + (L - 1)){
            return false;
        }

        QString Hex = " 0123456789aAbBcCdDeEfF";

        for (int i=0; i < N; i++)
        {
            QString fStr = S.sliced(i,1);
            int p = Hex.indexOf(fStr, 0, Qt::CaseInsensitive);

            if (p < 0)
            {
                return false;
            }
            else if (p >= 0)
            {                
                K ++;
            }
        }
        return (K == 2*L + (L - 1));
    }  catch(...)  {
        return false;
    }
}


void InitialDataDialog::on_lineEdit_textChanged(const QString &arg1)
{
    Valid1 = CheckHex(ui->lineEdit->text(), Len1);

    QPalette palette;
    palette.setColor(QPalette::Base,Qt::white);
    QPalette palette1;
    palette1.setColor(QPalette::Base,Qt::yellow);
    ui->lineEdit->setPalette(Valid1 ? palette : palette1);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(Valid1 && Valid2);
}


void InitialDataDialog::on_lineEdit_2_textChanged(const QString &arg1)
{
    Valid2 = CheckHex(ui->lineEdit_2->text(), Len2);

    QPalette palette;
    palette.setColor(QPalette::Base, Qt::white);
    QPalette palette1;
    palette1.setColor(QPalette::Base, Qt::yellow);
    ui->lineEdit_2->setPalette(Valid2 ? palette : palette1);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(Valid1 && Valid2);
}



