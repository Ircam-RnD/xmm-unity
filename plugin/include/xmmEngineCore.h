#include "xmmTool.h"

enum modelTypeE { gmmModelTypeE = 0, hmmModelTypeE };

class xmmEngineCore {

private:
	modelTypeE modelType;
	bool bimodal;
	xmmToolBase *xmm;

	xmm::TrainingSet set;
	std::vector<int> freeList;

public:
	xmmEngineCore();
	~xmmEngineCore();

	int addPhrase(xmm::Phrase xp);
	xmm::Phrase getPhrase(int index);
	void removePhrase(int index);
	void removePhrasesOfLabel(std::string label);
	int getTrainingSetSize();
	std::vector<std::string> getTrainingSetLabels();
	xmm::TrainingSet getTrainingSet();
	xmm::TrainingSet getTrainingSet(std::string label);
	// Json::Value getTrainingSet();
	// Json::Value getTrainingSet(std::string label);
	void setTrainingSet(xmm::TrainingSet set);
	void addTrainingSet(xmm::TrainingSet set);
	// void setTrainingSet(Json::Value jset);
	// void addTrainingSet(Json::Value jset);
	void clearTrainingSet();

	int getModelType();
	int getNbOfModels();
	bool getBimodal();
	int getInputDimension();
	int getOutputDimension();
	int getGaussians();
	float getRelativeRegularization();
	float getAbsoluteRegularization();
	int getCovarianceMode();
	bool getHierarchical();
	int getStates();
	int getTransitionMode();
	int getRegressionEstimator();

	void setModelType(int t);
	void setGaussians(int g);
	void setRelativeRegularization(float relReg);
	void setAbsoluteRegularization(float absReg);
	void setCovarianceMode(int c);
	void setHierarchical(bool h);
	void setStates(int s);
	void setTransitionMode(int t);
	void setRegressionEstimator(int r);

	int getLikelihoodWindow();
	void setLikelihoodWindow(int w);

	void train();
	bool isTraining();
	void cancelTraining();

	Json::Value getModel();
	void setModel(Json::Value jm);
	void clearModel();

	void filter(const std::vector<float>& obs);
	std::string getLikeliestLabel();
	std::vector<float> getLikelihoods();
	std::vector<float> getTimeProgressions();
	std::vector<float> getOutputValues();

	void reset();

private:
	void updateModel();
};