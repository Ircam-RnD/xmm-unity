//typedef unsigned long int std::size_t;
//typedef unsigned long int size_t;

#define MAX_NB_OF_CLASSES 512
#define MAX_OUTPUT_VALUES 512

// our bridge shared library's header
#include "xmmEngine.h"

#include "xmmModelTools.h"

// static variables for tmp type conversions

static const char *sccp;
static std::string ss;
static std::vector<float> sfv;
static std::vector<std::string> ssv;

// static variables for recognition results storage / passing

static const char *labels[MAX_NB_OF_CLASSES];
static float likelihoods[MAX_NB_OF_CLASSES];
static float timeProgressions[MAX_NB_OF_CLASSES];
static float outputValues[MAX_OUTPUT_VALUES];

// static variables for instances management :

static std::map<int, XmmToolBase *> models;
static std::vector<int> mFreeList;
static XmmToolBase *m;
static int currentModel = -1;

static std::map<int, XmmTrainingSetTool *> sets;
static std::vector<int> sFreeList;
static XmmTrainingSetTool *s;
static int currentSet = -1;


//=========================== INSTANCES MANAGEMENT ===========================//

//--------------------------------- MODELS -----------------------------------//

int createModelInstance(const char *arg) { //------------- create ------------//
  if (mFreeList.size() > 0) {
    currentModel = mFreeList.back();
    mFreeList.pop_back();
  } else {
    currentModel = static_cast<int>(models.size());
  }
  
  Json::Reader jr;
  Json::Value jm;
  ss = std::string(arg);
  if (ss.compare("hhmm") == 0) {
    models[currentModel] = new XmmHmmTool();
  } else if (ss.compare("gmm") == 0) {
    models[currentModel] = new XmmGmmTool();
  } else if (jr.parse(ss, jm)) {
    Json::Value jc = jm["configuration"]["default_parameters"];
    if (jc.isMember("states")) { // could be any other hmm specific config param
      models[currentModel] = new XmmHmmTool(jm);
    } else {
      models[currentModel] = new XmmGmmTool(jm);
    }
  } else {
    return -1;
  }
  
  m = models.at(currentModel);
  return currentModel;
}

int deleteModelInstance(int index) { //------------------- delete ------------//
  auto it = models.find(index);
  
  if (it != models.end() && models.size() > 0) {
    delete models.at(index);
    models.erase(index);
    mFreeList.push_back(index);
    std::sort(mFreeList.begin(), mFreeList.end(), std::greater<int>());
    
    if (models.size() > 0) {
      if (index == currentModel) {
        currentModel = models.begin()->first;
      }
      m = models.at(currentModel);
    } else {
      currentModel = -1;
    }
    return currentModel;
  }
  
  return -1;
}

void setCurrentModelInstance(int index) { //------------ set current ---------//
  // WARNING : NOT SAFE !
  // manage the indices carefully on the C# side !
  // (taken care of by the XmmModel wrapper class)
  currentModel = index;
  m = models.at(index);
}

int getCurrentModelInstance() {
  return currentModel;
}

int getNbOfModelInstances() {
  // TODO : do better, considering C#'s GC is running on a background thread
  // the result could be "false" in some cases !!!
  return static_cast<int>(models.size());
}



//------------------------------ TRAINING SETS -------------------------------//

int createSetInstance() { //------------------------------ create ------------//
  if (sFreeList.size() > 0) {
    currentSet = sFreeList.back();
    sFreeList.pop_back();
  } else {
    currentSet = static_cast<int>(sets.size());
  }
  
  sets[currentSet] = new XmmTrainingSetTool();
  
  s = sets.at(currentSet);
  return currentSet;
}

int deleteSetInstance(int index) { //--------------------- delete ------------//
  auto it = sets.find(index);
  
  if (it != sets.end() && sets.size() > 0) {
    delete sets.at(index);
    sets.erase(index);
    sFreeList.push_back(index);
    std::sort(sFreeList.begin(), sFreeList.end(), std::greater<int>());
    
    if (sets.size() > 0) {
      if (index == currentSet) {
        currentSet = sets.begin()->first;
      }
      s = sets.at(currentSet);
    } else {
      currentSet = -1;
    }
    return currentSet;
  }
  
  return -1;
}

