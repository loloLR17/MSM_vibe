// REVIEW ONLY; mm. Opening toward nearest Zio; rotated180deg at top anchor.
H=12.25; R=3; bore=2.4;
$fn=96; difference(){cylinder(h=H,r=R);translate([0,0,-.1])cylinder(h=H+.2,d=bore);translate([-5,0,-1])cube([10,10,H+2]);}
