#include "plotsample.h"
#include "ui_plotsample.h"
#include <QLabel>


void PlotSample::addGrid()
{
    QPainter paint(this);
    paint.setPen(QColor(0,0,0,255));

    for (int idx = 0; idx < gridNumX_; idx++) {
        int x0 = static_cast<int>(static_cast<float>(width())*
                (((float)idx + 1.f) / ((float)gridNumX_ + 1.f)));
        paint.drawLine(x0,0,x0,height());
    }
    for (int idx = 0; idx < gridNumY_; idx++) {
        int y0 = static_cast<int>(static_cast<float>(height())*
                (((float)idx + 1.f) / ((float)gridNumY_ + 1.f)));

        paint.drawLine(0,y0,width(), y0);
    }
    paint.end();
}

void PlotSample::addLabels()
{
    QPainter paint(this);
    paint.setPen(QColor(0,0,0,255));

    for (int idx = 0; idx <= gridNumX_; idx++) {
        for (int jdx = 1; jdx <= gridNumY_; jdx++) {
            //add text
            double val_x =
                    (((double)idx) / ((double)gridNumX_ + 1.));
            double val_y =
                    (((double)jdx) / ((double)gridNumY_ + 1.));
            QPointF pt(static_cast<double>(width())*val_x,
                       static_cast<double>(height())*val_y);

            val_x *= scale.x();
            val_y *= scale.y();
            val_x += origin.x();
            val_y += origin.y();

            paint.drawText(pt, QString::number(val_x) + ";" + QString::number(val_y));
        }
    }

    paint.end();

}

void PlotSample::update_data()
{
    QFile inputFile(filename_);
    double xmax = std::numeric_limits<double>::lowest(),
           ymax = std::numeric_limits<double>::lowest();
    double xmin = std::numeric_limits<double>::max(),
           ymin = std::numeric_limits<double>::max();
    setWindowTitle(filename_);

    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       data_x_.clear();
       data_y_.clear();

       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList pt; // = in.readLine();
          QRegExp rx = QRegExp("\\s+");
          if (line.size() < 1) continue;
          if (line[0] == '#') {
              info_ << line;
          } else {
              pt = line.split(rx, QString::SkipEmptyParts);
              if (pt.size() != 2) throw
                  std::runtime_error("Data: Incorrect number of args;");
              bool ok;
              double x = pt[0].toDouble(&ok);
              if (!ok) throw
                  std::runtime_error("Data: incorrect 1st arg fp representation;");
              double y = pt[1].toDouble(&ok);
              if (!ok) throw
                  std::runtime_error("Data: incorrect 1st arg fp representation;");
              xmax = std::max(x, xmax);
              ymax = std::max(y, ymax);
              xmin = std::min(x, xmin);
              ymin = std::min(y, ymin);
              data_x_.push_back(x);
              data_y_.push_back(y);
          }
       }
       inputFile.close();
    } else throw std::runtime_error("File wasn't open");

    double xscale = xmax - xmin;
    double yscale = ymax - ymin;
    // norm points right after getting
    for (int idx = 0; idx < data_x_.size(); idx++) {
        data_x_[idx] -= xmin;
        data_x_[idx] /= xscale;
        data_y_[idx] -= ymin;
        data_y_[idx] /= yscale;
    }
    origin = {xmin, ymin};
    scale  = {xscale, yscale};

}

QString PlotSample::getInfo() {
    return info_.join("\n");
}

void PlotSample::setLabelFlag(bool flag) {
    withLabels_ = flag;
    resize(1920, 600);
    update();
}

PlotSample::PlotSample(QString filename, QWidget *parent) :
    filename_(filename), QWidget(parent),
    ui(new Ui::PlotSample)
{
    ui->setupUi(this);
    update_data();
    this->show();
}

void PlotSample::paintEvent(QPaintEvent* event)
{
    QPainter paint(this);
    paint.setPen(QColor(255,34,255,255));

    double h = height();
    double w = width();

    for (int idx = 0; idx < data_x_.size(); idx++) {
        QPointF tmp {data_x_[idx] * w, data_y_[idx] * h};
        paint.drawPoint(tmp);
        if (idx <data_x_.size()-1) {
            double x1 = data_x_[idx+1]*w, y1 = data_y_[idx+1]*h;
            paint.drawLine(tmp, {x1, y1});
        }
    }

    paint.end();
    if (withGrid_) addGrid();
    if (withLabels_) {
        addLabels();
    }
    this->show();
}

PlotSample::~PlotSample()
{
    delete ui;
}
