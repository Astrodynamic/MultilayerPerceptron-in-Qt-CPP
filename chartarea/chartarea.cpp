#include "chartarea.h"

ChartArea::ChartArea(QWidget *parent) : QChartView{parent} {
  QPieSeries *m_series = new QPieSeries();
  for (unsigned i = 0; i < 26; ++i) {
    m_series->append(new QPieSlice(QChar(65 + i), 1, m_series));
    m_series->slices().at(i)->setLabelVisible();
  }
  m_chart = new QChart();
  m_chart->addSeries(m_series);
  m_chart->legend()->setVisible(false);
  m_chart->setTheme(QChart::ChartThemeLight);
  m_chart->setAnimationOptions(QChart::SeriesAnimations);
  chart()->deleteLater();
  setChart(m_chart);
  setRenderHint(QPainter::Antialiasing);
}

ChartArea::~ChartArea() { m_chart->removeAllSeries(); }

int ChartArea::GetResults(QVector<double> rate) {
  QPieSeries *series = (QPieSeries *)m_chart->series().at(0);
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

GraphArea::GraphArea(QWidget *parent) : QChartView{parent}, m_series{nullptr} {
  setMouseTracking(true);
  setRubberBand(QChartView::HorizontalRubberBand);
  m_series = new QLineSeries();
  QChart *m_chart = this->chart();
  m_chart->addSeries(m_series);
  m_chart->setTitle("Change netvork's error graph");
  m_chart->legend()->setVisible(false);
  m_chart->createDefaultAxes();
  m_chart->setTheme(QChart::ChartThemeLight);
  m_chart->setAnimationOptions(QChart::SeriesAnimations);
  setRenderHint(QPainter::Antialiasing);
  setMinimumHeight(200);
}

GraphArea::~GraphArea() { chart()->removeAllSeries(); }

void GraphArea::ClearCanvas() { m_series->clear(); }

void GraphArea::SetNewMistake(QList<QPointF> points) {
  chart()->axisX()->setMax(points.last().rx() + 1);
  m_series->append(points);
}
