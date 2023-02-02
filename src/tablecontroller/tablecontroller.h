#ifndef TABLECONTROLLER_H
#define TABLECONTROLLER_H

#include <QPushButton>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QtMath>

class TableController : public QTableWidget {
  Q_OBJECT
 public:
  explicit TableController(QWidget *parent = nullptr);
  void AddDefLayout();
  void AddLayout();
  void RemoveLayout();
  bool WasUpdated();

 private:
  void AddLayout(int);
  void RemoveLayout(int);
  int GetDiffVall(int);
 public slots:
  void SetLayouts(QVector<unsigned> &);
  void QuadChange(int);
  void OnRemoveBtnCliced();
  void SetLayots(int);
  void SendLayouts();
 signals:
  void SetCount(int);
  void LayoutsSended(QVector<unsigned>);
};

#endif  // TABLECONTROLLER_H
