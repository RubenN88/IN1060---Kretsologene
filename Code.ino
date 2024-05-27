// ~ KODE FOR INTERVALL-CAPS ~ Laget av Kretsologene for emnet IN1060 ved institutt for informatikk (UiO).
// Touchsensor
// Pulssensor
// Vibrasjonsmotorer

/*
Noe av koden tilknyttet bruk av modulene er inspirert av andres tidligere arbeid. Her kan dere finne kildene våre: 
- Kode tilknyttet vibrasjonsmotoren: http://deepbluembedded.com/arduino-vibration-motor-code-circuit. 
- Kode tilknyttet pulssensoren: https://lastminuteengineers.com/pulse-sensor-arduino-tutorial/. 
- Kode tilknyttet touchsensoren: https://arduinomodules.info/ky-036-metal-touch-sensor-module/. 

NB: Ingenting i koden er faktisk "copy-paste". Vi har skrevet hele dette programmet selv, men ønsker å være åpne om hvor vi fant dokumentasjon, kunnskap og inspirasjon.
*/

// Importerer biblioteket til pulssensoren vår
#include <PulseSensorPlayground.h>

#define USE_ARDUINO_INTERRUPTS true // Anbefalt innstilling for nøyaktig pulsmåling ifølge Last Minute Engineers

// Definerer alle konstante variabler som skal brukes av programmet
const int PIN_TOUCHSENSOR = 7;
const int PIN_PULSSENSOR = A0;
const int PIN_VIBRASJONSMOTOR1 = 2;
const int PIN_VIBRASJONSMOTOR2 = 3;
const int PIN_BRETTLYS = 13; // Brukes for å visualisere puls til debugging situasjoner
const int PULSSENSOR_SENSITIVITET = 510; // Grense for hvilke signaler som skal registreres som faktiske hjerteslag
const int PULSE_GRENSE = 167; // Utregnet ved hjelp av makspulsformelen fra NTNU: 211 - 0,64 x alder

// Definerer alle globale verdier og objekter som skal brukes av programmet
int teller_intervall = 0; // Brukes for å kontrollere hvilket intervall brukeren er på
PulseSensorPlayground pulssensor; // Oppretter er PulseSensorPlayground-objekt som vi kan bruke medfølgende metoder på
bool running = false; // Variabel for å kontrollere kjøring av programmet

/*****************************************************************************************************************************************/
// OPPSETT
void setup() {
    // Starter Serial kommunikasjon med en hastighet på 9600 baud, da dette er anbefalt for pulssensoren vår
    Serial.begin(9600);

    // Oppsett av PulseSensorPlayground-objektet (metodene kommer fra PulseSensorPlayground-biblioteket)
    pulssensor.analogInput(PIN_PULSSENSOR);
    pulssensor.blinkOnPulse(PIN_BRETTLYS);
    pulssensor.setThreshold(PULSSENSOR_SENSITIVITET);

    // Oppsett av input og output PIN-ene
    pinMode(PIN_VIBRASJONSMOTOR1, OUTPUT);
    pinMode(PIN_VIBRASJONSMOTOR2, OUTPUT);
    pinMode(PIN_TOUCHSENSOR, INPUT);

    // Starter opp PulseSensorPlayground-objektet
    pulssensor.begin();
}

// Kjører fram til touchsensoren blir berørt, og resten av programmet skal starte
void touchsensor() {
    int touch_verdi = digitalRead(PIN_TOUCHSENSOR);
    while (touch_verdi == LOW) {
        touch_verdi = digitalRead(PIN_TOUCHSENSOR);
        Serial.println(touch_verdi);
        delay(20); // delay brukes løpende i programmet for å unngå buffer overflyt
    }
    running = true;
}

// Sjekker puls i BPM kontinuerlig, fram til PULSE_GRENSE er nådd og intervall skal startes
void BPM() {
    while (running) {
        int maaltBPM = pulssensor.getBeatsPerMinute();
        delay(20);
        Serial.print("BPM: ");
        Serial.println(maaltBPM);
        if (maaltBPM >= PULSE_GRENSE) {
            break;
        }
        sjekkForStopp();
    }
}

