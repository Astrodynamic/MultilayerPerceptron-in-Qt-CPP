#include "controller.h"

#include <iostream>

#include "./ui_view.h"

Controller *Controller::instance = nullptr;

Controller *Controller::create(QWidget *parent) {
  if (instance == nullptr) {
    instance = new Controller(parent);
  }
  return instance;
}

void Controller::destroy() { delete this; }

Controller::Controller(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::View),
      m_model(this),
      m_model_state(m_model, this) {
  ui->setupUi(this);
  SetDrowArea();
  MakeThread();
  SetChartArea();
  SetTableController();
  ui->settings_dw->hide();
  ui->statistic_dw->hide();
  ui->wgt_main->update();
}

Controller::~Controller() {
  delete ui;
  thr->quit();
  thr->deleteLater();
  thr_state->quit();
  thr_state->deleteLater();
}

void Controller::GetPicture() {
  if (m_model_state.CheckProcess(kIsThinking)) {
    emit SendImg(ui->graphicsView->grab());
  }
}

void Controller::SetDrowArea() {
  DrowArea *canvas = new DrowArea(this);
  connect(ui->brh_size_spx, SIGNAL(valueChanged(int)), canvas,
          SLOT(SetBrushSize(int)));
  connect(ui->brh_type_cbx, SIGNAL(currentIndexChanged(int)), canvas,
          SLOT(SetBrushType(int)));
  connect(ui->actionLoad_image_to_scan, SIGNAL(triggered()), canvas,
          SLOT(SetImage()));
  connect(canvas, SIGNAL(updatePicture()), this, SLOT(GetPicture()));
  ui->graphicsView->setScene(canvas);
}

void Controller::MakeThread() {
  ConnectInfoSlots();
  ConnectControlSlots();
  ConnectDataSlots();
  ConnectStateSlots();
  ConnectThread();
}

void Controller::ConnectInfoSlots() {
  connect(&m_model, SIGNAL(NetworkLearningStage(int)), ui->learn_pbr,
          SLOT(setValue(int)));
  connect(&m_model, SIGNAL(NetworkTestingStage(int)), ui->test_pbr,
          SLOT(setValue(int)));

  connect(&m_model, SIGNAL(LearningTime(QTime)), this,
          SLOT(SetLearningTime(QTime)));
  connect(&m_model, SIGNAL(TestingingTime(QTime)), this,
          SLOT(SetTestingTime(QTime)));

  connect(&m_model, SIGNAL(SendMetrix(QVector<double>)), this,
          SLOT(ShowTestData(QVector<double>)));
}

void Controller::ConnectControlSlots() {
  connect(this, SIGNAL(StartLearning()), &m_model, SLOT(Learning()));
  connect(this, SIGNAL(StartTesting()), &m_model, SLOT(Testing()));
  connect(ui->learninig_rate_spb, SIGNAL(valueChanged(double)), &m_model,
          SLOT(UpdateLearningRate(double)));
  connect(this, SIGNAL(SavedMLP(QString)), &m_model, SLOT(SaveBinMLP(QString)));
}

void Controller::ConnectDataSlots() {
  connect(ui->crossvalid_spb, SIGNAL(valueChanged(int)), &m_model,
          SLOT(SetCrossValidGroup(int)));
  connect(this, SIGNAL(SendImg(QPixmap)), &m_model, SLOT(ScanImg(QPixmap)));
  connect(ui->layout_info, SIGNAL(LayoutsSended(QVector<unsigned>)), &m_model,
          SLOT(Update(QVector<unsigned>)));
}

void Controller::ConnectStateSlots() {
  connect(&m_model, SIGNAL(IsReady()), &m_model_state, SLOT(SetItReady()));

  connect(&m_model_state, SIGNAL(InitUpdating()), ui->layout_info,
          SLOT(SendLayouts()));

  connect(&m_model_state, SIGNAL(InitLoadLearningDate()),
          ui->actionLoad_learning_data, SIGNAL(triggered()));
  connect(&m_model_state, SIGNAL(InitLearning()), ui->teach_btn,
          SIGNAL(clicked()));

  connect(&m_model_state, SIGNAL(InitLoadTestingDate()),
          ui->actionLoad_testing_data, SIGNAL(triggered()));
  connect(&m_model_state, SIGNAL(InitTesting()), ui->test_btn,
          SIGNAL(clicked()));
}

void Controller::ConnectThread() {
  thr = new QThread(this);
  thr_state = new QThread(this);

  m_model.setParent(nullptr);
  m_model_state.setParent(nullptr);

  m_model.moveToThread(thr);
  m_model_state.moveToThread(thr_state);

  thr_state->start(QThread::HighPriority);
  thr->start(QThread::NormalPriority);
}

