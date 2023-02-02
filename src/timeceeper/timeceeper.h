#ifndef TIMECEEPER_H
#define TIMECEEPER_H

#include <QObject>
#include <QTime>

class ProgressCeeper : QObject {
  Q_OBJECT
 public:
  ProgressCeeper(QObject* parent = nullptr);
  void StartTimer();
  QTime EndTimer();

 signals:
  void LearnProgress(int);
  void TestProgress(int);

 private:
  QTime time_stamp_;
  int progress_;
};

#endif  // TIMECEEPER_H
