#ifndef MLP_PRIMITIVE_PRIMITIVE_H_
#define MLP_PRIMITIVE_PRIMITIVE_H_

#include <random>

struct Weight {
 public:
  double data = {};
  Weight(double data = distribution(generator));

 private:
  static constexpr double min_random_value = -0.5;
  static constexpr double max_random_value = +0.5;

  static std::random_device rd;
  static std::default_random_engine generator;
  static std::uniform_real_distribution<double> distribution;
};

struct Neuron {
 public:
  double data = {};
  void ActivationFunction();
  double Delta(const Neuron &target) const;
  double DerivativeActivatioFunction() const;
};

struct GNeuron : public Neuron {
 public:
  double delta = {};
  Weight *bias = {};
  std::vector<Weight *> prev;
  std::vector<Weight *> next;

  void Delta(double ans = {});
  double Delta(Neuron &target) = delete;
  void Delta(const std::vector<GNeuron> &layer);
  void ForwardPropagation(const std::vector<GNeuron> &layer);
  void CorrectWeights(const std::vector<GNeuron> &layer, double rate);
};

#endif  // MLP_PRIMITIVE_PRIMITIVE_H_
