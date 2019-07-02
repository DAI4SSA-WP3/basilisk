/*
 ISC License

 Copyright (c) 2016, Autonomous Vehicle Systems Lab, University of Colorado at Boulder

 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.

 THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

 */

#ifndef planetEphemeris_H
#define planetEphemeris_H

#include <vector>
#include "_GeneralModuleFiles/sys_model.h"
#include "simMessages/spicePlanetStateSimMsg.h"
#include "utilities/linearAlgebra.h"
#include "utilities/orbitalMotion.h"
#include <Eigen/Dense>

/*! \addtogroup SimModelGroup
 * @{
 */


/*! @brief The planetEphemeris module uses classical heliocentric orbit elements to specify a planet's
 position and velocity vectors.

 Optionally, the planet's orientation can also be specified through a right ascension angle, a declination angle, and a location sidereal time at epoch.  The planet rotation about its third (polar) axis can be specified as well.  If the planet attitude information is not complete or missing then an inertially fixed zero planet orientation is modeled.  The module is able to receive a stack or vector of classical orbit elements and orientation information to output a series of planet ephemeris messages. The module [PDF Description](Basilisk-planetEphemeris-20190422.pdf) contains further information on this module's function, how to run it, as well as testing.
*/

class PlanetEphemeris: public SysModel {
public:
    PlanetEphemeris();
    ~PlanetEphemeris();
    
    void SelfInit();
    void Reset(uint64_t CurrentSimNanos);
    void UpdateState(uint64_t CurrentSimNanos);
    
public:
    uint64_t outputBufferCount;                 //!< -- Number of output buffers to use
    std::vector<std::string>planetNames;        //!< -- Array of planet names
    std::vector<classicElements>planetElements; //!< -- Array of planet classical orbit elements

    std::vector<double> rightAscension;         //!< [r] right ascension of the north pole rotation axis (3-axis)
    std::vector<double> declination;            //!< [r] Declination of the north pole rotation axis (neg. 2-axis)
    std::vector<double> lst0;                   //!< [r] initial planet local sidereal time angle

    std::vector<double> rotRate;                //!< [r/s] planet rotation rate

private:
    std::vector<std::uint64_t>planetOutMsgId;   //!< -- array of output message IDs
    double epochTime;                           //!< [s] time of provided planet ephemeris epoch
    int computeAttitudeFlag;                    //!< -- flag indicating if the planet orienation information is provided
    std::vector<Eigen::Vector3d> eHat_N;        //!< -- planet north pole rotation axis
};

/*! @} */

#endif