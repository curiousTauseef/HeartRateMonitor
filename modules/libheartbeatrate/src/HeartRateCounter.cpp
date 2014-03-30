/*
 * HeartRateCounter.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: pavel
 */

#include <limits>

#include <boost/format.hpp>

#include "heartratemonitor/HeartRateTools.h"
#include "heartratemonitor/HeartRateCounter.h"

namespace hrm {

HeartRateCounter::HeartRateCounter(
        IHeartRateGenerator::Ptr hrGenerator,
        IHeartRateNumber::Ptr hrNumber,
        IHeartRateVisualizer::Ptr hrVisualizer):
                _hrg(hrGenerator), _hrn(hrNumber), _hrv(hrVisualizer){
}

HeartRateCounter::~HeartRateCounter() {
    HeartRateTools::instance()->getLog()->DEBUG("HeartRateCounter::~HeartRateCounter()");
    _thread.interrupt();
    _thread.join();
}

bool HeartRateCounter::start() {
    // TODO lambda problem deadlock
    _thread = boost::thread(boost::bind(&HeartRateCounter::threadFunc, this));
    return true;
}

void HeartRateCounter::threadFunc(void) {
    try {
        while(1){
            RawMeasurementGraph hrRawGraph = _hrg->getHeartRate();
            MeasurementGraph hrGraph;
            HeartRateValue hrNumber = 70;

            /* calculate heart rate */

            NormalizedMeasurementValue
                minValue = std::numeric_limits<NormalizedMeasurementValue>().max(),
                maxValue = std::numeric_limits<NormalizedMeasurementValue>().min();
            for(const RawMeasurement& r: hrRawGraph){
                minValue = std::min(r.get<1>(), minValue);
                maxValue = std::max(r.get<1>(), maxValue);
            }

            NormalizedMeasurementValue diff = maxValue - minValue;

            HeartRateTools::instance()->getLog()->DEBUG((boost::format(
                    "min %1%, max %2%, diff %3%") % minValue % maxValue % diff
                    ).str());

            for(const RawMeasurement& r: hrRawGraph){
                hrGraph.push_back(Measurement(r.get<0>(),
                        ((r.get<1>() - minValue) / diff), 0));
            }

            _hrn->setHeartRateNumber(hrNumber);
            _hrv->visualizeHeartRate(hrGraph);

            boost::this_thread::interruption_point();
        }
    } catch (const boost::thread_interrupted& e) {
    }
}

}  // namespace hrm

