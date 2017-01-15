// this is our interface to Unity :

extern "C" {

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

	// config parameters getters
	int getModelType(); 					// 0 : GMM, 1 : HHMM
	int getNbOfModels();
	int getInputDimension();
	int getOutputDimension();
	bool getBimodal();
	int getGaussians();
	float getRelativeRegularization();
	float getAbsoluteRegularization();
	// see xmmGaussianDistribution.hpp :
	int getCovarianceMode(); 			// 0 : full, 1 : diagonal
	bool getHierarchical();
	int getStates();	
	// see xmmHmmParameters.hpp :
	int getTransitionMode(); 			// 0 : ergodic, 1 : leftright
	int getRegressionEstimator(); // 0 : full, 1 : windowed, 2 : likeliest

	// config parameters setters
	void setModelType(int t);
	void setGaussians(int g);
	void setRelativeRegularization(float relReg);
	void setAbsoluteRegularization(float absReg);
	void setCovarianceMode(int c);
	void setHierarchical(bool h);
	void setStates(int s);	
	void setTransitionMode(int t);
	void setRegressionEstimator(int r);

	// get / set filtering parameters
	int getLikelihoodWindow();
	void setLikelihoodWindow(int w);

	void train();
	bool isTraining();
	void cancelTraining();

	const char *getModel();
	void setModel(const char *sm); // JSON model string
	void clearModel();

	void filter(float *observation, int observationSize);

	const char *getFilteringResults();	// JSON string (not very useful)
	const char *getLikeliest();					// string label
	float *getLikelihoods(); 			// we have "getNbOfModels()" likelihoods
	float *getTimeProgressions(); // we have "getNbOfModels()" time progressions
	float *getRegression();				// we have "getOutputDimension()" output values

	void reset();
}