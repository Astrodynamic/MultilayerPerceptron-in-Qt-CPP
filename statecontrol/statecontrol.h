#ifndef STATECONTROL_H
#define STATECONTROL_H

#include <QMessageBox>
#include <QObject>

#include "network.h"

enum ModelProcess : unsigned {
  kWaitCommand = 0,
  kIsUpdating,
  kIsLoadingTrainData,
  kIsLearning,
  kIsLoadingTestingData,
  kIsTesting,
  kIsThinking,
  kIsSavingWeights,
  kIsLoadingWeigts
};
enum ModelState : unsigned {
  kClearModel = 0,
  kIsUpdated,
  kIsLearned,
  kIsTested,
  sSize
};
enum DataLoaded : unsigned { kLearnigData = 0, kTestingData, dSize };

class StateControl : public QObject {
  Q_OBJECT
 public:
  explicit StateControl(Network &model, QObject *parent = nullptr);
  bool CheckProcess(ModelProcess);

 private:
  Network &m_model;
  ModelProcess m_cur_process;
  std::bitset<ModelState::sSize> m_model_state;
  std::bitset<DataLoaded::dSize> m_data;

  bool CheckState(ModelProcess);

 public slots:
  void SetItReady();

 signals:
  void InitUpdating();
  void InitLearning();
  void InitTesting();
  void InitLoadLearningDate();
  void InitLoadTestingDate();
};

#endif  // STATECONTROL_H
