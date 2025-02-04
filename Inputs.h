#ifndef Inputs_H
#define Inputs_H

#include <vector>
#include <string>

class Inputs {
public:
    double time;
    int uav_id;
    double x_pos;
    double y_pos;
    Inputs(double t, int id, double x, double y);
};

std::vector<Inputs> SimCmds(const std::string& input_f);

#endif 