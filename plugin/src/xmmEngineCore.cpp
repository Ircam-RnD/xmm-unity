#include "xmmEngineCore.h"

xmmEngineCore::xmmEngineCore() {
  modelType = gmmModelTypeE;
  bimodal = false;
	xmm = new xmmGmmTool();
}

xmmEngineCore::~xmmEngineCore() {
	delete xmm;
}

//============================================================================//

int
xmmEngineCore::addPhrase(xmm::Phrase xp) {
  int index;

  if (freeList.size() == 0) {
    index = set.size();
  } else {
    index = freeList.back();
    freeList.pop_back();
  }

  if (set.size() == 0) {
    if (xp.bimodal()) {
      set = xmm::TrainingSet(xmm::MemoryMode::OwnMemory,
                             xmm::Multimodality::Bimodal);
      set.dimension_input.set(xp.dimension_input.get());
    } else {
      set = xmm::TrainingSet();
    }

    set.dimension.set(xp.dimension.get());
    set.column_names.set(xp.column_names, true);

    // if multimodality changed :
    if (bimodal != xp.bimodal()) {
      bimodal = xp.bimodal();
      updateModel();
    }
  }

  set.addPhrase(index, xp);
  return index;
}

xmm::Phrase
xmmEngineCore::getPhrase(int index) {
	bool empty = false;

  for (int i = 0; i < freeList.size(); ++i) {
    if (index == freeList[i]) {
    	empty = true;
    	break;
    }
  }

  if (!empty && index < set.size() + freeList.size()) {
    return xmm::Phrase(*(set.getPhrase(index)));
  } else {
    //throw std::exception ?
    xmm::Phrase xp;
    return xp;
  }
}

void
xmmEngineCore::removePhrase(int index) {
	if (index >= set.size() + freeList.size()) {
		return;
	}

  for (int i = 0; i < freeList.size(); ++i) {
    if (index == freeList[i]) {
    	return;
    }
  }

	freeList.push_back(index);
	std::sort(freeList.begin(), freeList.end(), std::greater<int>());
	set.removePhrase(index);
}

void
xmmEngineCore::removePhrasesOfLabel(std::string label) {
	xmm::TrainingSet *s = set.getPhrasesOfClass(label);

	for(auto const &phrase : *s) {
		freeList.push_back(phrase.first);
	}

	std::sort(freeList.begin(), freeList.end(), std::greater<int>());
	set.removePhrasesOfClass(label);
}

int
xmmEngineCore::getTrainingSetSize() {
 return set.size();
}

std::vector<std::string>
xmmEngineCore::getTrainingSetLabels() {
  std::set<std::string> labels = set.labels();
  std::vector<std::string> res(labels.begin(), labels.end());
  return res;
}

xmm::TrainingSet
xmmEngineCore::getTrainingSet() {
  return set;
}

xmm::TrainingSet
xmmEngineCore::getTrainingSet(std::string label) {
  return *(set.getPhrasesOfClass(label));
}

void
xmmEngineCore::setTrainingSet(xmm::TrainingSet set) {
  this->set = set;

  if (bimodal != set.bimodal()) {
    bimodal = set.bimodal();
    updateModel();
  }
}

void
xmmEngineCore::addTrainingSet(xmm::TrainingSet set) {
  int index;

  for (auto &xp : set) {
    if (freeList.size() == 0) {
      index = this->set.size();
    } else {
      index = freeList.back();
      freeList.pop_back();
    }

    if (this->set.size() == 0) {
      if (xp.second->bimodal()) {
        this->set = xmm::TrainingSet(xmm::MemoryMode::OwnMemory,
                                     xmm::Multimodality::Bimodal);
        this->set.dimension_input.set(xp.second->dimension_input.get());
      } else {
        this->set = xmm::TrainingSet();
      }

      this->set.dimension.set(xp.second->dimension.get());
      this->set.column_names.set(xp.second->column_names, true);
    }

    this->set.addPhrase(index, xp.second);
  }

  if (bimodal != this->set.bimodal()) {
    bimodal = this->set.bimodal();
    updateModel();
  }
}

void
xmmEngineCore::clearTrainingSet() {
	set.clear();
}

//============================================================================//

int
xmmEngineCore::getModelType() {
  return modelType;
}

