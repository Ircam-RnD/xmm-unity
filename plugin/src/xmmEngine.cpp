//typedef unsigned long int std::size_t;
//typedef unsigned long int size_t;

#define MAX_NB_OF_CLASSES 512
#define MAX_OUTPUT_VALUES 512

#include "xmmEngine.h"  // our bridge shared library's header
#include "xmmEngineCore.h"
#include <string>

// static variables :

// TODO : upgrade this to an xmmEngineCore factory !!!!!(?)
static xmmEngineCore x;

static const char *labels[MAX_NB_OF_CLASSES];
static float likelihoods[MAX_NB_OF_CLASSES];
static float timeProgressions[MAX_NB_OF_CLASSES];
static float outputValues[MAX_OUTPUT_VALUES];

static const char *s;
static std::string ss;
static std::vector<float> fv;
static std::vector<std::string> ssv;
//static float 

// we determine the modality when first phrase is added and set is empty
void addPhraseFromData(const char *label, const char **colNames, float *phrase,
                       int dimIn, int dimOut, int phraseSize) {
  xmm::Phrase xp;
  int dim = dimIn + dimOut;

  if (dimOut > 0) {
    xp = xmm::Phrase(xmm::MemoryMode::OwnMemory, xmm::Multimodality::Bimodal);
  }

  // set phrase dimensions  
  xp.dimension.set(static_cast<unsigned int>(dim));
  if (dimOut > 0) {
    xp.dimension_input.set(static_cast<unsigned int>(dimIn));
  }

  // set phrase label
  xp.label.set(std::string(label));

  // set phrase column names
  std::vector<std::string> cn(dim);
  for (int i = 0; i < dim; ++i) {
    cn[i] = std::string(*(colNames + i));
  }
  xp.column_names = cn;

  // fill the phrase
  std::vector<float> obs(dim);
  for (int i = 0; i < phraseSize; ++i) {
    for (int j = 0; j < dim; ++j) {
      obs[j] = *(phrase + i * dim + j);
    }
    xp.record(obs);
  }

  x.addPhrase(xp);
}

void addPhrase(const char *sp) {
  Json::Reader jr;
  Json::Value jp;
  if (jr.parse(std::string(sp), jp)) {
    x.addPhrase(xmm::Phrase(jp));
  }
}

const char *getPhrase(int index) {
  Json::StyledWriter jw;
  ss = jw.write(x.getPhrase(index).toJson());
  s = ss.c_str();
  return s;
  //return jw.write(x.getPhrase(index).toJson()).c_str();
}

void removePhrase(int index) {
  x.removePhrase(index);
}

void removePhrasesOfLabel(const char *label) {
  x.removePhrasesOfLabel(std::string(label));
}

int getTrainingSetSize() {
  return x.getTrainingSetSize();
}

int getTrainingSetNbOfLabels() {
  return x.getTrainingSetLabels().size();
}

const char **getTrainingSetLabels() {
  ssv = x.getTrainingSetLabels();
  for (int i = 0; i < ssv.size(); ++i) {
    labels[i] = ssv[i].c_str();
  }
  return labels;
}

const char *getTrainingSet() {
  Json::StyledWriter jw;
  ss = jw.write(x.getTrainingSet().toJson());
  s = ss.c_str();
  return s;
  //return jw.write(x.getTrainingSet().toJson()).c_str();
}

const char *getSubTrainingSet(const char *label) {
  Json::StyledWriter jw;
  ss = jw.write(x.getTrainingSet(std::string(label)).toJson());
  s = ss.c_str();
  return s;  
}

void setTrainingSet(const char *sts){
  Json::Reader jr;
  Json::Value js;
  if (jr.parse(std::string(sts), js)) {
    x.setTrainingSet(xmm::TrainingSet(js));
  }
}

void addTrainingSet(const char *sts) {
  Json::Reader jr;
  Json::Value js;
  if (jr.parse(std::string(sts), js)) {
    x.addTrainingSet(xmm::TrainingSet(js));
  }
}

