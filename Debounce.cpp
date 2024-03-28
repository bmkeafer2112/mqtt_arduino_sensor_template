
/* Debounce
 *  This class debounces any input by periodicly reading the input.  It changing the debounced
 *  value after a specified time delay only if the value remains constant.  A user must call the
 *  heartbeat() function at least as fast as the delay time for this class to work.
 *  
 *  Original Version: 1.0.0
 *  Original Author: R. D. MacBride
 *  Modified: C. Sweat 2/14/24
 *  For questions, call (757)503-3257
 */
// Include the DHT11 library for interfacing with the sensor.
#include <DHT11.h>
#include "Arduino.h"
#include "Debounce.h"
//If using DHT11 sensor for humidity and temp. Use Digital pin #12 only.
DHT11 dht11(12);
//Class header
  //pMode is stter for 1==digital, 2==analog, 3==special_case1
Debounce::Debounce(int pin, long delay, int pMode) {
  _pMode = pMode;
  _pin = pin;                //Input pin
  _state = 0;                 //Debounced state of pin
  _timeout = millis();        //Last time the debounced state matched the actual state
  _delay = delay;             //Holding time required to change debounced state
  

}


/* Heartbeat
 *  This function checks for updates on an input.  It will return true if the state changes.
 *  This class must be called at least twice as fast as the debounce delay to prevent nuisance changes.
 *  It functions in a similar maner to a watchdog timer.
 */
bool Debounce::heartbeat(){
  //Obtain current input state
  int newState;
  if (_pMode == 1) {
    newState = digitalRead(_pin);
  }
  else{}
  if (_pMode == 2) {
    newState = analogRead(_pin);
  }
  else{}
  if (_pMode == 3) {
    newState = dht11.readHumidity();
  }
  else{}
  if (_pMode == 4) {
    newState = dht11.readTemperature();
  }
  else{}
  
if(millis() - _timeout > _delay){
    _state = newState;
    return true;
  
  //If the states do not match, but the delay condition is not met, than just return false
  }else{
    return false;
  }
}


/* State
 *  This function provides a safe way of obtaining the debounced input state
 */
int Debounce::state(){

  return _state;
}
