$fn=100;


shell = 1.5;
pcbWidth = 80;
pcbLength = 60;
clearance = 2;
boxWidth = 90;

//difference()
//{
//  union()
//  {
//    Box(boxWidth + clearance, 60 + clearance, 33);
//    Screws(2, 6, 8, 80, 60, 2.5);
//    translate([0, 0, 33 + shell - 10]) LidScrews(2, 15, 10, boxWidth + clearance, 60 + clearance, 3);
//  }
//  translate([55, 0, 2.5]) rotate([0, 0, 90]) linear_extrude(height=2) text("Mark was here", size = 4, halign="center");
//}

translate([0,0,40])
difference()
{
  union()
  {
    width = boxWidth + clearance;
    length = 60 + clearance;
    height = 2;
    hull()
    {
      translate([-width/2+shell, -length/2+shell, 0]) cylinder(r=shell*2,h=height);
      translate([+width/2-shell, -length/2+shell, 0]) cylinder(r=shell*2,h=height);
      translate([+width/2-shell, +length/2-shell, 0]) cylinder(r=shell*2,h=height);
      translate([-width/2+shell, +length/2-shell, 0]) cylinder(r=shell*2,h=height);
    }
    translate([0, 0, height]) linear_extrude(height = 0.5, center = false, convexity = 10) scale(0.4) import("SoundOfBrownsburgLogo.svg", center = true);
  }
  translate([0,0,-0.5]) LidScrewHoles(3, 10, boxWidth + clearance, 60 + clearance, 3);
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
    translate([22, -length/2+0.5, 7+8+shell]) rotate([90, 0, 0]) cylinder(d=8, h=shell+1);
    translate([0, -length/2-0.5, 8+8+shell]) cube([10, shell+1, 5], center=true);
  }
  translate([width/2 + 8, 0, 0]) Ear(20, length, 3);
  translate([-width/2 - 8, 0, 0]) mirror([1,0,0]) Ear(20, length, 3);
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
    translate([inset, inset, 0]) cylinder(d = inside, h = height + 1);
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

module Ear(width, length, thickness)
{
  inset = 5;
  hole = 3;
  difference()
  {
    hull()
    {
      translate([-width/2+shell, -length/2+shell, 0]) cylinder(r=shell*2,h=thickness);
      translate([+width/2-shell, -length/2+shell, 0]) cylinder(r=shell*2,h=thickness);
      translate([+width/2-shell, +length/2-shell, 0]) cylinder(r=shell*2,h=thickness);
      translate([-width/2+shell, +length/2-shell, 0]) cylinder(r=shell*2,h=thickness);
    }
    translate([width/2 - inset, length/2 - inset, 0]) cylinder(d = hole, h = thickness + 1);
    translate([width/2 - inset, -length/2 + inset, 0]) cylinder(d = hole, h = thickness + 1);
  }
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