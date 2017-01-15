#ifndef _XMMTOOL_H_
#define _XMMTOOL_H_

#include "xmm.h"

class xmmToolBase {
public:
  virtual ~xmmToolBase() {}

  virtual int getNbOfModels() = 0;
  virtual int getInputDimension() = 0;
  virtual int getOutputDimension() = 0;
  virtual int getGaussians() = 0;
  virtual float getRelativeRegularization() = 0;
  virtual float getAbsoluteRegularization() = 0;
  virtual int getCovarianceMode() = 0;
  virtual bool getHierarchical() = 0;
  virtual int getStates() = 0;
  virtual int getTransitionMode() = 0;
  virtual int getRegressionEstimator() = 0;

  virtual void setGaussians(int g) = 0;
  virtual void setRelativeRegularization(float relReg) = 0;
  virtual void setAbsoluteRegularization(float absReg) = 0;
  virtual void setCovarianceMode(int c) = 0;
  virtual void setHierarchical(bool h) = 0;
  virtual void setStates(int s) = 0;
  virtual void setTransitionMode(int t) = 0;
  virtual void setRegressionEstimator(int r) = 0;

  virtual int getLikelihoodWindow() = 0;
  virtual void setLikelihoodWindow(int w) = 0;

  virtual void train(xmm::TrainingSet *set) = 0;
  virtual bool isTraining() = 0;
  virtual void cancelTraining() = 0;

  virtual Json::Value getModel() = 0;
  virtual void setModel(Json::Value jm) = 0;
  virtual void clearModel() = 0;

  virtual void filter(std::vector<float> const& obs) = 0;
  virtual std::string getLikeliestLabel() = 0;
  virtual std::vector<float> getLikelihoods() = 0;
  virtual std::vector<float> getTimeProgressions() = 0;
  virtual std::vector<float> getOutputValues() = 0;

  virtual void reset() = 0;
};

// templated derived class

template <class Model>
class xmmTool : public xmmToolBase {
protected:
  xmmTool(bool bimodal = false) {
    model = Model(bimodal);
    model.configuration.multithreading = xmm::MultithreadingMode::Sequential;
    model.configuration.multiClass_regression_estimator = xmm::MultiClassRegressionEstimator::Mixture;
    model.configuration.gaussians.set(1);
    model.configuration.relative_regularization.set(0.001);
    model.configuration.absolute_regularization.set(0.001);
    model.configuration.changed = true;
  }

public:
  virtual ~xmmTool() {}

  virtual int getNbOfModels() {
    return model.size();
  }

  virtual int getInputDimension() {
    std::shared_ptr<xmm::SharedParameters> params = model.shared_parameters;
    if (model.shared_parameters->bimodal.get()) {
      return params->dimension_input.get();
    } else {
      return params->dimension.get();
    }
  }

  virtual int getOutputDimension() {
    std::shared_ptr<xmm::SharedParameters> params = model.shared_parameters;
    if (model.shared_parameters->bimodal.get()) {
      return params->dimension.get() - params->dimension_input.get();
    } else {
      return 0;
    }
  }

  virtual int getGaussians() {
    return model.configuration.gaussians.get();
  }

  virtual float getRelativeRegularization() {
    return model.configuration.relative_regularization.get();
  }

  virtual float getAbsoluteRegularization() {
    return model.configuration.absolute_regularization.get();
  }

  virtual int getCovarianceMode() {
    int res;
    switch (model.configuration.covariance_mode.get()) {
      case xmm::GaussianDistribution::CovarianceMode::Diagonal:
        res = 1;
        break;

      case xmm::GaussianDistribution::CovarianceMode::Full:
      default:
        res = 0;
        break;
    }
    return res;
  }

  // only relevant for HMMs
  virtual bool getHierarchical() = 0;
  virtual int getStates() = 0;
  virtual int getTransitionMode() = 0;
  virtual int getRegressionEstimator() = 0;

  virtual void setGaussians(int g) {
    model.configuration.gaussians.set(g);
    model.configuration.changed = true;
  }

  virtual void setRelativeRegularization(float relReg) {
    model.configuration.relative_regularization.set(relReg);
    model.configuration.changed = true;
  }

  virtual void setAbsoluteRegularization(float absReg) {
    model.configuration.absolute_regularization.set(absReg);
    model.configuration.changed = true;
  }

  virtual void setCovarianceMode(int c) {
    xmm::GaussianDistribution::CovarianceMode cm;
    switch (c) {
      case 1:
        cm = xmm::GaussianDistribution::CovarianceMode::Diagonal;
        break;

      case 0:
      default:
        cm = xmm::GaussianDistribution::CovarianceMode::Full;
        break;
    }
    model.configuration.covariance_mode.set(cm);
    model.configuration.changed = true;
  }

  // only relevant for HMMs
  virtual void setHierarchical(bool h) = 0;
  virtual void setStates(int s) = 0;
  virtual void setTransitionMode(int t) = 0;
  virtual void setRegressionEstimator(int r) = 0;

  virtual int getLikelihoodWindow() {
    return model.shared_parameters->likelihood_window.get();
  }

  virtual void setLikelihoodWindow(int w) {
    model.shared_parameters->likelihood_window.set(w);
  }

