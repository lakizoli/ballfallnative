#pragma once

#include "management/IUtil.h"

class AndroidUtil : public IUtil {
	virtual void Log (const string& log) override;
};
