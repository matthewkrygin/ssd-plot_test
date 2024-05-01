#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "plotsample.h"

#include <QPainter>
#include <QLabel>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

#ifdef Q_OS_WIN //this is Windows specific code, not portable
    QWindowsWindowFunctions::setWindowActivationBehavior(QWindowsWindowFunctions::AlwaysActivateWindow);
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionOpen_triggered()
{

    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open"), "./", tr("Data files (*.ssd)"));
    try {
        auto iter = m_id.find(filename);
        if (iter != m_id.end())
            throw std::runtime_error("File with that name has already opened; try refresh instead.");

        std::unique_ptr<PlotSample> tmp = std::unique_ptr<PlotSample>(new PlotSample(filename));
        int id_window = m_windows.size();
        m_id.insert(m_id.end(), filename, id_window);
        m_windows.emplace_hint(m_windows.end(), id_window, std::move(tmp));
        QListWidget newItem;
        this->ui->listWidget->insertItem(
                    m_windows.size() - 1, filename);
    } catch(std::exception& msg) {
        QMessageBox::critical(this,"Error", msg.what());
    }
}


void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    MainWindow::on_pushButton_5_clicked();
    MainWindow::on_pushButton_3_clicked();
}


void MainWindow::on_pushButton_3_clicked()
{
    const auto& items = ui->listWidget->selectedItems();

    if (items.size() < 1) {
        QMessageBox::warning(this, "No selection!", "0 files were chosen.");
        return;
    } else if (items.size() == 1) {
        QString name = items.first()->text();

        m_windows[m_id[name]]->show();
        m_windows[m_id[name]]->raise();
        m_windows[m_id[name]]->activateWindow();
        return;
    }
    QMessageBox::warning(this, "Incorrect selection!", "Too many files were chosen.");

}


void MainWindow::on_pushButton_5_clicked()
{
    const auto& items = ui->listWidget->selectedItems();
    if (items.size() < 1) {
        QMessageBox::warning(this, "No selection!", "0 files were chosen.");
        return;
    } else if (items.size() == 1) {
        QString name = items.first()->text();
        QMessageBox::information(this, "Information about:" + name, m_windows[m_id[name]]->getInfo());
        return;
    }
    QMessageBox::warning(this, "Incorrect selection!", "Too many files were chosen.");
}


void MainWindow::on_pushButton_4_clicked()
{
    const auto& items = ui->listWidget->selectedItems();
    if (items.size() < 1) {
        QMessageBox::warning(this, "No selection!", "0 files were chosen.");
        return;
    }
    for (auto& item: items ) {
        QString name = item->text();
        try {
            m_windows[m_id[name]]->update_data();
            QMessageBox::information(this, "Updated:" + name, m_windows[m_id[name]]->getInfo());
        } catch (std::exception& msg) {
            QMessageBox::critical(this,"Error", msg.what());
        }
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    const auto& items = ui->listWidget->selectedItems();
    if (items.size() < 1) {
        QMessageBox::warning(this, "No selection!", "0 files were chosen.");
        return;
    }
    for (auto& item: items ) {
        QString name = item->text();
        int flag = QMessageBox::question(this, "Closing file", "Do you want to close file:\n" + name, QMessageBox::Yes, QMessageBox::No);
        if (flag == QMessageBox::Yes)
        {
            ui->listWidget->takeItem(ui->listWidget->row(item));
            auto it_id = m_id.find(name);
            m_windows[it_id.value()].reset();
            m_id.erase(it_id);
            QMessageBox::information(this, "Closed", name);
        }
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    MainWindow::on_actionOpen_triggered();
}

void MainWindow::on_pushButton_clicked()
{
    MainWindow::on_actionExit_triggered();
}

void MainWindow::on_checkBox_2_toggled(bool checked)
{
    const auto& items = ui->listWidget->selectedItems();
    if (items.size() < 1) {
        ui->checkBox_2->setChecked(false);
        return;
    }
    for (auto& item: items ) {
        QString name = item->text();
        m_windows[m_id[name]]->setGridFlag(checked);
    }
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    const auto& items = ui->listWidget->selectedItems();
    if (items.size() < 1) {
        ui->checkBox->setChecked(false);
        return;
    }
    for (auto& item: items ) {
        QString name = item->text();
        m_windows[m_id[name]]->setLabelFlag(checked);
    }
}

void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    const auto& items = ui->listWidget->selectedItems();
    for (auto& val: items ) {
        QString name = val->text();
        ui->checkBox->setChecked(m_windows[m_id[name]]->getLabelFlag());
        ui->checkBox_2->setChecked(m_windows[m_id[name]]->getGridFlag());
    }
}
