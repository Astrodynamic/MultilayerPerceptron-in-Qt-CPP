#include "statecontrol.h"

StateControl::StateControl(Network &model, QObject *parent)
    : QObject{parent}, m_model{model}, m_cur_process{kWaitCommand} {}

bool StateControl::CheckProcess(ModelProcess command_new_proc) {
  QMessageBox dialog;
  QString name_process;
  if (m_cur_process != kWaitCommand) {
    if (m_cur_process != kIsLearning && m_cur_process != kIsTesting) {
      if (m_cur_process != kIsLoadingTestingData &&
          m_cur_process != kIsLoadingTrainData) {
        switch (m_cur_process) {
          case kIsUpdating:
            name_process = "updating";
            break;
          case kIsThinking:
            name_process = "thinking";
            break;
          case kIsSavingWeights:
            name_process = "saving";
            break;
          case kIsLoadingWeigts:
            name_process = "loading";
            break;
          default:
            break;
        }
        dialog.setText("MLP is " + name_process +
                       " now. Please wait and try later.");
        dialog.exec();
      }
      return false;
    } else {
      switch (m_cur_process) {
        case kIsLearning:
          name_process = "Learning";
          break;
        case kIsTesting:
          name_process = "Testing";
          break;
        default:
          break;
      }
      dialog.setText(name_process + " MLP is running.");
      dialog.setInformativeText("Do you want to breake it?");
      dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      if (dialog.exec() == QMessageBox::Yes) {
        m_model.thread()->wait(10);
        m_model.BreakProcess();
        if (command_new_proc == m_cur_process) {
          return false;
        }
      } else {
        return false;
      }
    }
  }
  return CheckState(command_new_proc);
}

bool StateControl::CheckState(ModelProcess command_new_proc) {
  QMessageBox dialog;
  dialog.setInformativeText("Do you want to load it?");
  dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  switch (command_new_proc) {
    case kIsLearning: {
      if (!m_data[kLearnigData]) {
        dialog.setText("Learning data in not loaded.");
        if (dialog.exec() == QMessageBox::Yes) {
          emit InitLoadLearningDate();
          emit InitLearning();
        }
        return false;
      }
      break;
    }
    case kIsTesting: {
      if (!m_model_state[kIsLearned]) {
        dialog.setText("MLP is not learned.");
        dialog.setInformativeText("Do you want to teach it?");
        if (dialog.exec() == QMessageBox::Yes) {
          emit InitLearning();
        }
        return false;
      } else {
        if (!m_data[kTestingData]) {
          dialog.setText("Testing data in not loaded.");
          if (dialog.exec() == QMessageBox::Yes) {
            emit InitLoadTestingDate();
            emit InitTesting();
          }
          return false;
        }
      }
      break;
    }
    case kIsThinking: {
      if (!m_model_state[kIsLearned]) {
        dialog.setText("MLP is not learned.");
        dialog.setInformativeText("Do you want to teach it?");
        if (dialog.exec() == QMessageBox::Yes) {
          emit InitLearning();
        }
        return false;
      }
      break;
    }
    case kIsLoadingTrainData: {
      if (!m_model_state[kIsUpdated]) {
        m_cur_process = kIsUpdating;
        emit InitUpdating();
        emit InitLoadLearningDate();
        return false;
      }
      break;
    }
    default:
      break;
  }
  m_cur_process = command_new_proc;
  return true;
}

void StateControl::SetItReady() {
  switch (m_cur_process) {
    case kIsUpdating: {
      m_model_state.set(kIsUpdated);
      break;
    }
    case kIsLearning: {
      m_model_state.set(kIsLearned);
      m_model_state.reset(kIsTested);
      break;
    }
    case kIsTesting: {
      m_model_state.set(kIsTested);
      break;
    }
    case kIsLoadingWeigts: {
      m_model_state.set(kIsUpdated);
      m_model_state.set(kIsLearned);
      m_model_state.set(kIsTested);
      break;
    }
    case kIsLoadingTrainData: {
      m_data.set(kLearnigData);
      break;
    }
    case kIsLoadingTestingData: {
      m_data.set(kTestingData);
      break;
    }
    default:
      break;
  }
  m_cur_process = kWaitCommand;
}
