#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "initialdatadialog.h"

#include <iostream>
#include <string>
#include <sstream>

#include <QPixmap>
#include <QRandomGenerator>
#include <random>


//------------ таблица замен -------------
const unsigned char MainWindow::SB[16][8] =
{
  { 4, 14,  5,  7,  6,  4, 13,  1},
  {10, 11,  8, 13, 12, 11, 11, 15},
  { 9,  4,  1, 10,  7, 10,  4, 13},
  { 2, 12, 13,  1,  1,  0,  1,  0},
  {13,  6, 10,  0,  5,  7,  3,  5},
  { 8, 13,  3,  8, 15,  2, 15,  7},
  { 0, 15,  4,  9, 13,  1,  5, 10},
  {14, 10,  2, 15,  8, 13,  9,  4},
  { 6,  2, 14, 14,  4,  3,  0,  9},
  {11,  3, 15,  4, 10,  6, 10,  2},
  { 1,  8, 12,  6,  9,  8, 14,  3},
  {12,  1,  7, 12, 14,  5,  7, 14},
  { 7,  0,  6, 11,  0,  9,  6,  6},
  {15,  7,  0,  2,  3, 12,  8, 11},
  { 5,  5,  9,  5, 11, 15,  2,  8},
  { 3,  9, 11,  3,  2, 14, 12, 12}
};

const int MainWindow::keyOrder[64] =    // Порядок использования байт ключа
{
  0, 1, 2, 3, 4, 5, 6, 7,   // Зашифровывание, раунды 1-8
  0, 1, 2, 3, 4, 5, 6, 7,   // --"--, раунды 9-16
  0, 1, 2, 3, 4, 5, 6, 7,   // --"--, раунды 17-24
  7, 6, 5, 4, 3, 2, 1, 0,   // --"--, раунды 25-32
  0, 1, 2, 3, 4, 5, 6, 7,   // Расшифровывание, раунды 1-8
  7, 6, 5, 4, 3, 2, 1, 0,   // --"--, раунды 9-16
  7, 6, 5, 4, 3, 2, 1, 0,   // --"--, раунды 17-24
  7, 6, 5, 4, 3, 2, 1, 0    // --"--, раунды 25-32
};


//---------------------------------------------------------------------------
//---- Шестнадцатиричное представление
QString __fastcall MainWindow::ToHex(const void* D, int L)
{
  QString S = "";        // Начинаем с пустой строки
  unsigned char* d = (unsigned char*)D;
                            // Поле рассатриваем как массив беззнаковых байт
  for (int i=0; i < L; i++)   // Цикл по длине поля
  {
    if (i) S += ' ';        // Если не первый шаг, добавляем разделительный пробел
    //S += std::IntToHex(d[i], 2); // Добавляем очередной байт
    //S += QString::number(d[i], 16);
    S += QString("%1").arg(d[i], 2, 16, QChar('0'));
  }
  return S;   // Возвращаем полученную строку
}

//---------------------------------------------------------------------------
//---- Функция преобразования ГОСТ
unsigned int __fastcall MainWindow::F (unsigned char key, unsigned int in)
{
  // Прибавление ключа
  unsigned int M1 = in + key;
  ui->SEditM1->setText(ToHex(&M1, 4));
  //Pause();

  // Выполнение замен по таблице
  unsigned int M2 = 0;
  for(int i=7; i>=0; i--)
  {
     M2 = (M2<<4) | SB[ (M1>>(i*4)) & 0xF ][i];
  }
  ui->SEditM2->setText(ToHex(&M2, 4));
  //Pause();

  // Циклический сдвиг
  unsigned int M3 = (M2<<11) | (M2>>(32-11));
  ui->SEditM3->setText(ToHex(&M3, 4));
  //Pause();

  return M3;
}

// скрытие дочерних элементов
void MainWindow::setLayoutItemVisible(QLayoutItem *item, bool visibility)
{
    QLayout* layout = item->layout();

    if( layout ) {
        int itemsCount = layout->count();
        for ( int i = 0; i < itemsCount; ++i ) {
            setLayoutItemVisible(layout->itemAt(i), visibility);
        }
    } else {
        QWidget* itemWidget = item->widget();
        if(itemWidget) {
            visibility ? itemWidget->show() : itemWidget->hide();
        }
    }
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    roundCount = 32;

    // таймер
    timer = new QTimer();
    timer->setInterval(1500);
    connect(timer, SIGNAL(timeout()), this, SLOT(slot_timer_callback()));

    ui->randomBtn->click();
    //timer->start(); // И запустим таймер
}

MainWindow::~MainWindow()
{
    delete ui;
}

