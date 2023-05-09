#ifndef MLP_SRC_CONTROLLER_H_
#define MLP_SRC_CONTROLLER_H_

#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QProgressBar>
#include <QStandardItemModel>
#include <QTableWidgetItem>

#include "chartarea.h"
#include "drowarea.h"
#include "network.h"
#include "statecontrol.h"
#include "tablecontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class View;
}
QT_END_NAMESPACE

class Controller : public QMainWindow {
  Q_OBJECT

 private:
  static Controller *instance;
  Ui::View *ui;
  Network m_model;
  QThread *thr = nullptr;
  QThread *thr_state = nullptr;
  StateControl m_model_state;

 public:
  static Controller *create(QWidget *parent = nullptr);

  explicit Controller(const Controller &other) = delete;
  explicit Controller(Controller &&other) noexcept = delete;

  Controller &operator=(const Controller &other) = delete;
  Controller &operator=(Controller &&other) noexcept = delete;

  void destroy();

 private slots:
  void GetPicture();
  void on_actionLoad_testing_data_triggered();
  void on_actionLoad_learning_data_triggered();
  void on_teach_btn_clicked();
  void on_test_btn_clicked();

  void ShowTestData(QVector<double>);
  void SetLearningTime(QTime);
  void SetTestingTime(QTime);
  void on_actionLoad_MLP_triggered();
  void on_actionSave_MLP_triggered();
  void on_learn_pbr_valueChanged(int value);
  void on_test_pbr_valueChanged(int value);

  void on_crossvalid_cbx_stateChanged(int arg1);

 signals:
  void StartLearning();
  void StartTesting();
  void SendImg(QPixmap);
  void SavedMLP(QString);

 private:
  void SetDrowArea();
  void MakeThread();
  void ConnectInfoSlots();
  void ConnectControlSlots();
  void ConnectDataSlots();
  void ConnectStateSlots();
  void ConnectThread();
  void SetChartArea();
  void SetTableController();

  explicit Controller(QWidget *parent = nullptr);
  virtual ~Controller();
};
#endif  // MLP_SRC_CONTROLLER_H_
