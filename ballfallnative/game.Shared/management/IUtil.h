#pragma once

/// The interface of the OS specific utility classes.
class IUtil {
	virtual void Log (const string& log) = 0;
};
