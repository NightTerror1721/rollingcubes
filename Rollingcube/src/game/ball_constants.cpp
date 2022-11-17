#include "ball_constants.h"


namespace balls::model
{
	static Model::Ref loadModel()
	{
		Model::Ref ref = ModelManager::root().loadFromFile(name.data(), (resources::models / Path("ball.obj")).string(), true);
		if (!ref)
			logger::fatal("BALL OBJECT MODEL NOT FOUND!!!!!");

		return ref;
	}


	Model::Ref getModel()
	{
		static constinit Model::Ref BallModel = nullptr;

		if (BallModel == nullptr)
			BallModel = loadModel();
		return BallModel;
	}
}
