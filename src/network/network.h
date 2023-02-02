#ifndef MLP_SRC_MLP_H_
#define MLP_SRC_MLP_H_

#include <QFileDialog>
#include <QObject>
#include <QPixmap>
#include <QThreadPool>
#include <QtMath>
#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <vector>

#include "matrix.h"
#include "normalaizeimage.h"
#include "timeceeper.h"

enum NetworkFlags : unsigned { kMatrixLearning = 0, kGraphLearning, kSize };

enum LayerType : unsigned { kSLayer = 0, kALayer, kRLayer, kLayers };

enum ConfusionMatrix : unsigned {
  kTruePositive = 0,
  kFalsePositive,
  kFalseNegative,
  kTrueNegative,
  kConfusionMatrixAttribute
};

enum Metrics {
  kAccuracy = 0,
  kPrecision,
  kRecall,
  kFMeasure,
  kMetricsAttribute
};

class Network : public QObject {
  Q_OBJECT
 public:
  Network() = delete;

  explicit Network(QObject *parent = nullptr);

  explicit Network(const Network &other) = delete;
  explicit Network(Network &&other) = delete;

  virtual ~Network();

  Network &operator=(const Network &other) = delete;
  Network &operator=(Network &&other) noexcept = delete;

  void SetLearningType(const NetworkFlags flag);

  void LoadLearnSample(const std::string &filename);
  void LoadTestsSample(const std::string &filename);

  void LoadBinMLP(QString);

 public slots:
  void Update(QVector<unsigned> depths);
  void Learning();
  void Testing();

  void ScanImg(QPixmap);
  void SaveBinMLP(QString);
  void UpdateLearningRate(double);
  void SetEpoch(unsigned epoch);
  void BreakProcess();
  void SetCrossValidGroup(int);

 signals:
  void LearningTime(QTime);
  void TestingingTime(QTime);
  void NetworkLearningStage(int);
  void NetworkTestingStage(int);
  void IsReady();
  void SendResult(QVector<double>);
  void SendMetrix(QVector<double>);
  void LoadingModel(QVector<unsigned> &depths);
  void SendNewMistake(QList<QPointF>);

 private:
  float learn_step{0.f};
  float learn_stage{0.f};
  float test_step{0.f};
  float test_stage{0.f};
  QVector<double> average_MLP_metrics_;
  ProgressCeeper tkeeper;
  bool break_flag_ = false;
  unsigned int m_epoch = 1U;
  unsigned int m_validation_parts = 0U;
  double m_learning_rate = 0.13;
  constexpr static double m_colorspace = 255.0;
  unsigned m_layers_position[LayerType::kLayers]{0U, 1U};

  std::vector<unsigned> m_depths;
  NetworkFlags m_flags;
  std::vector<Matrix<Neuron> *> m_layers_f;
  std::vector<Matrix<Neuron> *> m_layers_b;
  std::vector<Matrix<Weight> *> m_weights_main;
  std::vector<Matrix<Weight> *> m_weights_bias;
  std::vector<std::vector<GNeuron> > m_graph_layers;
  std::vector<std::pair<unsigned, Matrix<Neuron> *> > m_input_csv_learn;
  std::vector<std::pair<unsigned, Matrix<Neuron> *> > m_input_csv_tests;

  std::vector<std::pair<unsigned, Matrix<Neuron> *> > LoadSCV(
      const std::string &filename, const unsigned depth);

  void GenerateWeights(QVector<unsigned> &depths);
  void UpdateLayerPosition(unsigned last);
  void GenerateGraphLayers(QVector<unsigned> &depths);

  QVector<double> NormalizeResult();

  template <class T>
  void DestroyData(std::vector<T> &src, unsigned idx);

  template <class T1, class T2>
  void DestroyData(std::vector<std::pair<T1, T2> > &src, unsigned idx);

  void DestroyLayers();
  void DestroyWeight();
  void DestroyCSV();

  void MatrixLearning();
  std::vector<double> MatrixTesting();

  QPointF MatrixTrainingError(unsigned position, unsigned ans);
  void MatrixValidation();

  void GraphLearning();
  std::vector<double> GraphTesting();

  QPointF GraphTrainingError(unsigned position, unsigned ans);
  void GpaphValidation();

  void LayerFormation(std::vector<GNeuron> &layer, unsigned position);
  void MatForwardPropagation(Matrix<Neuron> *sensor);
  void MatCalculateDelta(unsigned ans);
  void MatBackPropagation(unsigned ans);
  void MatCalculateDelta(Matrix<Weight> *&weights, Matrix<Neuron> *&out,
                         Matrix<Neuron> *&tgt);
  void MatCorrectWeights(Matrix<Neuron> *&out, Matrix<Neuron> *&tgt,
                         Matrix<Weight> *&weights, Matrix<Weight> *&biases);

  void GraphCorrectWeights();
  void GraphCalculateDelta(unsigned ans);
  void GraphBackPropagation(unsigned ans);
  void GraphInitSLayer(Matrix<Neuron> *&layer);
  void GraphForwardPropagation(Matrix<Neuron> *layer);

  std::vector<double> FormMetrixNetwork(std::vector<unsigned> &confusion);
  void MatFormConfusionMatrix(std::vector<unsigned> &confusion, unsigned ans);

  void GraphFormConfusionMatrix(std::vector<unsigned> &confusion, unsigned ans);

  Matrix<Neuron> *SenseData(QImage);
};

#endif  // MLP_SRC_MLP_H_
