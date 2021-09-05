$fn=100;


shell = 1.5;
pcbWidth = 80;
pcbLength = 60;
clearance = 2;
boxWidth = 120;
lcdWidth = 98;
lcdLength = 60;

//Box(boxWidth + clearance, 60 + clearance, 33);
//Screws(2, 6, 8, 80, 60, 2.5);
//translate([0, 0, 33 + shell - 10]) LidScrews(2, 15, 10, boxWidth + clearance, 60 + clearance, 3);
//translate([-50, -20, shell]) linear_extrude(height=0.5) rotate([0,0,90]) text(size = 4, "Mark was here");

difference()
{
  union()
  {
    difference()
    {
      Box(boxWidth + clearance, 60 + clearance, 12);
      cube([76, 26, 5], center = true);
      translate([0,0,-0.5]) LidScrewHoles(3, 12+shell, boxWidth + clearance, 60 + clearance, 3);
    }
    Screws(3, 8, 9.5, lcdWidth, lcdLength, 2.5);
    LidScrews(3, 17, 12+shell, boxWidth + clearance, 60 + clearance, 3);
  }
  LidScrewHoles(7, 5, boxWidth + clearance, 60 + clearance, 3);
}
module Box(width, length, height)
{
  difference()
  {
    hull()
    {
      translate([-width/2+shell, -length/2+shell, 0]) cylinder(r=shell*2,h=height + shell);
      translate([+width/2-shell, -length/2+shell, 0]) cylinder(r=shell*2,h=height + shell);
      translate([+width/2-shell, +length/2-shell, 0]) cylinder(r=shell*2,h=height + shell);
      translate([-width/2+shell, +length/2-shell, 0]) cylinder(r=shell*2,h=height + shell);
    }
    translate([-width/2, -length/2, shell]) cube([width, length, height + 1]);
//    translate([22, -length/2+0.5, 7+8+shell]) rotate([90, 0, 0]) cylinder(d=8, h=shell+1);
//    translate([0, -length/2-0.5, 8+8+shell]) cube([10, shell+1, 5], center=true);
  }
}

module Screw(inside, outside, height)
{
  difference()
  {
    cylinder(d = outside, h = height);
    cylinder(d = inside, h = height + 1);
  }
}

module Screws(inside, outside, height, width, length, inset)
{
  translate([-width/2 + inset, -length/2 + inset, shell]) Screw(inside, outside, height);
  translate([+width/2 - inset, -length/2 + inset, shell]) Screw(inside, outside, height);
  translate([+width/2 - inset, +length/2 - inset, shell]) Screw(inside, outside, height);
  translate([-width/2 + inset, +length/2 - inset, shell]) Screw(inside, outside, height);
}

module LidScrew(inside, outside, height, inset)
{
  difference()
  {
    cylinder(d = outside, h = height);
    translate([inset, inset, -0.5]) cylinder(d = inside, h = height + 1);
    translate([-outside/2, -outside-1, 0]) cube([outside + 1, outside + 1, height + 1]);
    translate([-outside-1, -outside/2, 0]) cube([outside + 1, outside + 1, height + 1]);
  }
}

module LidScrews(inside, outside, height, width, length, inset)
{
  translate([-width/2, -length/2, 0]) LidScrew(inside, outside, height, inset);
  translate([+width/2, -length/2, 0]) rotate([0, 0, 90]) LidScrew(inside, outside, height, inset);
  translate([+width/2, +length/2, 0]) rotate([0, 0, 180]) LidScrew(inside, outside, height, inset);
  translate([-width/2, +length/2, 0]) rotate([0, 0, 270]) LidScrew(inside, outside, height, inset);
}


module LidScrewHole(inside, height, inset)
{
  translate([inset, inset, 0]) cylinder(d = inside, h = height + 1);
}

module LidScrewHoles(inside, height, width, length, inset)
{
  translate([-width/2, -length/2, 0]) LidScrewHole(inside, height, inset);
  translate([+width/2, -length/2, 0]) rotate([0, 0, 90]) LidScrewHole(inside, height, inset);
  translate([+width/2, +length/2, 0]) rotate([0, 0, 180]) LidScrewHole(inside, height, inset);
  translate([-width/2, +length/2, 0]) rotate([0, 0, 270]) LidScrewHole(inside, height, inset);
}