  virtual void train(xmm::TrainingSet *set) {
    model.train(set);
  }

  virtual bool isTraining() {
    return !model.trained();
  }

  virtual void cancelTraining() {
    model.cancelTraining();
  }

  virtual Json::Value getModel() {
    return model.toJson();
  }

  // virtual void setModel(Json::Value jm) {
  //   // is this really a good idea to expose this in this way ?
  //   // model = Model(jm);
  //   // model.reset(); ?
  //   // remember to update the settings in the derived classes
  // }
  // keep it pure virtual here for now :
  virtual void setModel(Json::Value jm) = 0;

  virtual void clearModel() {
    model.clear();
  }

  virtual void filter(std::vector<float> const& obs) {
    model.filter(obs);
  }

  virtual std::string getLikeliestLabel() = 0;
  virtual std::vector<float> getLikelihoods() = 0;
  virtual std::vector<float> getTimeProgressions() = 0;
  virtual std::vector<float> getOutputValues() = 0;

  virtual void reset() {
    model.reset();
  }

protected:
  Model model;
};

//========================== THE "REAL" CLASSES ==============================//

//------------------------------ GMM / GMR -----------------------------------//

class xmmGmmTool : public xmmTool<xmm::GMM> {
public:
  xmmGmmTool(bool bimodal = false) : xmmTool(bimodal) {}
  ~xmmGmmTool() {}

  bool getHierarchical() { return false; }
  int getStates() { return 1; }
  int getTransitionMode() { return -1; }
  int getRegressionEstimator() { return -1; }

  void setHierarchical(bool h) {}
  void setStates(int s) {}
  void setTransitionMode(int t) {}
  void setRegressionEstimator(int r) {}

  void setModel(Json::Value jm) {}

  std::string getLikeliestLabel() {
    return model.results.likeliest;
  }

  std::vector<float> getLikelihoods() {
    // std::vector<double>& l = model.results.smoothed_log_likelihoods;
    std::vector<double>& l = model.results.smoothed_normalized_likelihoods;
    return std::vector<float>(l.begin(), l.end());
  }

  std::vector<float> getTimeProgressions() {
    return std::vector<float>(0);
  }

  std::vector<float> getOutputValues() {
    return model.results.output_values;
  }
};

//------------------------------ HMM / HMR -----------------------------------//

class xmmHmmTool : public xmmTool<xmm::HierarchicalHMM> {
public:
  xmmHmmTool(bool bimodal = false) : xmmTool(bimodal) {}
  ~xmmHmmTool() {}

  bool getHierarchical() {
    return model.configuration.hierarchical.get();
  }

  int getStates() {
    return model.configuration.states.get();
  }

  int getTransitionMode() {
    int res;
    switch (model.configuration.transition_mode.get()) {
      case xmm::HMM::TransitionMode::LeftRight:
        res = 1;
        break;

      case xmm::HMM::TransitionMode::Ergodic:
      default:
        res = 0;
        break;
    }
    return res;
    // return model.configuration.transition_mode.get();
  }

  int getRegressionEstimator() {
    int res;
    switch (model.configuration.regression_estimator.get()) {
      case xmm::HMM::RegressionEstimator::Windowed:
        res = 1;
        break;

      case xmm::HMM::RegressionEstimator::Likeliest:
        res = 2;
        break;

      case xmm::HMM::RegressionEstimator::Full:
      default:
        res = 0;
        break;
    }
    return res;
    // return model.configuration.regression_estimator.get();
  }

  void setHierarchical(bool h) {
    model.configuration.hierarchical.set(h);
    model.configuration.changed = true;
  }

  void setStates(int s) {
    model.configuration.states.set(s);
    model.configuration.changed = true;
  }

  void setTransitionMode(int t) {
    xmm::HMM::TransitionMode tm;
    switch (t) {
      case 1:
        tm = xmm::HMM::TransitionMode::LeftRight;
        break;

      case 0:
      default:
        tm = xmm::HMM::TransitionMode::Ergodic;
        break;
    }
    model.configuration.transition_mode.set(tm);
    model.configuration.changed = true;
  }

  void setRegressionEstimator(int r) {
    xmm::HMM::RegressionEstimator re;
    switch (r) {
      case 1:
        re = xmm::HMM::RegressionEstimator::Windowed;
        break;

      case 2:
        re = xmm::HMM::RegressionEstimator::Likeliest;
        break;

      case 0:
      default:
        re = xmm::HMM::RegressionEstimator::Full;
        break;
    }
    model.configuration.regression_estimator.set(re);
    model.configuration.changed = true;
  }

  void setModel(Json::Value jm) {}

  std::string getLikeliestLabel() {
    return model.results.likeliest;
  }

  std::vector<float> getLikelihoods() {
    // std::vector<double>& l = model.results.smoothed_log_likelihoods;
    std::vector<double>& l = model.results.smoothed_normalized_likelihoods;
    return std::vector<float>(l.begin(), l.end());
  }

  std::vector<float> getTimeProgressions() {
    std::vector<float> res(model.size());
    int i = 0;
    for (auto &m : model.models) {
      res[i] = m.second.results.progress;
      i++;
    }
    return res;
  }

  std::vector<float> getOutputValues() {
    return model.results.output_values;
  }
};

#endif /* _XMMTOOL_H_ */
