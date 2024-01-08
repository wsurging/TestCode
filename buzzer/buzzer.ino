int buzzer = 12;
unsigned char currentIdx;
#define MELODY_LENGTH 95
#define C(x) (0 + (x)*12)
#define C_SHARP(x) (1 + (x)*12)
#define D_FLAT(x) (1 + (x)*12)
#define D(x) (2 + (x)*12)
#define D_SHARP(x) (3 + (x)*12)
#define E_FLAT(x) (3 + (x)*12)
#define E(x) (4 + (x)*12)
#define F(x) (5 + (x)*12)
#define F_SHARP(x) (6 + (x)*12)
#define G_FLAT(x) (6 + (x)*12)
#define G(x) (7 + (x)*12)
#define G_SHARP(x) (8 + (x)*12)
#define A_FLAT(x) (8 + (x)*12)
#define A(x) (9 + (x)*12)
#define A_SHARP(x) (10 + (x)*12)
#define B_FLAT(x) (10 + (x)*12)
#define B(x) (11 + (x)*12)
#define SILENT_NOTE 0xFF
unsigned char note[MELODY_LENGTH] = {
  E(5), SILENT_NOTE, E(5), SILENT_NOTE, E(5), SILENT_NOTE, C(5), E(5),
  G(5), SILENT_NOTE, G(4), SILENT_NOTE,

  C(5), G(4), SILENT_NOTE, E(4), A(4), B(4), B_FLAT(4), A(4), G(4),
  E(5), G(5), A(5), F(5), G(5), SILENT_NOTE, E(5), C(5), D(5), B(4),

  C(5), G(4), SILENT_NOTE, E(4), A(4), B(4), B_FLAT(4), A(4), G(4),
  E(5), G(5), A(5), F(5), G(5), SILENT_NOTE, E(5), C(5), D(5), B(4),

  SILENT_NOTE, G(5), F_SHARP(5), F(5), D_SHARP(5), E(5), SILENT_NOTE,
  G_SHARP(4), A(4), C(5), SILENT_NOTE, A(4), C(5), D(5),

  SILENT_NOTE, G(5), F_SHARP(5), F(5), D_SHARP(5), E(5), SILENT_NOTE,
  C(6), SILENT_NOTE, C(6), SILENT_NOTE, C(6),

  SILENT_NOTE, G(5), F_SHARP(5), F(5), D_SHARP(5), E(5), SILENT_NOTE,
  G_SHARP(4), A(4), C(5), SILENT_NOTE, A(4), C(5), D(5),

  SILENT_NOTE, E_FLAT(5), SILENT_NOTE, D(5), C(5)
};

unsigned int duration[MELODY_LENGTH] = {
  100, 25, 125, 125, 125, 125, 125, 250, 250, 250, 250, 250,

  375, 125, 250, 375, 250, 250, 125, 250, 167, 167, 167, 250, 125, 125,
  125, 250, 125, 125, 375,

  375, 125, 250, 375, 250, 250, 125, 250, 167, 167, 167, 250, 125, 125,
  125, 250, 125, 125, 375,

  250, 125, 125, 125, 250, 125, 125, 125, 125, 125, 125, 125, 125, 125,

  250, 125, 125, 125, 250, 125, 125, 200, 50, 100, 25, 500,

  250, 125, 125, 125, 250, 125, 125, 125, 125, 125, 125, 125, 125, 125,

  250, 250, 125, 375, 500
};

void setup() {
  // put your setup code here, to run once:
  pinMode(buzzer, OUTPUT);
  currentIdx = 0;
  Serial.begin(9600);
  Serial.println("music!");
}

void loop() {
  for (int thisNote = 0; thisNote < 95; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

    int noteDuration = 3000 / duration[thisNote];
    tone(buzzer, note[thisNote], duration[thisNote]);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzer);
  }
}
