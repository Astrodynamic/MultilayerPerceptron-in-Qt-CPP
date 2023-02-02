#include "timeceeper.h"

ProgressCeeper::ProgressCeeper(QObject* parent) : QObject(parent) {}

void ProgressCeeper::StartTimer() { time_stamp_ = QTime::currentTime(); }

QTime ProgressCeeper::EndTimer() {
  return QTime::fromMSecsSinceStartOfDay(
      time_stamp_.msecsTo(QTime::currentTime()));
}
