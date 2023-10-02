// Parameters
$fn = 200;
t_step = 0.01;
curve_width = 5;
thickness = 18;
full_height = 10;
inner_vase = "YES";


// Shape Module - PolyLine
module line(point1, point2, width = 1) {
    angle = 90 - atan((point2[1] - point1[1]) / (point2[0] - point1[0]));
    offset_x = 0.5 * width * cos(angle);
    offset_y = 0.5 * width * sin(angle);

    offset1 = [-offset_x, offset_y];
    offset2 = [offset_x, -offset_y];

    polygon(points=[
        point1 + offset1, point2 + offset1,  
        point2 + offset2, point1 + offset2
    ]);
}

module polyline(points, width = 1) {
    module polyline_inner(points, index) {
        if(index < len(points)) {
            line(points[index - 1], points[index], width);
            polyline_inner(points, index + 1);
        }
    }

    polyline_inner(points, 1);
}

// Shape Module - Bezier Curve
function bezier_coordinate(t, n0, n1, n2, n3) = n0 * pow((1 - t), 3) + 3 * n1 * t * pow((1 - t), 2) + 3 * n2 * pow(t, 2) * (1 - t) + n3 * pow(t, 3);

function bezier_point(t, p0, p1, p2, p3) = 
    [
        bezier_coordinate(t, p0[0], p1[0], p2[0], p3[0]),
        bezier_coordinate(t, p0[1], p1[1], p2[1], p3[1]),
        bezier_coordinate(t, p0[2], p1[2], p2[2], p3[2])
    ];
    

function bezier_curve(t_step, p0, p1, p2, p3) = [for(t = [0: t_step: 1 + t_step]) bezier_point(t, p0, p1, p2, p3)];


// Object Module
module letter_G() {
    
    px0 = 0;
    py0 = 20;   
    px1 = -20;  
    py1 = 20;  
    px2 = -20; 
    py2 = -10;  
    px3 = 0;   
    py3 = -10;
    px4 = 0;
    py4 = -10;
    px5 = 5.5;
    py5 = -6.5;
    px6 = 6.5;
    py6 = -2.5;
    px7 = 0.0;
    py7 = 7;
    px8 = 0;
    py8 = 7;
    px9 = 1.5;
    py9 = 7;
    px10 = 2.5;
    py10 = 7;
    px11 = 6.5;
    py11 = 7;
    px12 = 6.5;
    py12 = 7;
    px13 = 6.5;
    py13 = -9;
    px14 = 6.5;
    py14 = -7;
    px15 = 6.5;
    py15 = -10;

    p0 = [px0, py0, 0];
    p1 = [px1, py1, 0]; 
    p2 = [px2, py2, 0];
    p3 = [px3, py3, 0];
    p4 = [px4, py4, 0];
    p5 = [px5, py5, 0];
    p6 = [px6, py6, 0];
    p7 = [px7, py7, 0];
    p8 = [px8, py8, 0];
    p9 = [px9, py9, 0];
    p10 = [px10, py10, 0];
    p11 = [px11, py11, 0];
    p12 = [px12, py12, 0];
    p13 = [px13, py13, 0];
    p14 = [px14, py14, 0];
    p15 = [px15, py15, 0];

    linear_extrude(thickness, center = true);
    
    points1 = bezier_curve(t_step, p0, p1, p2, p3);
    points2 = bezier_curve(t_step, p4, p5, p6, p7);
    points3 = bezier_curve(t_step, p8, p9, p10, p11);
    points4 = bezier_curve(t_step, p12, p13, p14, p15);
                    
    union() {
        polyline(points1, curve_width);
        polyline(points2, curve_width);
        polyline(points3, curve_width);
        polyline(points4, curve_width);
    }
}

module letter_O() {
    difference() {
        union() {
            circle(50);
            translate([0, 1]) circle(40);
        }
        translate([0, 7]) scale([1, 0.5, 1]) circle(45);
    }
}

module letter_L() {
    unit_len = curve_width * 5;
    union() {
        translate([unit_len * 3, 0, 0])
        polygon(points=[
            [0, 0], [unit_len, 0],
            [unit_len, unit_len*2], [0, unit_len*2]
        ]);
        polygon(points=[
            [0,0], [100, 0],
            [100, -25], [0, -25]
        ]);
    }
}

module letter_E() {
    unit_len = curve_width * 5;
    union() {
        
        translate([unit_len * 3, 0, 0])
        polygon(points=[
            [0, 0], [unit_len, 0],
            [unit_len, unit_len*2], [0, unit_len*2]
        ]);
        
        translate([unit_len * 1.5, 0, 0])
        polygon(points=[
            [0, 0], [unit_len, 0],
            [unit_len, unit_len*2], [0, unit_len*2]
        ]);
        
        translate([0, 0, 0])
        polygon(points=[
            [0, 0], [unit_len, 0],
            [unit_len, unit_len*2], [0, unit_len*2]
        ]);
        
        polygon(points=[
            [0,0], [100, 0],
            [100, -25], [0, -25]
        ]);
        
//        translate([100, 0, 0]) rotate([90, 0, 180]) 
//        cube([curve_width * 4, 1, 65]);
//        
//        translate([60, 0, 0]) rotate([90, 0, 180]) 
//        cube([curve_width * 4, 1, 65]);
//        
//        translate([20, 0, 0]) rotate([90, 0, 180]) 
//        cube([curve_width * 4, 1, 65]);
//        
//        cube([100, curve_width * 4, 1]);
    }
}

module google(tckn) {
    for (i=[0:1:tckn]) {
        color("blue") translate([10, 0, i]) scale([3, 3.5, 3.5]) rotate([0, 0, 90])  letter_G();
        color("red") translate([0, 90, i]) letter_O();
        color("yellow") translate([0, 200, i]) letter_O();
        color("blue") translate([10, 320, i]) scale([3, 3.5, 3.5]) rotate([0, 0, 80])  letter_G();
        color("green") translate([-60, 390, i]) letter_L();
        color("red") translate([-60, 480, i]) letter_E();
    }
}


// Display - Static Google
google(thickness);
//letter_L();
