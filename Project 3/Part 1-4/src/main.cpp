#include <Arduino.h>
#include "Encoder.h"
#include "digital_out.h"
#include <avr/interrupt.h>
#include <pi_control.h>
#include <timer_msec.h>
#include <timer0_msec.h>

double setpoint = 2000.0; // Target speed in PPS
Digital_out LED(5);
Encoder encoder(0, 2); 
Digital_out Aout1(1);
Digital_out sleep(2);
Digital_in FLTpin(3, 'B');
Timer0_msec timer0;
Timer_msec timer;
PI_Control *control;

int counter = 0; 
int array1[100]; // Array to store actual speed values
int array2[100]; // Array to store PWM values


class Context; // Forward declaration to allow the definition of a pointer to Context without compile error

enum StateID {INIT, PRE_OP, OPERATIONAL, STOPPED};

class State
{
  /**
   * @var Context
   */

protected:
  Context *context_;

public:
  virtual ~State()
  {
  }

  void set_context(Context *context)
  {
    this->context_ = context;
  }

  virtual void on_entry() = 0;
  virtual void on_exit() = 0;
};

/**
 * The Context defines the interface of interest to clients. It also maintains a
 * reference to an instance of a State subclass, which represents the current
 * state of the Context.
 */

class Context
{
  /**
   * @var State A reference to the current state of the Context.
   */

private:
  State *state_;
  StateID currentStateID;

public:
  Context(State *state) : state_(nullptr)
  {
    this->transition_to(state, INIT);
  }

  ~Context()
  {
    delete state_;
  }

  /**
   * The Context allows changing the State object at runtime.
   */

  void transition_to(State *state, StateID newStateID)
  {
    if (newStateID != currentStateID) // Only transition if the new state is different
    {
      if (this->state_ != nullptr)
      {
        this->state_->on_exit();
        delete this->state_;
      }

      this->state_ = state;
      this->currentStateID = newStateID;
      this->state_->set_context(this);
      this->state_->on_entry();
    }
    else
    {
      Serial.println("Already in the requested state, no transition made.");
    }
  }

  /**
   * The Context delegates part of its behavior to the current State object.
   */
};

/**
 * Concrete States implement various behaviors, associated with a state of the
 * Context.
 */

class Initialization : public State
{
public:
  void on_entry() override;
  void on_exit() override;
};

class Pre_Operational : public State
{
public:
  void on_entry() override;
  void on_exit() override;
};

class Operational : public State
{
public:
  void on_entry() override;
  void on_exit() override;
};

class Stopped : public State
{
public:
  void on_entry() override;
  void on_exit() override;
};

void Initialization::on_entry()
{
  Serial.println("Initializing system");
  LED.init();
  LED.set_lo();
  Aout1.init();
  sleep.init();
  sleep.set_lo();
  FLTpin.init();
  this->context_->transition_to(new Pre_Operational, PRE_OP);
}

void Initialization::on_exit()
{
}

void Pre_Operational::on_entry()
{
  Serial.println("Entering Pre Operational stage");
  Serial.println("What should be value of Kp?");
  float Kp = 0.0;
  float Ti = 0.0;
  while (true) {
    if (Serial.available()){
      String data = Serial.readStringUntil('\n');
      Kp = data.toFloat();
      break;
    }
  }
  Serial.println("What should be value of Ti?");
  while (true) {
    if (Serial.available()){
      String data = Serial.readStringUntil('\n');
      Ti = data.toFloat();
      break;
    }
  }
  control = new PI_Control(Kp, Ti, 0.5, setpoint + 1000); // Kp, Ti, T, saturation_limit
  Serial.println("The system is ready to receive a command!");
  sleep.set_lo();
 
  
  while (!Serial.available())
  {
    LED.toggle();
    _delay_ms(1000);
  }
}

void Pre_Operational::on_exit()
{
  Serial.println("Exeting Pre Operational stage");
  LED.set_lo();
}

void Operational::on_entry()
{
    Serial.println("Entering Operational stage");
    LED.set_hi(); 
    sleep.set_hi();
    timer0.init(4); 
    timer.init(2); 
    
    while (!Serial.available())
    {
      //Serial.print(FLTpin.is_hi());
        if (!FLTpin.is_hi()) {
            this->context_->transition_to(new Stopped, STOPPED);
            return;
        }
    }
}
void print(){
    for (int i = 0; i < 100; i++){
       // Serial.print("Reference value is: ");
        //Serial.println(setpoint);
        Serial.print("Actual value is: ");
        Serial.println(array1[i]);
        Serial.print("PWM value is: ");
        Serial.println(array2[i]);
        array1[i] = 0; //reset array
        array2[i] = 0; //reset array
    }

}
void Operational::on_exit()
{
  Serial.println("Exeting Operational stage");
  LED.set_lo();
  Aout1.set_lo();
  sleep.set_lo();
  // print();
  // counter = 0;
}

void Stopped::on_entry()
{
  Serial.println("Entering a stopped state, enter a command to continue");
  while (!Serial.available())
  {
    LED.toggle();
    _delay_ms(500);
  }
}

void Stopped::on_exit()
{
  Serial.println("exiting a stopped state");
  LED.set_lo();
}

Context *context;


void setup()
{
  Serial.begin(57600);
  context = new Context(new Initialization);
}

void loop()
{
  int command = 0;
  delay(100);

  if (Serial.available())
  {
    command = Serial.read();
    switch (command)
    {
    case 'p':
      context->transition_to(new Pre_Operational, PRE_OP);
      break;
    case 'o':
      context->transition_to(new Operational, OPERATIONAL);
      break;
    case 'r':
      context->transition_to(new Initialization, INIT);
      break;
    }
  }
}

  
// ISR for encoder
ISR(INT0_vect)
{
    if (encoder.pin2.is_hi()) 
    {
        if (encoder.pin1.is_hi()) 
        {
            encoder.count++;
        }
        else 
        {
            encoder.count--;
        }
    }
    
    else
    {
        if (encoder.pin1.is_lo())
        {
            encoder.count++;
        }
        else
        {
            encoder.count--;
        }
    }

}

ISR(TIMER1_COMPA_vect)
{
    Aout1.set_hi();
}

ISR(TIMER1_COMPB_vect)
{
    Aout1.set_lo();
}


ISR(TIMER0_COMPA_vect)
{ 
    // This ISR executes every 4ms to update control and store data
    int pwm = control->update(setpoint, encoder.count*250);
    if (counter < 100){
        array1[counter] = (encoder.count/0.004);
        array2[counter] = pwm;
        counter ++;
    if (counter == 100){
      print();
    }
    encoder.count = 0;
}
}

