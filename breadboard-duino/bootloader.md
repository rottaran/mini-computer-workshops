# Den Ardunino Bootloader auf den Mini-Computer bringen

Um unseren neu gebauten Mini-Computer tatsächlich benutzen zu können, brauchen wir eine Methode, um unsere Programme in seinen Speicher schreiben zu können. Die offizielle Schnittstelle dafür (ISP In-System-Programmer) benutzt die Beine RST (Reset), D11 (MOSI Master-Out-Slave-In), D12 (MISO Master-In-Slave-Out), D13 (SCK Serial Clock) zusammen mit einem passenden Programmiergerät. Das ist auf Dauer unschön, weil wir die Beine auch für andere Zwecke benutzen wollen und das Programmiergerät nicht unbedingt klein und billig ist.

Die Lösung des Arduino-Projektes ist ein kleines Betriebssystem, "Bootloader" genannt, das neue Programme über die Serielle Schnittstelle des USB-Adapters empfängt und in den Speicher des Micro-Controllers schreibt. Ist dieser Bootloader einmal auf dem Mini-Computer installiert, brauchen wir das Programmiergerät nicht mehr.

Im folgenden werden zwei Möglichkeiten zum Installieren des Bootloaders vorgestellt. Die erste Variante benutzt einen funktionierenden Arduino Uno, Arduino Mini, Arduino Nano oder Steckbrett-Duino als Programmiergerät. Die zweite Variante benutzt den "AVR ISP mkII" von Atmel. Es gibt noch weitere, auch preisgünstige Programmiergeräte. Aber ein Arduino(-Nachbau) reicht aus.


## Variante 1: Arduino/Brotduino mit ArduinoISP

Es wird die Arduino-IDE Entwicklungsumgebung von http://arduino.cc/en/Main/Software gebraucht. Zudem muss unter MacOS und Windows ein Treiber für den CP2102 USB-Adapter installiert werden und danach der Computer neu gestartet werden. Die Webseite mit dem offiziellen Treiber ist: TODO

Im Menü "Datei->Beispiele->ArduinoISP->ArduinoISP" öffnet das Programm das für das Programmiergerät gebraucht wird. Dieses auf den funktionierenden Arduino oder Steckbrett-Duino hochladen. Dann mit flexiblen Drahtbrücken wie weiter unten beschrieben die Verbindung zum neuen Steckbrett-Duino herstellen. Nun kann im Menü "Werkzeuge->Bootloader brennen" gewählt werden, um den Bootloader über das Arduino-Programmiergerät auf den Steckbrett-Duino zu installieren.

Zur Verkabelung:
* den USB-Adapter aus dem Ziel-Steckbrett-Duino entfernen. Stattdessen versorgen wir unseren Steckbrett-Duino vom Programmier-Arduino aus:
  * GND (- der Stromversorgung) vom Programmier-Arduino zur - Strom-Spalte des Ziel-Steckbrett-Duino verbinden
  * VCC (5V oder + der Stromversorgung) vom Programmier-Arduino zur + Strom-Spalte des Ziel-Steckbrett-Duino verbinden
* das Bein D13 bzw. SCK vom Programmier-Arduino zum Bein D13 (SCK) des Ziel-Steckbrett-Duino verbinden
* das Bein D12 bzw. MISO vom Programmier-Arduino zum Bein D12 (MISO) des Ziel-Steckbrett-Duino verbinden
* das Bein D11 bzw. MOSI vom Programmier-Arduino zum Bein D11 (MOSI) des Ziel-Steckbrett-Duino verbinden
* das Bein D10 vom Programmier-Arduino zum Bein RST (Reset) des Ziel-Steckbrett-Duino verbinden
* am Programmier-Arduino eine Verbindung von der + Spalte zum RST-Bein bzw. vom VCC-Anschluss zum RST Anschluss.

Die Drahtverbindung von +/VCC zu RST am Programmier-Arduino ist notwendig, um den automatischen Neustart beim öffnen der Seriellen Verbindung über das USB-Kabel zu unterbinden. Beim installieren des Bootloaders wird von der Arduino-Umgebung ein Programm namens "avrdude" benutzt. Dieses baut mehrmals hintereinander eine neue Verbindung zum Programmiergerät auf, um verschiedene Teile des Bootloaders zu übertragen. Normalerweise würde das unseren ArduinoISP jedes mal neu starten und dabei durcheinander bringen. Um dies zu verhindern, wird RST direkt mit 5V verbunden, so dass der Kondensator zum DTR-Bein des USB-Adapters im Programmier-Arduino wirkungslos wird.

Bei einem Steckbrett-Duino als Programmier-Gerät kann alternativ vorübergehend der Kondensator zwischen DTR-Bein des USB-Adapters und RST-Bein des Micro-Controllers entfernt werden.

Hinweis: Am Programmier-Arduino können drei Leuchtdioden mit Vorwiderständen (ca. 180 bis 330 Ohm) angebracht werden, um den Status des Programmier-Arduino zu sehen. Die Anschlüsse sind wie folgt:
* Bein D9: Herzschlag, pulsiert wenn der Programmier-Arduino funktioniert und läuft
* Bein D8: Fehler, leuchtet wenn ein Problem aufgetreten ist
* Bein D7: Programmieren, leuchtet wenn Daten an den Ziel-Steckbrett-Duino übertragen werden.


TODO überprüfen ob MISO und MOSI doch gekreuzt werden müssen

TODO Zeichnung und Foto der Verkabelung, Screenshots der Menu-Punkte

TODO geht auch mit Arduino-Nano? Da ist das RST-Unterdrücken nicht notwendig?



### Literatur im Internet
* http://arduino.cc/en/Tutorial/ArduinoToBreadboard
* http://arduino.cc/en/Main/Standalone


## Variante 2: Programmiergerät "AVR ISP mkII"
