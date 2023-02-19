const int POT=0;
int val = 0;

const int BUTTON=2;
boolean lastButton = LOW;
boolean currentButton = LOW;
const int BLED=9;
const int GLED=10;
const int RLED=11;

int selectedColor = BLED;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT);
  pinMode(BLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(RLED, OUTPUT);
}

boolean debounce(boolean last)
{
  boolean current = digitalRead(BUTTON);
  if (last != current)
  {
    delay(5);
    current = digitalRead(BUTTON);
    return current;
  }
}

void changeColor(int color, int value)
{
  analogWrite(color, value/4);
}

void loop() {
  val = analogRead(POT);
  // Serial.println(val);
  currentButton = debounce(lastButton);
  if (lastButton == LOW && currentButton == HIGH)
  {
    selectedColor++;
  }
  lastButton = currentButton;
  if (selectedColor > 11)
  {
    selectedColor = 9;    
  }
  changeColor(selectedColor, val);
}
