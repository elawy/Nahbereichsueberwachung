# Nahbereichsüberwachung
Implementierung einer Nahbereichsüberwachung in ein bestehendes Rover-System

Es wurde eine API entwickelt, um in eine bestehende Software integriert werden zu können.

## Verwendete Technik

- Entwicklungsboard STM Nucleo-144 
- Sensor SRF08 von Devantech

## Eckdaten der Sensoren
- I2C Kommunikation
- Ansteuerung von 16 Sensoren über einen Bus
- Einheiten der Messdaten: μs, cm und zoll
- Reichweite: 3cm bis 6m


## API

#### - void changeI2Caddress(uint8_t oldAdrdess, uint8_t newAddress);
notwendige Adressenänderung der Sensoren nach der Lieferung


#### - void obstacleDetection_ctor(struct S_obstacleDetec *this, uint8_t thisSensorAddr[],int size,int range);
Erstellung eines Objekts, das eine Seite des Rovers darstellt.
Übergabeparametern: - Sensoradressen
                    - Anzahl der verwendeten Sensoren
                    - gewünschte Reichweite 
Die Initialisierung muss bei jedem Neustart vorgenommen werden. Da die Reichweite, anders als
die Adressänderung, nur in einem flüchtigen Arbeitsspeicher des Sensors gespeichert wird, ist sie bei jedem Einschalten wieder auf das Maximum gesetzt


#### - void triggerMessuarments(struct S_obstacleDetec *this);
Mit dieser Funktion können die einzelnen Seiten des Rovers einen Messbefehl erhalten. Die
ausgewerteten Daten werden in einem Array gespeichert und können mit der folgenden Funktion
jederzeit abgerufen werden.


#### -void getLastMessurments(struct S_obstacleDetec *this);
Damit letzten Messwerte nicht verloren gehen und weiterhin abgerufen werden
können. Wenn Task plötzlich abgebrochen werden muss, weil ein Task mit einer höheren
Priorität gestartet werden muss.
