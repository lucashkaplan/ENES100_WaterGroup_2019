#include "Enes100.h"
#include "Tank.h"

int markerid = 5;
int wifitx = 2, wifirx = 3; 

void setup() {
  //Team Name, Mission Type, Marker ID, TX Pin, RX Pin
  Enes100.begin("Water Those", WATER, markerid, wifitx, wifirx);
   
	Enes100.print("Destination is at (");
  Enes100.print(Enes100.destination.x);
  Enes100.print(", ");
  Enes100.print(Enes100.destination.y);
  Enes100.println(")");	
}

void loop() {
	 if (Enes100.updateLocation()) {
        Enes100.print("OSV Location: (");
        Enes100.print(Enes100.location.x);
        Enes100.print(", ");
        Enes100.print(Enes100.location.y);
        Enes100.print(", ");
        Enes100.print(Enes100.location.theta);
        Enes100.println(")");
        delay(500);
    } 
    else {
        // OSV's location was not found
        Enes100.println("Error 404! OSV Not Found!");
    }	
}
