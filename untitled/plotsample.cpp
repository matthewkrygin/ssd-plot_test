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
       info_.clear();

       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList pt; // = in.readLine();
          QRegExp rx = QRegExp("\\s+");
          if (line.size() < 1) continue;
          if (line[0] == '#') {
              info_ << line.mid(2, line.size() - 2);
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

    if (data_x_.size() < 1)
        throw std::runtime_error("No measurements\' data.");

    double xscale = xmax - xmin;
    if (xscale <= std::numeric_limits<double>::min()) {
        xmin -=1;
        xscale = 2;
    }

    double yscale = ymax - ymin;
    if (yscale <= std::numeric_limits<double>::min()) {
        ymin -=1;
        yscale = 2;
    }

    // norm points right after getting
    for (int idx = 0; idx < data_x_.size(); idx++) {
        data_x_[idx] -= xmin;
        data_x_[idx] /= xscale;
        data_y_[idx] -= ymin;
        data_y_[idx] /= yscale;
    }
    origin = {xmin, ymin};
    scale  = {xscale, yscale};
    this->update();
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

    QPointF p0, p1;
    if (!data_x_.size()) return;
    p0 = QPointF{data_x_[0] * w, data_y_[0] * h};
    paint.drawPoint(p0);
    for (int idx = 1; idx < data_x_.size(); idx++) {
        p1 = QPointF {data_x_[idx] * w, data_y_[idx] * h};
        paint.drawLine(p0, p1);
        paint.drawPoint(p1);
        p0 = p1;
    }

    paint.end();
    if (withGrid_)   addGrid();
    if (withLabels_) addLabels();
    this->show();
}

PlotSample::~PlotSample()
{
    delete ui;
}