int
xmmEngineCore::getNbOfModels() {
  // TODO : implement a size() method
  return xmm->getNbOfModels();
}

bool
xmmEngineCore::getBimodal() {
  return bimodal;
}

int
xmmEngineCore::getInputDimension() {
  return xmm->getInputDimension();
}

int 
xmmEngineCore::getOutputDimension() {
  return xmm->getOutputDimension();
}

int
xmmEngineCore::getGaussians() {
  return xmm->getGaussians();
}

float
xmmEngineCore::getRelativeRegularization() {
  return xmm->getRelativeRegularization();
}

float
xmmEngineCore::getAbsoluteRegularization() {
  return xmm->getAbsoluteRegularization();
}

int
xmmEngineCore::getCovarianceMode() {
  return xmm->getCovarianceMode();
}

bool
xmmEngineCore::getHierarchical() {
  return xmm->getHierarchical();
}

int
xmmEngineCore::getStates() {
  return xmm->getStates();
}

int
xmmEngineCore::getTransitionMode() {
  return xmm->getTransitionMode();
}

int
xmmEngineCore::getRegressionEstimator() {
  return xmm->getRegressionEstimator();
}

//============================================================================//

void
xmmEngineCore::setModelType(int t) {
  modelTypeE mt = (t == 1) ? hmmModelTypeE : gmmModelTypeE;
  if (mt != modelType) {
    modelType = mt;
    updateModel();
  }
}

void
xmmEngineCore::setGaussians(int g) {
  xmm->setGaussians(g);
}

void
xmmEngineCore::setRelativeRegularization(float relReg) {
  xmm->setRelativeRegularization(relReg);
}

void
xmmEngineCore::setAbsoluteRegularization(float absReg) {
  xmm->setAbsoluteRegularization(absReg);
}

void
xmmEngineCore::setCovarianceMode(int c) {
  xmm->setCovarianceMode(c);
}

void
xmmEngineCore::setHierarchical(bool h) {
  xmm->setHierarchical(h);
}

void
xmmEngineCore::setStates(int s) {
  xmm->setStates(s);
}

void
xmmEngineCore::setTransitionMode(int t) {
  xmm->setTransitionMode(t);
}

void
xmmEngineCore::setRegressionEstimator(int r) {
  xmm->setRegressionEstimator(r);
}

//============================================================================//

int
xmmEngineCore::getLikelihoodWindow() {
  return xmm->getLikelihoodWindow();
}

void
xmmEngineCore::setLikelihoodWindow(int w) {
  xmm->setLikelihoodWindow(w);
}

void
xmmEngineCore::train() {
  xmm->train(&set);
}

bool
xmmEngineCore::isTraining() {
  return xmm->isTraining();
}

void
xmmEngineCore::cancelTraining() {
  xmm->cancelTraining();
}

Json::Value
xmmEngineCore::getModel() {
  return xmm->getModel();
}

// don't do anything for now, as this is not trivial
// better to load a training set on a specific model type and call train
void
xmmEngineCore::setModel(Json::Value jm) {
  // xmm.setModel(jm);
  // update modelType and bimodal according to Json content
}

void
xmmEngineCore::clearModel() {
  xmm->clearModel(); // useless ?
}

void
xmmEngineCore::filter(std::vector<float> const& obs) {
  xmm->filter(obs);
}

std::string
xmmEngineCore::getLikeliestLabel() {
  return xmm->getLikeliestLabel();
}

std::vector<float>
xmmEngineCore::getLikelihoods() {
  return xmm->getLikelihoods();
}

std::vector<float>
xmmEngineCore::getTimeProgressions() {
	return xmm->getTimeProgressions();
}

std::vector<float>
xmmEngineCore::getOutputValues() {
  return xmm->getOutputValues();
}

void
xmmEngineCore::reset() {
  xmm->reset();
}

//============================================================================//

void
xmmEngineCore::updateModel() {
  delete xmm;

  switch (modelType) {
    case gmmModelTypeE:
      xmm = new xmmGmmTool(bimodal);
      break;

    case hmmModelTypeE:
      xmm = new xmmHmmTool(bimodal);
      break;

    default: // should never happen
      break;
  }

  if (set.size() > 0) {
    xmm->train(&set);
  }
}
