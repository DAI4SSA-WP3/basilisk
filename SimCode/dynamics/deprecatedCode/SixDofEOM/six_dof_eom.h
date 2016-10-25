/*
Copyright (c) 2016, Autonomous Vehicle Systems Lab, Univeristy of Colorado at Boulder

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

#ifndef SIX_DOF_EOM_H
#define SIX_DOF_EOM_H

#include <vector>
#include "_GeneralModuleFiles/sys_model.h"
#include "_GeneralModuleFiles/dyn_effector.h"
#include "utilities/sphericalHarmonics.h"
#include "utilities/coeffLoader.h"
#include "utilities/simMacros.h"
#include "dynamics/deprecatedCode/Thrusters/thruster_dynamics.h"
#include "dynamics/deprecatedCode/ReactionWheels/reactionwheel_dynamics.h"
#include "dynamics/deprecatedCode/HingedRigidBodies/hinged_rigid_bodies.h"
#include "dynamics/deprecatedCode/FuelTank/fuel_tank.h"
#include "dynamics/deprecatedCode/_GeneralModuleFiles/dynObject.h"
#include "dynamics/deprecatedCode/_GeneralModuleFiles/integrator.h"
#include "dynamics/deprecatedCode/_GeneralModuleFiles/rk4Integrator.h"
/*! \addtogroup SimModelGroup
 * @{
 */

//!@brief Container for gravitational body data
/*! This class is designed to hold all of the information for a gravity 
    body.  The nominal use-case has it initialized at the python level and 
    attached to dynamics using the AddGravityBody method.
*/
class GravityBodyData
{
public:
    // TO-DO: modified by MANUEL.
    // This class used to be a structure. For backwards compatibility, all attributes will remain PUBLIC.
    // New attributes are added as Private.
    bool IsCentralBody;             //!<          Flag indicating that object is center
    bool IsDisplayBody;             //!<          Flag indicating that body is display
    bool UseJParams;                //!<          Flag indicating to use perturbations
    bool UseSphericalHarmParams;    //!<          Flag indicating to use spherical harmonics perturbations
    std::vector<double> JParams;    //!<          J perturbations to include
    double PosFromEphem[3];         //!< [m]      Position vector from central to body
    double VelFromEphem[3];         //!< [m/s]    Velocity vector from central body
    double J20002Pfix[3][3];        //!<          Transformation matrix from J2000 to planet-fixed
    double J20002Pfix_dot[3][3];    //!<          Derivative of the transformation matrix from J2000 to planet-fixed
    double posRelDisplay[3];        //!< [m]      Position of planet relative to display frame
    double velRelDisplay[3];        //!< [m]      Velocity of planet relative to display frame
    double mu;                      //!< [m3/s^2] central body gravitational param
    double ephemTime;               //!< [s]      Ephemeris time for the body in question
    double ephIntTime;              //!< [s]      Integration time associated with the ephem data
    double radEquator;              //!< [m]      Equatorial radius for the body
    uint64_t ephemTimeSimNanos;     //!< [ns]     Simulation nanoseconds associated with Ephemeris time
    std::string BodyMsgName;        //!<          Gravitational body name
    std::string outputMsgName;      //!<          Ephemeris information relative to display frame
    std::string planetEphemName;    //!<          Ephemeris name for the planet
    int64_t outputMsgID;            //!<          ID for output message data
    int64_t BodyMsgID;              //!<          ID for ephemeris data message
    
    // Default constructor
    GravityBodyData();
    
    // Constructor to be used for creating bodies with a spherical harmonic model
    GravityBodyData(const std::string& sphHarm_filename, const unsigned int max_degree, const double mu_in, const double reference_radius);
    virtual ~GravityBodyData();
    
    // Copy constructor
    GravityBodyData(const GravityBodyData& gravBody);
    
    // Overloaded operators
    GravityBodyData& operator=(const GravityBodyData& gravBody);
    
    sphericalHarmonics* getSphericalHarmonicsModel(void);
private:
    sphericalHarmonics* _spherHarm;     //!<          Object that computes the spherical harmonics gravity field
    coeffLoaderCSV* _coeff_loader;      //!<          Object that loads the coefficients
};

//!@brief The SixDofEOM class is used to handle all dynamics propagation for a spacecraft
/*! It is designed to handle all gravitational effects and unforced attitude 
    propagation internally.  All non-conservative effects are designed to be 
    handled by the DynEffector class and attached to dynamics through the 
    AddBodyEffector call.
*/
class SixDofEOM: public SysModel, public dynObject {
public:
    SixDofEOM();
    ~SixDofEOM();
    
