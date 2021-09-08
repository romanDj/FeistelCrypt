#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string>

#include <QMainWindow>
#include <QString>
#include <QLineEdit>
#include <QTimer>
#include <QLayoutItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void slot_timer_callback();

    void on_randomBtn_clicked();

    void on_atFirstBtn_clicked();

    void on_pauseBtn_clicked();

    void on_startBtn_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    // ---- Константы
    static const unsigned char SB[16][8];   // Таблицы S-боксов
    static const int keyOrder[64];  // Порядок использования байт ключа
    // ---- Поля
    int stepNo; // Номер шага
    int roundCount; // Число раундов
    unsigned char Key[8];   // Данные ключа
    unsigned int Pl[2]; // Открытый текст
    unsigned int Sh[2]; // Зашифрованный текст
    unsigned int L, R;
    unsigned int M;
    unsigned char K;
    // ---- Методы
    QString __fastcall ToHex(const void* D, int L);
    unsigned int __fastcall F (unsigned char key, unsigned int in);
    void __fastcall Pause (unsigned int wait = 1000);
    void setLayoutItemVisible(QLayoutItem* item, bool visibility);
};
#endif // MAINWINDOW_H
