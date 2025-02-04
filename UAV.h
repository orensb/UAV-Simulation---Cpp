#ifndef UAV_H
#define UAV_H

#include <utility> //for using pair 
#include <string>


class UAV {
private:
   std::string uav_name_file;
   int UAV_num;
   double step_t;
   double time;
   double radius;
   double pos_x;
   double pos_y;
   double height;
   double azimuth; // where the UAV is looking at
   double distance=0;
protected:
   double dest_x;
   double dest_y;
   double velocity;
   double theta; // the angle from the UAV to the point
public:
   // Constructor
   UAV(std::string uav_name , int num,double delta, double start_time, double def_r, double def_x, double def_y, double def_h, double def_v, double def_az);
   void set_destination(const double set_x,const double set_y); // update destination
   void transit_x_y(); //update X and Y via linear distnace with axis
   void Hold_x_y(); //update the X and Y via polar coordinate
   void near_dest();
   void update_time();
   std::pair<double,double> velocity_calc(); // calcualte the velocity by componetnes.
   std::string distance_from_target();
   void update_azimuth_transit();
   void update_distance();
   double get_pos_x() const { return pos_x; }
   double get_pos_y() const { return pos_y; }
   double get_des_x() const { return dest_x; }
   double get_des_y() const { return dest_y; }
   double get_azimuth() const { return azimuth; }
   double get_therta() const {return theta; }
   double get_distance() const {return distance;}
   std::string get_name_file() const {return uav_name_file;}
   int get_uav_id() const {return UAV_num;}


   ~UAV() = default;

};

#endif // UAV_H