    void SelfInit();
    void CrossInit();
    void UpdateState(uint64_t CurrentSimNanos);
    void ReadInputs();
    virtual void equationsOfMotion(double t, double *X, double *dX);
    void integrateState(double CurrentTime);
    void computeOutputs();
    void AddGravityBody(GravityBodyData *NewBody);
    void WriteOutputMessages(uint64_t CurrentClock);
    void addThrusterSet(ThrusterDynamics *newThrusterSet);
	void addReactionWheelSet(ReactionWheelDynamics *newReactionWheelSet);
    void addHingedRigidBodySet(HingedRigidBodies *newHingedRigidEffector);
    void addBodyEffector(DynEffector *newBodyEffector);
    void addFuelTank(FuelTank *newFuelTank);
    void setIntegrator(integrator *NewIntegrator);
    void initPlanetStateMessages();
    void jPerturb(GravityBodyData *gravBody, double r_N[3], double perturbAccel[3]);
    void computeGravity(double t, double r_BN_N[3], double BN[3][3], double *g_N);
    void computeCompositeProperties();
public:
    std::vector<double> PositionInit; //!< [m]   Initial position (inertial)
    std::vector<double> VelocityInit; //!< [m/s] Initial velocity (inertial)
    std::vector<double> AttitudeInit; //!<       Inertial relative MRPs for attitude
    std::vector<double> AttRateInit;  //!< [r/s] Inertial relative body rate
    std::vector<double> baseInertiaInit;//!< [kgm2] Inertia tensor at init (dry)
    std::vector<double> baseCoMInit;  //!< [m]   Initial center of mass in structure (dry)
    std::vector<double> T_Str2BdyInit;//!< []    Initial (perm) structure to bdy rotation
    double ibaseMassInit;             //!< [kg]  Initial mass of vehicle (dry)
    
    std::string outputStateMessage;   //!<       Output state data
    std::string outputMassPropsMsg;   //!<       Output mass properties
    std::string centralBodyOutMsgName;//!<       Output central body
    uint64_t OutputBufferCount;
    std::vector<GravityBodyData> gravData; //!<  Central body grav information
    GravityBodyData* centralBody;         //!<  Central body
    bool messagesLinked;              //!<       Indicator for whether inputs bound
    uint64_t RWACount;                //!<        Number of reaction wheels to model
    uint64_t numRWJitter;             //!<        Number of reaction wheels that are modeling jitter
    uint64_t numHRB;                  //!<        Number of hinged rigid bodies to model
    uint64_t numFSP;                  //!<        Number of fuel slosh particles
    double baseCoM[3];                //!< [m]    center of mass of dry spacecraft str
    double baseI[3][3];               //!< [kgm2] Inertia tensor for base spacecraft str
    double baseMass;                  //!< [kg]   Mass of dry spacecraft structure
    double compCoM[3];                //!< [m]    Center of mass of spacecraft in str
    double compI[3][3];               //!< [kgm2] Inertia tensor for vehicle
    double compIinv[3][3];            //!< [m2/kg] inverse of inertia tensor
    double compMass;                  //!< [kg]   Mass of the vehicle
    double timePrev;                  //!< [s]    Previous update time
    double r_BN_N[3];                 //!< [m]    Current position vector (inertial)
    double v_BN_N[3];                 //!< [m/s]  Current velocity vector (inertial)
    double sigma_BN[3];               //!<        Current MRPs (inertial)
    double omega_BN_B[3];             //!< [r/s]  Current angular velocity (inertial)
    double InertialAccels[3];         //!< [m/s2] Current calculated inertial accels
    double NonConservAccelBdy[3];     //!< [m/s2] Observed non-conservative body accel
    double ConservAccelBdy[3];        //!< [m/s2] Observed conservative body accel
    double T_str2Bdy[3][3];           //!<        Structure to body DCM matrix
    double AccumDVBdy[3];             //!< [m/s]  Accumulated DV in body
    double rwaGyroTorqueBdy[3];       //!<
    uint64_t MRPSwitchCount;          //!<        Count on times we've shadowed
    double totScOrbitalEnergy;        //!< [J]    Total orbital energy of spacecraft
    double totScOrbitalAngMom_N[3];   //!< [kg-m^2/s] Total orbital angular momentum of the spacecraft in inertial frame components
    double totScOrbitalAngMomMag;     //!< [kg-m^2/s] Magnitude of total orbital angular momentum of the spacecraft
    double totScRotEnergy;            //!< [J]    Total spacecraft energy about its center of mass
    double totScRotAngMom_N[3];       //!< [kg-m^2/s] Total angular momentum of the spacecraft about its center of mass in N frame
    double totScRotAngMomMag;         //!< [kg-m^2/s] Magnitude of total angular momentum of the spacecraft about its center of mass
    double scRotPower;                //!< [W] Mechanical Power of the spacecraft rotational motion (analytical work-energy theorem)
    double scRotEnergyRate;           //!< [W] Rate of change of energy to check with power (numerically evaluatated power)
    bool   useTranslation;            //!<        Flag indicating to use translation dynamics
    bool   useRotation;               //!<        Flag indicating to use rotational dynamics
    bool   useGravity;                //!<        Flag indicating to use gravity in dynamics 
    std::vector<HingedRigidBodies *> hingedRigidBodies; //!< (-) Vector of hinged rigid bodies in body
    std::vector<FuelTank *> fuelTanks;          //!< (-) Vector of fuel tank
    std::vector<DynEffector *> bodyEffectors;   //!< (-) Vector of one way coupled body effectors
    
private:
    double *XState;                   //!<        Container for total state
    int64_t StateOutMsgID;            //!<        Output message id for state data
    int64_t MassPropsMsgID;           //!<        Output message id for state data
    int64_t centralBodyOutMsgId;
    uint32_t NStates;                 //!<        Count on states available
    //std::vector<DynEffector*> BodyEffectors;  //!<  Vector of effectors on body
    std::vector<ThrusterDynamics *> thrusters; //!< (-) Vector of thrusters in body
	std::vector<ReactionWheelConfigData *> reactWheels; //!< (-) Vector of RW in body
    integrator* Integrator;            //!<          Integrator used to integrate the EOM
    bool DefaultIntegrator;
};

/*! @} */

#endif