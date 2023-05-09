#include "timeceeper.h"

ProgressCeeper::ProgressCeeper(QObject* parent) : QObject(parent) {}

void ProgressCeeper::StartTimer() { m_time_stamp = QTime::currentTime(); }

QTime ProgressCeeper::EndTimer() {
  return QTime::fromMSecsSinceStartOfDay(
      m_time_stamp.msecsTo(QTime::currentTime()));
}
