class Button {
private:
  mutable bool previousState = false;
  int pin;

public:
  Button (int pin);

  void  Init ();
  
  bool  GetState () const;
  bool  GetPreviousState () const;
};