void clearTrainingSet() {
  x.clearTrainingSet();
}

//======================= config parameters getters : ========================//

// 0 = GMM, 1 = HHMM
int getModelType() {
  return x.getModelType();
}

int getNbOfModels() {
  return x.getNbOfModels();
}

bool getBimodal() {
  return x.getBimodal();
}

int getInputDimension() {
  return x.getInputDimension();
}

int getOutputDimension() {
  return x.getOutputDimension();
}

int getGaussians() {
  return x.getGaussians();
}

float getRelativeRegularization() {
  return x.getRelativeRegularization();
}

float getAbsoluteRegularization() {
  return x.getAbsoluteRegularization();
}

// see xmmGaussianDistribution.hpp : 0 = full, 1 = diagonal
int getCovarianceMode() {
  return x.getCovarianceMode();
}

bool getHierarchical() {
  return x.getHierarchical();
}

int getStates() {
  return x.getStates();
}

// see xmmHmmParameters.hpp :
// 0 = ergodic, 1 = leftright
int getTransitionMode() {
  return x.getTransitionMode();
}

// 0 = full, 1 = windowed, 2 = likeliest
int getRegressionEstimator() {
  return x.getRegressionEstimator();
}

//======================= config parameters setters : ========================//

void setModelType(int t) {
  x.setModelType(t);
}

void setGaussians(int g) {
  x.setGaussians(g);
}

void setRelativeRegularization(float relReg) {
  x.setRelativeRegularization(relReg);
}

void setAbsoluteRegularization(float absReg) {
  x.setAbsoluteRegularization(absReg);
}

void setCovarianceMode(int c) {
  x.setCovarianceMode(c);
}

void setHierarchical(bool h) {
  x.setHierarchical(h);
}

void setStates(int s) {
  x.setStates(s);
}

void setTransitionMode(int t) {
  x.setTransitionMode(t);
}

void setRegressionEstimator(int r) {
  x.setRegressionEstimator(r);
}

//============================================================================//

int getLikelihoodWindow() {
  return x.getLikelihoodWindow();
}

void setLikelihoodWindow(int w) {
  x.setLikelihoodWindow(w);
}

void train() {
  x.train();
}

bool isTraining() {
  return x.isTraining();
}

void cancelTraining() {
  x.cancelTraining();
}

const char *getModel() {
  Json::StyledWriter jw;
  ss = jw.write(x.getModel());
  s = ss.c_str();
  return s;
}

void setModel(const char *sm) {
  Json::Reader jr;
  Json::Value jm;
  if (jr.parse(std::string(sm), jm)) {
    x.setModel(jm);
  }
  // x.setModel(jr.parse(std::string(sm)));
}

void clearModel() {
  x.clearModel();
}

void filter(float *observation, int observationSize) {
  std::vector<float> obs(observationSize);
  for (int i = 0; i < observationSize; ++i) {
    obs[i] = *(observation + i);
  }
  x.filter(obs);
}

// return JSON string (not very useful)
const char *getFilteringResults() {
  return "";
}

const char *getLikeliest() {
  ss = x.getLikeliestLabel();
  s = ss.c_str();
  return s;
  //return x.getLikeliestLabel().c_str();
}

float *getLikelihoods() {
  fv = x.getLikelihoods();
  for (int i = 0; i < fv.size(); ++i) {
    likelihoods[i] = fv[i];
  }
  return likelihoods;
}

float *getTimeProgressions() {
  fv = x.getTimeProgressions();
  for (int i = 0; i < fv.size(); ++i) {
    timeProgressions[i] = fv[i];
  }
  return timeProgressions;  
}

float *getRegression() {
  fv = x.getOutputValues();
  for (int i = 0; i < fv.size(); ++i) {
    outputValues[i] = fv[i];
  }
  return outputValues;
}

void reset() {
  x.reset();
}
