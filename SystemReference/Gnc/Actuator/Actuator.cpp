// ======================================================================
// \title  Actuator.cpp
// \author fprime
// \brief  cpp file for Actuator component implementation class
// ======================================================================


#include <SystemReference/Gnc/Actuator/Actuator.hpp>
#include <FpConfig.hpp>
#include <stdio.h> // for debugging

// SG90 servo by tower pro 
const U32 SG90_PWM_PERIOD = 5000000; 
const U32 SG90_MIDDLE = 1300000; 
const U32 SG90_MAX_ON_TIME = 2200000;
const U32 SG90_MIN_ON_TIME = 500000;


namespace Gnc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  Actuator ::
    Actuator(
        const char *const compName
    ) : ActuatorComponentBase(compName)
  {

  }

  Actuator ::
    ~Actuator()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void Actuator ::
    imuAccelIn_handler(
        const NATIVE_INT_TYPE portNum,
        const Gnc::Vector &imuVector
    )
  {
    // P-controller algorithm 
    error =  desiredPos - imuVector[1]; // -.01 < error < 1

    if( imuVector[0] < 0 ){
      newOnTime = currentOnTime - (gain * error);
      if( newOnTime <= SG90_MIN_ON_TIME ){
        newOnTime = SG90_MIN_ON_TIME;
      }
    }
    if( imuVector[0] > 1 ){
      newOnTime = currentOnTime + (gain * error);
      if( newOnTime >= SG90_MAX_ON_TIME ){
        newOnTime = SG90_MAX_ON_TIME;
    }
    }
    this->pwmSetOnTime_out(0, newOnTime);
    currentOnTime = newOnTime;
    printf( "NEW ON TIME = %d\n", currentOnTime );
    Os::Task::delay(400);
  }

  void Actuator ::
    run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void Actuator ::
    ACTIVATE_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        Fw::On on_off
    )
  {
    this->actuatorIsOn = on_off;
    if(on_off == Fw::On::ON){           // TODO: doesn't come on until off command is sent and on command is sent again
      this->pwmSetPeriod_out(0,SG90_PWM_PERIOD);
      Os::Task::delay(400);
      this->pwmSetOnTime_out(0,SG90_MIDDLE);
      currentOnTime = SG90_MIDDLE; 
      Os::Task::delay(400);
      this-pwmSetEnable_out(0,Fw::Enabled::ENABLED);
    }
    else{
      this-pwmSetEnable_out(0,Fw::Enabled::DISABLED);
    }
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

} // end namespace Gnc
