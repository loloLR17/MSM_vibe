// MSM TR2 shield V1.4 — REVIEW ONLY, NO FABRICATION RELEASE.
// Coordinates and contact surfaces are nominal hypotheses, not measured C05/MKI geometry.
// Caller selects part="mki", "click", "flat_clamp", "round_clamp", "user_plunger".
// Modules remain replaceable after removing two insulating through-bolts.
// Automatic keying is waived for these two prototypes; bolted positive retention remains.
part="mki";
review_only=true;
assert(review_only,"Fabrication blocked: mechanical fit and materials need review");
$fn=48;
module_bottom=11; pcb_t=1.6; contact_z=module_bottom+pcb_t;
roof_z=22; roof_t=3; foot_r=3;
mki_anchors=[[39,20],[71,54.5]];
click_anchors=[[91,75],[94.5,112.5]];
// MKI body below is the explicitly provisional envelope from V1, not a ST dimension.
mki_body=[42.46,21.46,62.78,54.48];
module bore(p,d,h=30){translate([p[0],p[1],-1])cylinder(d=d,h=h);}
module beam(points,z,t,r=3){hull()for(p=points)translate([p[0],p[1],z])cylinder(r=r,h=t);}
module support(p){translate([p[0],p[1],0])cylinder(r=foot_r,h=roof_z+roof_t);}
module mark(p,label){translate([p[0],p[1],roof_z+roof_t-.45])linear_extrude(1)text(label,size=2,halign="center",valign="center");}
module mki(){
 difference(){
  union(){
   for(p=mki_anchors)support(p);
   beam(mki_anchors,roof_z,roof_t);
   // Broad roof is above assumed component envelope; edge pads alone set the nominal contact height.
   for(p=[[43.1,24.3],[62.1,44.9]])translate([p[0]-.5,p[1]-1,contact_z])cube([1,2,roof_z-contact_z+.1]);
  }
  bore(mki_anchors[0],2.8);bore(mki_anchors[1],3.4);
  mark([55,37],"DRAFT");
 }
}
module click(){
 difference(){
  union(){
   for(p=click_anchors)support(p);
   beam(click_anchors,roof_z,roof_t);
   beam([[83,91.43],[91,91.43]],roof_z,roof_t,2);
   translate([83.6,90.43,contact_z])cube([1,2,roof_z-contact_z+.1]);
  }
  bore(click_anchors[0],2.8);bore(click_anchors[1],3.4);
  mark([91,92],"FIT?");
 }
}
// Case-mounted saddles, never mounted on a module or electronic connector.
// Dimensions below are design proposals. Actual jacket diameter and ribbon thickness are unknown.
module saddle(flat=false){
 difference(){
  translate([-12,-4,0])cube([24,8,8]);
  for(x=[-9,9])translate([x,0,-1])cylinder(d=3.4,h=10);
  if(flat)translate([-4.2,-5,-1])cube([8.4,10,2]);
  else translate([0,-5,1])rotate([-90,0,0])cylinder(d=5,h=10);
  if(!flat)translate([-2.5,-5,-1])cube([5,10,2]);
 }
}
module user_plunger(){
 // Generic captive plunger, cut length only after C05 height/stroke is known.
 cylinder(d=8,h=2);translate([0,0,2])cylinder(d=3,h=12);
}
if(part=="mki")mki();
else if(part=="click")click();
else if(part=="flat_clamp")saddle(true);
else if(part=="round_clamp")saddle(false);
else if(part=="user_plunger")user_plunger();
else assert(false,"Unknown part");