void setCurrentSetInstance(int index) {
  // WARNING : NOT SAFE !
  // manage the indices carefully on the C# side !
  // (taken care of by the XmmTrainingSet wrapper class)
  currentSet = index;
  s = sets.at(index);
}

int getCurrentSetInstance() {
  return currentSet;
}

int getNbOfSetInstances() {
  // TODO : do better, considering C#'s GC is running on a background thread
  // the result could be "false" in some cases !!!
  return static_cast<int>(sets.size());
}

//================================ TRAINING ==================================//

int trainModelFromSet() {
  if (currentModel >= 0 && currentSet >= 0) {
    m->train(s);
    return 1;
  }
  
  return 0;
}

int trained() {
  if (currentModel < 0) return 0;
  return m->trained() ? 1 : 0;
}

//========================= TRAINING SETS INTERFACE ==========================//

void addPhraseFromData(const char *label, const char **colNames, float *phrase,
                       int dimIn, int dimOut, int phraseSize) {
  if (currentSet < 0) return;
  
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
  
  s->addPhrase(xp);
}

void addPhrase(const char *sp) {
  if (currentSet < 0) return;
  
  Json::Reader jr;
  Json::Value jp;
  if (jr.parse(std::string(sp), jp)) {
    s->addPhrase(xmm::Phrase(jp));
  }
}

const char *getPhrase(int index) {
  if (currentSet < 0) return "";
  
  Json::StyledWriter jw;
  ss = jw.write(s->getPhrase(index).toJson());
  sccp = ss.c_str();
  return sccp;
}

void removePhrase(int index) {
  if (currentSet < 0) return;
  s->removePhrase(index);
}

void removePhrasesOfLabel(const char *label) {
  if (currentSet < 0) return;
  s->removePhrasesOfLabel(std::string(label));
}

int getTrainingSetSize() {
  if (currentSet < 0) return 0;
  return s->size();
}

int getTrainingSetNbOfLabels() {
  if (currentSet < 0) return 0;
  return static_cast<int>(s->getLabels().size());
}

const char **getTrainingSetLabels() {
  if (currentSet < 0) return labels;
  
  ssv = s->getLabels();
  for (int i = 0; i < ssv.size(); ++i) {
    labels[i] = ssv[i].c_str();
  }
  return labels;
}

const char *getTrainingSet() {
  if (currentSet < 0) return "";
  
  Json::StyledWriter jw;
  ss = jw.write(s->getSet().toJson());
  sccp = ss.c_str();
  return sccp;
}

const char *getSubTrainingSet(const char *label) {
  if (currentSet < 0) return "";
  
  Json::StyledWriter jw;
  ss = jw.write(s->getSubSet(std::string(label)).toJson());
  sccp = ss.c_str();
  return sccp;
}

void setTrainingSet(const char *sts) {
  if (currentSet < 0) return;
  
  Json::Reader jr;
  Json::Value js;
  if (jr.parse(std::string(sts), js)) {
    s->setSet(xmm::TrainingSet(js));
  }
}

void addTrainingSet(const char *sts) {
  if (currentSet < 0) return;
  
  Json::Reader jr;
  Json::Value js;
  if (jr.parse(std::string(sts), js)) {
    s->addSet(xmm::TrainingSet(js));
  }
}

void clearTrainingSet() {
  if (currentSet < 0) return;
  s->clearSet();
}


//============================= MODELS INTERFACE =============================//

//------------------------ config parameters getters -------------------------//

int getModelType() {
  if (currentModel < 0) return -1;
  
  int modelType;
  switch (m->getModelType()) {
    case hmmModelTypeE:
      modelType = 1;
      break;
      
    case gmmModelTypeE:
    default:
      modelType = 0;
      break;
  }
  
  return modelType;
}

int getBimodal() {
  if (currentModel < 0) return 0;
  return m->getBimodal();
}

int getNbOfModels() {
  if (currentModel < 0) return 0;
  return static_cast<int>(m->getNbOfModels());
}

const char **getModelLabels() {
  if (currentModel < 0) return labels;
  
  ssv = m->getLabels();
  for (int i = 0; i < ssv.size(); ++i) {
    labels[i] = ssv[i].c_str();
  }
  return labels;
}

int getInputDimension() {
  if (currentModel < 0) return 0;
  return m->getInputDimension();
}

