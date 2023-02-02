#include "chartarea.h"

ChartArea::ChartArea(QWidget *parent) : QChartView{parent} {
  QPieSeries *series_ = new QPieSeries();
  for (unsigned i = 0; i < 26; ++i) {
    series_->append(new QPieSlice(QChar(65 + i), 1, series_));
    series_->slices().at(i)->setLabelVisible();
  }
  chart_ = new QChart();
  chart_->addSeries(series_);
  chart_->legend()->setVisible(false);
  chart_->setTheme(QChart::ChartThemeLight);
  chart_->setAnimationOptions(QChart::SeriesAnimations);
  chart()->deleteLater();
  setChart(chart_);
  setRenderHint(QPainter::Antialiasing);
}

ChartArea::~ChartArea() { chart_->removeAllSeries(); }

int ChartArea::GetResults(QVector<double> rate) {
  QPieSeries *series = (QPieSeries *)chart_->series().at(0);
  for (int i = 0; i < rate.size(); ++i) {
    series->slices().at(i)->setValue(rate.at(i));
    if (series->slices().at(i)->percentage() <= 0.01) {
      series->slices().at(i)->setLabelVisible(false);
    } else {
      series->slices().at(i)->setLabelVisible(true);
    }
  }
  return 1;
}

GraphArea::GraphArea(QWidget *parent) : QChartView{parent}, series_{nullptr} {
  setMouseTracking(true);
  setRubberBand(QChartView::HorizontalRubberBand);
  series_ = new QLineSeries();
  QChart *chart_ = this->chart();
  chart_->addSeries(series_);
  chart_->setTitle("Change netvork's error graph");
  chart_->legend()->setVisible(false);
  chart_->createDefaultAxes();
  chart_->setTheme(QChart::ChartThemeLight);
  chart_->setAnimationOptions(QChart::SeriesAnimations);
  setRenderHint(QPainter::Antialiasing);
  setMinimumHeight(200);
}

GraphArea::~GraphArea() { chart()->removeAllSeries(); }

void GraphArea::ClearCanvas() { series_->clear(); }

void GraphArea::SetNewMistake(QList<QPointF> points) {
  chart()->axisX()->setMax(points.last().rx() + 1);
  series_->append(points);
}
