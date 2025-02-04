#ifndef PushButton_h
#define PushButton_h

class PushButton
{
  public:
    PushButton(const int& pin)
      :pin_input(pin)
    {

    };

    void setup_push_button(voidFuncPtr push_button_ISR)
    {
      pinMode(pin_input, INPUT);
      attachInterrupt(digitalPinToInterrupt(pin_input), push_button_ISR, CHANGE);
    };
  
    void state_ready_pending()
    {
      ready_button_was_pressed = true;
    };

    void reset_pending()
    {
      reset_button_was_pressed = true;
    };
    
    bool is_state_ready_pending()
    {
      if (ready_button_was_pressed) 
      {
        ready_button_was_pressed = false;
        return true;
      }
      return false;
    };

    bool is_reset_pending()
    {
      if (reset_button_was_pressed) 
      {
        reset_button_was_pressed = false;
        return true;
      }
      return false;
    };

  private:
    const int pin_input;
    volatile bool ready_button_was_pressed = false;
    volatile bool reset_button_was_pressed = false;
};
#endif