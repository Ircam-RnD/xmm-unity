// this is our interface to Unity :

extern "C" {
  
  //========================== INSTANCES MANAGEMENT ==========================//
  
  // "hhmm", "gmm", or JSON string model
  int createModelInstance(const char *modelType);
  int deleteModelInstance(int index);
  
  void setCurrentModelInstance(int index);
  int getCurrentModelInstance();
  int getNbOfModelInstances();
  
  int createSetInstance();
  int deleteSetInstance(int index);
  
  void setCurrentSetInstance(int index);
  int getCurrentSetInstance();
  int getNbOfSetInstances();
  
  int trainModelFromSet();
  int trained();
  
  //======================== TRAINING SETS INTERFACE =========================//
  
  // we determine the modality when first phrase is added to the empty set
  void addPhraseFromData(const char *label, const char **colNames, float *phrase,
                         int dimIn, int dimOut, int phraseSize);
  void addPhrase(const char *sp);
  const char *getPhrase(int index);
  void removePhrase(int index);
  void removePhrasesOfLabel(const char *label);
  
  int getTrainingSetSize();
  int getTrainingSetNbOfLabels();
  
  const char **getTrainingSetLabels();
  const char *getTrainingSet();
  const char *getSubTrainingSet(const char *label);
  
  void setTrainingSet(const char *sts);
  void addTrainingSet(const char *sts);
  void clearTrainingSet();
  
  //============================ MODELS INTERFACE ============================//
  
  // config parameters getters :
  int getModelType();            // 0 : GMM, 1 : HHMM
  int getBimodal();
  int getNbOfModels();
  const char **getModelLabels();
  int getInputDimension();
  int getOutputDimension();
  int getGaussians();
  float getRelativeRegularization();
  float getAbsoluteRegularization();
  // see xmmGaussianDistribution.hpp :
  int getCovarianceMode();       // 0 : full, 1 : diagonal
  int getHierarchical();
  int getStates();
  // see xmmHmmParameters.hpp :
  int getTransitionMode();      // 0 : ergodic, 1 : leftright
  int getRegressionEstimator(); // 0 : full, 1 : windowed, 2 : likeliest
  
  // config parameters setters
  void setGaussians(int g);
  void setRelativeRegularization(float relReg);
  void setAbsoluteRegularization(float absReg);
  void setCovarianceMode(int c);
  void setHierarchical(int h);
  void setStates(int s);
  void setTransitionMode(int t);
  void setRegressionEstimator(int r);
  
  // filtering related methods
  int getLikelihoodWindow();
  void setLikelihoodWindow(int w);
  
  // TODO : allow asynchronous training ?
  //int isTraining();
  //void cancelTraining();
  
  const char *getModel();
  void clearModel();
  
  void filter(float *observation, int observationSize);
  
  const char *getFilteringResults();  // JSON string (not very useful)
  const char *getLikeliest();         // string label
  float *getLikelihoods();      // we have "getNbOfModels()" likelihoods
  float *getTimeProgressions(); // we have "getNbOfModels()" time progressions
  float *getRegression();       // we have "getOutputDimension()" output values
  
  void reset();
}