int getOutputDimension() {
  if (currentModel < 0) return 0;
  return m->getOutputDimension();
}

int getGaussians() {
  if (currentModel < 0) return 0;
  return m->getGaussians();
}

float getRelativeRegularization() {
  if (currentModel < 0) return 0;
  return m->getRelativeRegularization();
}

float getAbsoluteRegularization() {
  if (currentModel < 0) return 0;
  return m->getAbsoluteRegularization();
}

// see xmmGaussianDistribution.hpp : 0 = full, 1 = diagonal
int getCovarianceMode() {
  if (currentModel < 0) return 0;
  return m->getCovarianceMode();
}

int getHierarchical() {
  if (currentModel < 0) return 0;
  return m->getHierarchical() ? 1 : 0;
}

int getStates() {
  if (currentModel < 0) return 0;
  return m->getStates();
}

// see xmmHmmParameters.hpp : 0 = ergodic, 1 = leftright
int getTransitionMode() {
  if (currentModel < 0) return 0;
  return m->getTransitionMode();
}

// see xmmHmmParameters.hpp : 0 = full, 1 = windowed, 2 = likeliest
int getRegressionEstimator() {
  if (currentModel < 0) return 0;
  return m->getRegressionEstimator();
}


//------------------------ config parameters setters -------------------------//

void setGaussians(int g) {
  if (currentModel < 0) return;
  m->setGaussians(g);
}

void setRelativeRegularization(float relReg) {
  if (currentModel < 0) return;
  m->setRelativeRegularization(relReg);
}

void setAbsoluteRegularization(float absReg) {
  if (currentModel < 0) return;
  m->setAbsoluteRegularization(absReg);
}

void setCovarianceMode(int c) {
  if (currentModel < 0) return;
  m->setCovarianceMode(c);
}

void setHierarchical(int h) {
  if (currentModel < 0) return;
  m->setHierarchical(h != 0);
}

void setStates(int s) {
  if (currentModel < 0) return;
  m->setStates(s);
}

void setTransitionMode(int t) {
  if (currentModel < 0) return;
  m->setTransitionMode(t);
}

void setRegressionEstimator(int r) {
  if (currentModel < 0) return;
  m->setRegressionEstimator(r);
}


//------------------------- filtering related methods ------------------------//

int getLikelihoodWindow() {
  if (currentModel < 0) return -1;
  return m->getLikelihoodWindow();
  
}

void setLikelihoodWindow(int w) {
  if (currentModel < 0) return;
  m->setLikelihoodWindow(w);
}

//int isTraining();
//void cancelTraining();

const char *getModel() {
  if (currentModel < 0) return "";
  
  Json::StyledWriter jw;
  ss = jw.write(m->getModel());
  sccp = ss.c_str();
  return sccp;
}

void clearModel() {
  if (currentModel < 0) return;
  m->clearModel();
}

void filter(float *observation, int observationSize) {
  if (currentModel < 0) return;

  std::vector<float> obs(observationSize);
  for (int i = 0; i < observationSize; ++i) {
    obs[i] = *(observation + i);
  }
  m->filter(obs);
}

// TODO : implement this (?)
const char *getFilteringResults() {
  if (currentModel < 0) return "";
  return "";
}

const char *getLikeliest() {
  if (currentModel < 0) return "";
  
  ss = m->getLikeliestLabel();
  if (!m->trained()) {
    ss = "";
  }
  sccp = ss.c_str();
  return sccp;
}

float *getLikelihoods() {
  if (currentModel < 0) return likelihoods;
  
  sfv = m->getLikelihoods();
  for (int i = 0; i < sfv.size(); ++i) {
    likelihoods[i] = sfv[i];
  }
  return likelihoods;
}

float *getTimeProgressions() {
  if (currentModel < 0) return timeProgressions;
  
  sfv = m->getTimeProgressions();
  for (int i = 0; i < sfv.size(); ++i) {
    timeProgressions[i] = sfv[i];
  }
  return timeProgressions;
}

float *getRegression() {
  if (currentModel < 0) return outputValues;
  
  sfv = m->getOutputValues();
  for (int i = 0; i < sfv.size(); ++i) {
    outputValues[i] = sfv[i];
  }
  return outputValues;
}

void reset() {
  if (currentModel < 0) return;
  m->reset();
}

