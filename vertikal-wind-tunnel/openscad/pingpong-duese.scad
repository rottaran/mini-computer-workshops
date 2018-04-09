sensorheight = 20;
sensorwidthbase = 22;
sensorwidthtop = 22;

basewidth = 60;
drilldist = 50;
baseheight = 5;
holedepth = baseheight;

wallwidth = 1.2;

// düsenparameter
height = 50;
openinginner = 28;
tubeinner = 64;
tubewidth = 2.8;

wirewidth = 4;

$fn=60;

module cylinder_outer(height,d){
   fn=16;
   fudge = 1/cos(180/fn);
   cylinder(h=height,r=d/2*fudge,$fn=fn);}

module round_corner_ex(height, rad) {
    translate([-rad,-rad,0])
    difference() {
        cube([rad,rad, height]);
        translate([0,0,-0.1]) cylinder(h=height+0.2, d=2*rad);
    }
}


difference() {
    union() {
        // Düse außen
        cylinder(h=height,d1=tubeinner+2*tubewidth,d2=openinginner+2*tubewidth);
        // Grundplatte
        translate([-basewidth/2,-basewidth/2,0]) cube([basewidth,basewidth,baseheight]);
    }
        
    // Düse innen abziehen
    translate([0,0,-0.05]) cylinder(h=height+0.1,d1=tubeinner,d2=openinginner);

    // Bohrungen abziehen
    translate([-drilldist/2,-drilldist/2,-0.1]) cylinder_outer(holedepth+0.2,4.5);
    translate([-drilldist/2,drilldist/2,-0.1]) cylinder_outer(holedepth+0.2,4.5);
    translate([drilldist/2,-drilldist/2,-0.1]) cylinder_outer(holedepth+0.2,4.5);
    translate([drilldist/2,drilldist/2,-0.1]) cylinder_outer(holedepth+0.2,4.5);

    // Platz für Schraubenkopf abziehen
    translate([-drilldist/2, -drilldist/2, baseheight+0.05]) cylinder(h=5, d1=8,d=8);
    translate([-drilldist/2, drilldist/2, baseheight+0.05]) cylinder(h=5, d1=8,d=8);
    translate([drilldist/2, -drilldist/2, baseheight+0.05]) cylinder(h=5, d1=8,d=8);
    translate([drilldist/2, drilldist/2, baseheight+0.05]) cylinder(h=5, d1=8,d=8);

    
    // Kabeldurchführung abziehen
    translate([0,0,sensorheight+wirewidth/2]) rotate([0,90,0]) cylinder_outer(50,wirewidth);
    
    // Ecken abrunden
     translate([-basewidth/2-0.05, -basewidth/2-0.05, -0.1]) rotate([0,0,180]) round_corner_ex(baseheight+0.2, 5);
    translate([-basewidth/2-0.05, basewidth/2+0.05, -0.1]) rotate([0,0,90]) round_corner_ex(baseheight+0.2, 5);
    translate([basewidth/2+0.05, -basewidth/2-0.05, -0.1]) rotate([0,0,270]) round_corner_ex(baseheight+0.2, 5);
    translate([basewidth/2+0.05, basewidth/2+0.05, -0.1]) rotate([0,0,0]) round_corner_ex(baseheight+0.2, 5);

}

// sensorplatform und strömungsdirektor
intersection() {
    union() {
        // Wände als Halterung des Podests
        translate([-wallwidth/2,-tubeinner/2,0]) cube([wallwidth, tubeinner,sensorheight]);
        translate([-tubeinner/2,-wallwidth/2,0]) cube([tubeinner,wallwidth, sensorheight]);
 
        // Sensorpodest
        cylinder(h=sensorheight,d1=sensorwidthbase, d2=sensorwidthtop);
    }
    // mit Düse Innen schneiden
    cylinder(h=height,d1=tubeinner+tubewidth,d2=openinginner+tubewidth);
}

// zur Kontrolle die Kegel des Sensor-Sichtbereichs
// licht emitter
%color("red",0.3) translate([1,0,sensorheight+1]) cylinder(h=50,d1=0,r2=sin(35)*50);
// licht collector
%color("blue",0.3) translate([-2,0,sensorheight+1]) cylinder(h=60,d1=0,r2=sin(25)*60);

// Schraubenkopf
%color("red", 0.3) translate([-drilldist/2, -drilldist/2, baseheight]) cylinder(h=2.6, d1=7,d=7);
%color("red", 0.3) translate([drilldist/2, -drilldist/2, baseheight]) cylinder(h=2.6, d1=7,d=7);
%color("red", 0.3) translate([-drilldist/2, drilldist/2, baseheight]) cylinder(h=2.6, d1=7,d=7);
%color("red", 0.3) translate([drilldist/2, drilldist/2, baseheight]) cylinder(h=2.6, d1=7,d=7);

//%color("green",0.2) translate([0,0,35]) difference() {
//    cylinder(h=60,d1=50,d2=50);
//    translate([0,0,-0.05]) cylinder(h=60+0.1,d1=44,d2=44);
//}

%color("green",0.2) translate([0,0,36]) {
    cylinder(h=60,d1=44,d2=44);
}