// событие таймера
void MainWindow::slot_timer_callback(){
    timer->stop();
    //bool cont = true;

    int roundNo = stepNo / 5;
    int operNo = stepNo % 5 + 1;
    int rndNo = roundNo % roundCount;
    bool encrypt = roundNo < roundCount;
    int keyNo = keyOrder[roundNo];
    bool lastRnd = (rndNo == roundCount - 1);
    bool lastOp = (lastRnd && (operNo == 5));

    QString action = encrypt ? "Зашифровывание" : "Расшифровывание";
    ui->mainTitle->setText(action + ", раунд " + QString::number(rndNo+1) + " из " + QString::number(roundCount));

    int w = ui->image->width();
    int h = ui->image->height();

    QPixmap pix1(":/resources/img/GOST-1.bmp");
    QPixmap pix2(":/resources/img/GOST-2.bmp");

    if (lastRnd)
    {
        ui->image->setPixmap(pix2.scaled(w, h, Qt::KeepAspectRatio));
        ui->SEditL3->setVisible(false);
        ui->SEditR3->setVisible(false);
    }
    else
    {
        ui->image->setPixmap(pix1.scaled(w, h, Qt::KeepAspectRatio));
        ui->SEditL3->setVisible(true);
        ui->SEditR3->setVisible(true);
    }

    if (lastOp) // Последняя операция раунда выполняется отдельно
    {
        if (encrypt) // При зашифровывании покажем шифротекст
        {
            Sh[0] = L;
            Sh[1] = R;
            ui->EditSh->setText(ToHex(Sh, 8));
        }
        else // При расшифровывании - конец процесса, стоп таймера
        {
            ui->pauseBtn->setEnabled(false);
            //cont = false;
        }
    }
    else // Если не последняя операция раунда, покажем ее контролы на экране
    {
        for (int i = 0; i < ui->steps_layout->count(); ++i) {
            QLayoutItem *item = ui->steps_layout->itemAt(i);

            QLayout *layout = item->layout();
            if(layout != NULL){
                QString nameObj = layout->objectName();
                QStringList nameParse = nameObj.split('_');
                setLayoutItemVisible(item, nameParse.last().toInt() <= operNo);
            }else{
                QWidget *w = item->widget();
                if(w != NULL){
                    QString nameObj = w->objectName();
                    QStringList nameParse = nameObj.split('_');
                    setLayoutItemVisible(item, nameParse.last().toInt() <= operNo);
                }
            }
        }
    }

    // Собственно процесс шифрования (по операциям одного раунда)
    switch (operNo)
    {
        case 1: // Показываем полублоки
            ui->EditL1->setText(ToHex(&L, 4));
            ui->EditR1->setText(ToHex(&R, 4));
            ui->SEditL1->setText(ToHex(&L, 4));
            ui->SEditR1->setText(ToHex(&R, 4));
            ui->SEditL2->setText("");
            ui->SEditR2->setText("");
            ui->SEditL3->setText("");
            ui->SEditR3->setText("");
            ui->SEditM1->setText("");
            ui->SEditM2->setText("");
            ui->SEditM3->setText("");
            ui->SEditK->setText("");
            break;
        case 2: // Выбираем и показываем нужный байт ключа
            K = Key[keyNo];
            ui->EditK->setText(ToHex(&K, 1));
            ui->SEditK->setText(ToHex(&K, 1));
            break;
        case 3: // Вычисляем и показываем маску
            ui->EditM->setText("");
            M = F(K, L);
            ui->EditM->setText(ToHex(&M, 4));
            ui->SEditM3->setText(ToHex(&M, 4));
            break;
        case 4: // Накладываем маску на правый полублок
            R ^= M;
            ui->EditL2->setText(ToHex(&L, 4));
            ui->EditR2->setText(ToHex(&R, 4));
            ui->SEditL2->setText(ToHex(&L, 4));
            ui->SEditR2->setText(ToHex(&R, 4));
            break;
        case 5: // Меняем полублоки местами, кроме последнего раунда
            if (! lastRnd)
            {
                unsigned int temp = L;
                L = R;
                R = temp;
                ui->EditL3->setText(ToHex(&L, 4));
                ui->EditR3->setText(ToHex(&R, 4));
                ui->SEditL3->setText(ToHex(&L, 4));
                ui->SEditR3->setText(ToHex(&R, 4));
            }
    }

    stepNo ++; // продвигаем номер шага
    timer->start(); // Перезапускаем таймер
}

// кнопка "Ввести"
void MainWindow::on_pushButton_clicked()
{    
    ui->pauseBtn->click();
    InitialDataDialog* dlg = new InitialDataDialog(this, Key, 8, Pl, 8);
    dlg->setModal(true);

    if (dlg->exec())
    {        
        ui->EditKey->setText(ToHex(Key, 8));
        ui->EditPl->setText(ToHex(Pl, 8));
        ui->EditSh->setText("");
        ui->atFirstBtn->click();
    }
}

// кнопка "Случайно"
void MainWindow::on_randomBtn_clicked()
{
    for (int i=0; i<8; i++) // Генерация случайного ключа
    {
        Key[i] = QRandomGenerator::global()->bounded(256);
    }
    ui->EditKey->setText(ToHex(Key, 8));
    for (int i=0; i<4; i++) // Генерация данных
    {
        L = (L << 8) + QRandomGenerator::global()->bounded(256);
        R = (R << 8) + QRandomGenerator::global()->bounded(256);
    }
    Pl[0] = L;
    Pl[1] = R;
    ui->EditPl->setText(ToHex(Pl, 8));
    ui->EditSh->setText("");
    ui->atFirstBtn->setEnabled(true);
    ui->atFirstBtn->click();
}

// кнопка "С начала"
void MainWindow::on_atFirstBtn_clicked()
{
    stepNo = 0;
    L = Pl[0];
    R = Pl[1];
    ui->EditSh->setText("");
    ui->startBtn->setEnabled(true);
    ui->startBtn->click();
}

// кнопка "Пауза"
void MainWindow::on_pauseBtn_clicked()
{
    timer->stop();
    ui->pauseBtn->setEnabled(false);
    ui->startBtn->setEnabled(true);
}

// кнопка "Пуск"
void MainWindow::on_startBtn_clicked()
{
    timer->start();
    ui->pauseBtn->setEnabled(true);
    ui->startBtn->setEnabled(false);
}

