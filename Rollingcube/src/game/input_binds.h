#pragma once

#include "engine/input/input.h"


namespace input
{
	namespace bind
	{
		static inline const InputBind FreecamForward = InputBind::make();
		static inline const InputBind FreecamBackward = InputBind::make();
		static inline const InputBind FreecamLeft = InputBind::make();
		static inline const InputBind FreecamRight = InputBind::make();
	}

	inline bool registerInputBind(InputBind bindId, const InputUniqueId& inputId)
	{
		return InputBindManager::registerBind(inputId, bindId);
	}
}
