#include<vector>
#include<iostream>
#include<algorithm>
#include<cmath>

#ifndef PI
#define PI 3.14159265358979323846
#endif

#include "UAV.h"

namespace UAVUtils{
    double round_to_two_decimal(double value) {
        return std::round(value * 100.0) / 100.0;
    }
    double theta_calculation(double x , double y){
        double angle = atan2(y,x); 
        if (angle<0){
            angle += 2* PI;
        }
        return angle;
    }
    double azimuth_calc(double x , double y){
        double angle = round_to_two_decimal((atan2(x,y) * (180/PI)));
        if (angle<0){
            angle +=360;
        }
        return angle;
    }
}
    


UAV::UAV(std::string name,int num,double delta, double start_time, double def_r, double def_x, double def_y, double def_h, double def_v, double def_az):
    uav_name_file(name),UAV_num(num),step_t(delta),time(start_time),radius(def_r),pos_x(def_x),pos_y(def_y),height(def_h),velocity(def_v),azimuth(def_az),dest_x(def_x),dest_y(def_y),theta(def_az){}


void UAV::set_destination(const double set_x,const double set_y){
    /*
    set the new destination of the UAV 
    */
    dest_x = set_x;
    dest_y= set_y;

    // calculate the new theta.

    double delta_x_2 = (pos_x - dest_x);
    double delta_y_2 = (pos_y - dest_y);
    theta = UAVUtils::theta_calculation(delta_x_2,delta_y_2);

}

void UAV::update_distance(){
    /*
    calcualte the distance between the UAV and his destination
    */
    double delta_x = (dest_x - pos_x);
    double delta_y = (dest_y - pos_y);
    distance = UAVUtils::round_to_two_decimal(sqrt(pow(delta_x,2) + pow(delta_y,2)));


}

void UAV::update_azimuth_transit(){
    /*
    Calculate the azimuth of the UAV facing during the transit 
    Calculate the theta angle from centre to the UAV, it will help us where we are reaching
    and preforming Hold.
    */
    //calculate the angle from position to desination in north as 0
    double delta_x = (dest_x - pos_x);
    double delta_y = (dest_y - pos_y);
    // atan2 -> radian
    azimuth = UAVUtils::azimuth_calc(delta_x,delta_y);

    // calculate theta from centre to UAV in math notation f
    theta = UAVUtils::theta_calculation(-delta_x,-delta_y);

}
std::pair<double,double> UAV::velocity_calc(){
    /*
    set the UAV velocity for X-axis and Y-axis.
    calcualte via the detla between destination and position
    normalize the direction vector first, to have a unit vector
    mul with V0
    */
    double fly_direction = azimuth * (PI/180);
    fly_direction = (PI/2) - fly_direction;
    double v_x = velocity * (cos(fly_direction));
    double v_y = velocity * (sin(fly_direction));
    return std::make_pair(v_x,v_y);
}
void UAV::transit_x_y(){
    /*
    when distance_from_target = false ->transit.
    transit flight, calcualte the velocity for each axis.
    update the each coordinate X and Y with the time step t
    */
    update_azimuth_transit();
    std::pair<double,double> vel = velocity_calc();
    pos_x = UAVUtils::round_to_two_decimal(pos_x + (step_t * vel.first));
    pos_y = UAVUtils::round_to_two_decimal(pos_y + (step_t * vel.second));
}
void UAV::update_time(){
    time+=step_t;
}


void UAV::Hold_x_y(){
    /*
    when reaching the centre by radius distance.
    UAV preform a circle motion clockwise. 
    Theta angle from centre to UAV in math notation
    Clockwise so we decrease the angle.
    Azimuth is tangted of this angle, but in north 0 notation
    */
    double omega = -(velocity/radius);
    theta += omega*step_t;

    if (theta < 0) {
        theta += 2 * PI;
    }

    pos_x = UAVUtils::round_to_two_decimal(dest_x + radius * cos(theta));
    pos_y = UAVUtils::round_to_two_decimal(dest_y + radius * sin(theta));
    // theta in north 0 notation
    double delta_x_3 = (pos_x - dest_x);
    double delta_y_3 = (pos_y - dest_y);
    double theta_deg = atan2(delta_x_3,delta_y_3) * (180/PI);
    if (theta_deg<0){
        theta_deg +=360;
    }
    azimuth = UAVUtils::round_to_two_decimal(theta_deg + (90));
    if (azimuth > 360) {
        azimuth -= 360; 
    }
}

std::string UAV::distance_from_target() {
    /*
    Check if the UAV is within the radius of the target.
    */
    update_distance();
    update_time();
    double margin = velocity * step_t;
    if (distance >radius+ margin) {
        return "Transit";
    }
    else if (radius-margin<=distance && distance <= radius+margin){
        return "Hold";
    }
    else return "Inside";
}

void UAV::near_dest(){
    /*
    in the circle radius. so need to fly away to the opposite direction.
    */

    if (distance !=0){
        update_azimuth_transit();
        azimuth +=180;
        if (azimuth>360){
            azimuth -=360;
        }
    }
    std::pair<double,double> vel = velocity_calc();
    std::cout<<"Vel.first"<<vel.first<<std::endl;
    pos_x = UAVUtils::round_to_two_decimal(pos_x + (step_t * vel.first));
    pos_y = UAVUtils::round_to_two_decimal(pos_y + (step_t * vel.second));
}