void Controller::SetChartArea() {
  ChartArea *chart = new ChartArea(this);
  connect(&m_model, SIGNAL(SendResult(QVector<double>)), chart,
          SLOT(GetResults(QVector<double>)));
  ui->result_lay->addWidget(chart);

  GraphArea *grapth = new GraphArea(this);

  connect(this, SIGNAL(StartLearning()), grapth, SLOT(ClearCanvas()));
  connect(&m_model, SIGNAL(SendNewMistake(QList<QPointF>)), grapth,
          SLOT(SetNewMistake(QList<QPointF>)));
  ui->graph_onLearn_layout->addWidget(grapth);
}

void Controller::SetTableController() {
  ui->layout_info->AddDefLayout();
  connect(&m_model, SIGNAL(LoadingModel(QVector<unsigned> &)), ui->layout_info,
          SLOT(SetLayouts(QVector<unsigned> &)));
  connect(ui->layout_count, SIGNAL(valueChanged(int)), ui->layout_info,
          SLOT(SetLayots(int)));
  connect(ui->layout_info, SIGNAL(SetCount(int)), ui->layout_count,
          SLOT(setValue(int)));
}

void Controller::on_actionLoad_learning_data_triggered() {
  if (m_model_state.CheckProcess(kIsLoadingTrainData)) {
    QString file_way = QFileDialog::getOpenFileName(
        this, tr("Load learning data"), QDir::homePath(), tr("Data (*.csv)"));
    if (!file_way.isEmpty()) {
      ui->learning_smpl_chk->setChecked(false);
      m_model.LoadLearnSample(file_way.toStdString());
      ui->learning_smpl_chk->setChecked(true);
    }
  }
}

void Controller::on_actionLoad_testing_data_triggered() {
  if (m_model_state.CheckProcess(kIsLoadingTestingData)) {
    QString file_way = QFileDialog::getOpenFileName(
        this, tr("Load testing data"), QDir::homePath(), tr("Data (*.csv)"));
    if (!file_way.isEmpty()) {
      ui->testting_sempl_chk->setChecked(false);
      m_model.LoadTestsSample(file_way.toStdString());
      ui->testting_sempl_chk->setChecked(true);
    }
  }
}

void Controller::on_teach_btn_clicked() {
  if (m_model_state.CheckProcess(kIsLearning)) {
    if (ui->type_cbx->currentIndex()) {
      m_model.SetLearningType(NetworkFlags::kGraphLearning);
    } else {
      m_model.SetLearningType(NetworkFlags::kMatrixLearning);
    }
    m_model.SetEpoch(ui->epoh_count_spb->value());
    emit StartLearning();
    ui->teach_btn->setText("Breake");
  }
}

void Controller::on_test_btn_clicked() {
  if (m_model_state.CheckProcess(kIsTesting)) {
    emit StartTesting();
    ui->test_btn->setText("Breake");
  }
}

void Controller::ShowTestData(QVector<double> testing_data) {
  ui->chance_mistake_lsd->display(testing_data.at(0));
  ui->precision_lsd->display(testing_data.at(1));
  ui->recall_lsd->display(testing_data.at(2));
  ui->F_Mes_lsd->display(testing_data.at(3));
}

void Controller::SetLearningTime(QTime time_stamp) {
  ui->average_learn_cd->display(time_stamp.toString());
}

void Controller::SetTestingTime(QTime time_stamp) {
  ui->average_test_lcd->display(time_stamp.toString());
}

void Controller::on_actionLoad_MLP_triggered() {
  if (m_model_state.CheckProcess(kIsLoadingWeigts)) {
    m_model.LoadBinMLP(QFileDialog::getOpenFileName(
        this, "Load MLP", QDir::homePath(), "MlP (*.mlp)"));
    ui->learn_pbr->setValue(100);
    ui->test_pbr->setValue(100);
  }
}

void Controller::on_actionSave_MLP_triggered() {
  emit SavedMLP(QFileDialog::getSaveFileName(this, "Save MLP", QDir::homePath(),
                                             "MlP (*.mlp)"));
}

void Controller::on_learn_pbr_valueChanged(int value) {
  if (value == 100) {
    ui->teach_btn->setText("Teach it");
    ui->test_pbr->setValue(0);
  }
}

void Controller::on_test_pbr_valueChanged(int value) {
  if (value == 100) {
    ui->test_btn->setText("Test it");
  }
}

void Controller::on_crossvalid_cbx_stateChanged(int arg1) {
  ui->crossvalid_spb->setEnabled(arg1);
  if (!arg1) {
    m_model.SetCrossValidGroup(0);
  } else {
    m_model.SetCrossValidGroup(ui->crossvalid_spb->value());
  }
}
