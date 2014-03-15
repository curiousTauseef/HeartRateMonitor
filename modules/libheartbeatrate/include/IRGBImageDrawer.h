/*
 * IRGBImageDrawer.h
 *
 *  Created on: Mar 15, 2014
 *      Author: ingener
 */

#ifndef IRGBIMAGEDRAWER_H_
#define IRGBIMAGEDRAWER_H_

#include <boost/smart_ptr.hpp>

#include "HeartBeatRateTypes.h"

namespace hrm {

class IRGBImageDrawer {
public:
	typedef boost::shared_ptr<IRGBImageDrawer> Ptr;

	virtual ~IRGBImageDrawer() {
	}

	virtual void drawImage(RGBImage image) = 0;
protected:
	IRGBImageDrawer() {
	}
};

}  // namespace hrm

#endif /* IRGBIMAGEDRAWER_H_ */
