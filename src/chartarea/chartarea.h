#ifndef CHARTAREA_H
#define CHARTAREA_H

#include <QBarSet>
#include <QChartView>
#include <QLineSeries>
#include <QPieSeries>
#include <QPieSlice>

class ChartArea : public QChartView {
  Q_OBJECT
 public:
  explicit ChartArea(QWidget *parent = nullptr);
  ~ChartArea();
 public slots:
  int GetResults(QVector<double> rate);

 private:
  QChart *chart_;
};

class GraphArea : public QChartView {
  Q_OBJECT
 public:
  explicit GraphArea(QWidget *parent = nullptr);
  ~GraphArea();
 public slots:
  void ClearCanvas();
  void SetNewMistake(QList<QPointF>);

 private:
  QLineSeries *series_;
};

#endif  // CHARTAREA_H
