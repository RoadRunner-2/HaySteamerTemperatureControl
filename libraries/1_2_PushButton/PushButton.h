#ifndef PushButton_h
#define PushButton_h

volatile bool was_button_pressed = false;
void push_button_is_pressed() // IRQ
{
    was_button_pressed = true;
}

class PushButton
{
  public:
    PushButton(const int& pin)
      :pin_input(pin)
    {

    };

    void setup_push_button()
    {
      pinMode(pin_input, INPUT);
      attachInterrupt(digitalPinToInterrupt(pin_input), push_button_is_pressed, CHANGE);
    };

    bool is_pressed()
    {
      if (was_button_pressed)
      {
        was_button_pressed = false;
        return true;
      }
      return false;
	};

  private:
    const int pin_input;
};
#endif