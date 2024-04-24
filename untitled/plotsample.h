#ifndef PLOTSAMPLE_H
#define PLOTSAMPLE_H

#include <QWidget>
#include <QFile>
#include <QPainter>
#include <QLabel>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

namespace Ui {
class PlotSample;
}

class PlotSample : public QWidget
{
    Q_OBJECT

public:
    explicit PlotSample(QString filename, QWidget *parent = nullptr);
    void paintEvent(QPaintEvent* event);
    void update_data();
    void addGrid();
    void addLabels();

    void setLabelFlag(bool flag);
    void setGridFlag(bool flag) {
        withGrid_ = flag; update();};
    bool getLabelFlag() { return withLabels_; };
    bool getGridFlag() { return withGrid_; };

    QString getInfo();
    ~PlotSample();

private:
    Ui::PlotSample *ui;
    QString filename_;
    QStringList info_;
    bool withGrid_ = false;
    bool withLabels_ = false;
    int gridNumX_=10, gridNumY_=10;
    QVector<double> data_x_;
    QVector<double> data_y_;
    QPointF origin, scale;
};

#endif // PLOTSAMPLE_H
