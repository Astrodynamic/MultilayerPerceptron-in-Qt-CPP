#include "statecontrol.h"

StateControl::StateControl(Network &model, QObject *parent)
    : QObject{parent}, model_{model}, cur_process_{kWaitCommand} {}

bool StateControl::CheckProcess(ModelProcess command_new_proc) {
  QMessageBox dialog;
  QString name_process;
  if (cur_process_ != kWaitCommand) {
    if (cur_process_ != kIsLearning && cur_process_ != kIsTesting) {
      if (cur_process_ != kIsLoadingTestingData &&
          cur_process_ != kIsLoadingTrainData) {
        switch (cur_process_) {
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
      switch (cur_process_) {
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
        model_.thread()->wait(10);
        model_.BreakProcess();
        if (command_new_proc == cur_process_) {
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
      if (!data_[kLearnigData]) {
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
      if (!model_state_[kIsLearned]) {
        dialog.setText("MLP is not learned.");
        dialog.setInformativeText("Do you want to teach it?");
        if (dialog.exec() == QMessageBox::Yes) {
          emit InitLearning();
        }
        return false;
      } else {
        if (!data_[kTestingData]) {
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
      if (!model_state_[kIsLearned]) {
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
      if (!model_state_[kIsUpdated]) {
        cur_process_ = kIsUpdating;
        emit InitUpdating();
        emit InitLoadLearningDate();
        return false;
      }
      break;
    }
    default:
      break;
  }
  cur_process_ = command_new_proc;
  return true;
}

void StateControl::SetItReady() {
  switch (cur_process_) {
    case kIsUpdating: {
      model_state_.set(kIsUpdated);
      break;
    }
    case kIsLearning: {
      model_state_.set(kIsLearned);
      model_state_.reset(kIsTested);
      break;
    }
    case kIsTesting: {
      model_state_.set(kIsTested);
      break;
    }
    case kIsLoadingWeigts: {
      model_state_.set(kIsUpdated);
      model_state_.set(kIsLearned);
      model_state_.set(kIsTested);
      break;
    }
    case kIsLoadingTrainData: {
      data_.set(kLearnigData);
      break;
    }
    case kIsLoadingTestingData: {
      data_.set(kTestingData);
      break;
    }
    default:
      break;
  }
  cur_process_ = kWaitCommand;
}