// Utsetter kjøring av programmet for lengden av et intervall gitt som parameter
// Sjekker også for om touchsensoren berøres under kjøring, da programmet i dette tilfellet skal avbrytes
void intervall(unsigned long interval_length) {
    unsigned long end_time = millis() + interval_length;
    while (millis() < end_time && running) {
        sjekkForStopp();
    }
}

// Gjør akkurat det samme som intervall() metoden, men vi syntes det var greit å definere en ny metode da det forbedret lesbarheten til koden
void pause(unsigned long pause_length) {
    unsigned long end_time = millis() + pause_length;
    while (millis() < end_time && running) {
        sjekkForStopp();
    }
}

// Vibrerer vibrasjonsmotoren 3 korte ganger
// Lengden og antall for vibrasjonene er implementert i sanntid med brukerne
void vibrasjonsmotor_3korte() {
    for (int i = 0; i < 3; i++) {
        digitalWrite(PIN_VIBRASJONSMOTOR1, HIGH);
        digitalWrite(PIN_VIBRASJONSMOTOR2, HIGH);
        delay(500);
        digitalWrite(PIN_VIBRASJONSMOTOR1, LOW);
        digitalWrite(PIN_VIBRASJONSMOTOR2, LOW);
        delay(200);
    }
}

// Vibrerer vibrasjonsmotoren 1 lang gang
// Lengden og antall for vibrasjonene er implementert i sanntid med brukerne
void vibrasjonsmotor_1lang() {
    digitalWrite(PIN_VIBRASJONSMOTOR1, HIGH);
    digitalWrite(PIN_VIBRASJONSMOTOR2, HIGH);
    delay(1600);
    digitalWrite(PIN_VIBRASJONSMOTOR1, LOW);
    digitalWrite(PIN_VIBRASJONSMOTOR2, LOW);
}

// Vibrerer vibrasjonsmotoren 2 ekstra korte ganger
// Lengden og antall for vibrasjonene er implementert i sanntid med brukerne
void vibrasjonsmotor_2kortere() {
    for (int i = 0; i < 2; i++) {
        digitalWrite(PIN_VIBRASJONSMOTOR1, HIGH);
        digitalWrite(PIN_VIBRASJONSMOTOR2, HIGH);
        delay(200);
        digitalWrite(PIN_VIBRASJONSMOTOR1, LOW);
        digitalWrite(PIN_VIBRASJONSMOTOR2, LOW);
        delay(100);
    }
}

// Sjekker om bruker har trykket på knappen, og programmet skal stoppes
// Om touchsensoren trykkes på settes running lik false, og hovedprogrammet vil ikke utføre noe mer kode før en ny loop er inisialisert og touchsensor() metoden kjører.
// Bruker println() for debugging situasjoner
void sjekkForStopp() {
    int touch_verdi = digitalRead(PIN_TOUCHSENSOR);
    if (touch_verdi == HIGH) {
        running = false;
        Serial.println("Program terminated by user.");
    }
}

// HOVEDLØKKE
// Her er println() implementert for debugging situasjoner
void loop() {
    if (!running) {
        touchsensor(); // Går i løkke fram til touchsensor er berørt
    }
    vibrasjonsmotor_1lang(); // Gir bruker tilbakemelding om at en intervall-økt er startet
    if (running) {
        delay(3000); // Må ha en delay for å unngå at touchsensoren blir registrert på nytt og terminerer programmet med en gang
        BPM(); // Går i løkke fram til puls over PULSE_GRENSE er målt
        while (teller_intervall < 4 && running) {
            Serial.print("Starter intervall ");
            Serial.println(teller_intervall + 1);
            vibrasjonsmotor_2kortere();
            intervall(240000); // Pauser for lengden av et intervall (4 minutter)
            if (!running) break;
            vibrasjonsmotor_3korte();
            if (teller_intervall < 3 && running) {
                pause(180000); // Pauser for lengden av en pause (3 minutter)
                if (!running) break;
                vibrasjonsmotor_2kortere();
                BPM(); // Sjekker puls kontinuerlig igjen
            } else if (running) {
                vibrasjonsmotor_1lang();
            }
            teller_intervall++;
        }
        teller_intervall = 0; // Resetter intervall_teller for en evt. ny intervall-økt
        Serial.println("Alle intervaller fullført. Klar for ny økt.");
        running = false;
    }
}
