#pragma once

#include <QVariant>

namespace Aero {
	namespace Branding {
		int reportedBuild();
		QVariant custom(const char *key);

		/* The way to use this:
		 *
		 * if (auto v = custom("my-param"); v.isValid())
		 *	   // ...
		 * else if (reportedVersion() < 7000)
		 *	   // ...
		 * else
		 *	   // ...
		 */
	}
}
