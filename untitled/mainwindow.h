#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include <QListWidgetItem>
#include <memory>
#include <unordered_map>
#include "plotsample.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void getFilename();
    ~MainWindow();

private slots:
    void on_actionExit_triggered();
    void on_actionOpen_triggered();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_3_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_clicked();

    void on_checkBox_2_toggled(bool checked);
    void on_checkBox_toggled(bool checked);

    void on_listWidget_currentRowChanged(int currentRow);

private:
    Ui::MainWindow *ui;
    QMap<QString, int> m_id;
    std::unordered_map<int, std::unique_ptr<PlotSample>> m_windows;
};
#endif // MAINWINDOW_H
