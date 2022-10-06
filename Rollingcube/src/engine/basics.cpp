#include "basics.h"


const glm::mat4& Transformable::getModelMatrix() const
{
	if (_modelNeedUpdate)
	{
		glm::mat4 translate = glm::translate(_position);
		glm::mat4 rotate = glm::toMat4(glm::quat(glm::radians(_rotation)));
		glm::mat4 scale = glm::scale(_scale);

		_model = translate * rotate * scale;

		_modelNeedUpdate = false;
	}

	return _model;
}

const glm::mat4& Transformable::getInvertedModelMatrix() const
{
	if (_invertedModelNeedUpdate)
	{
		_invertedModel = glm::inverse(getModelMatrix());
		_invertedModelNeedUpdate = false;
	}

	return _invertedModel;
}
