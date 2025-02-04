#include "Inputs.h"
#include <fstream> //for reading the file
#include <sstream> //for using line as input
#include <stdexcept> // error handle
#include <algorithm>

Inputs::Inputs(double t, int id, double x, double y) 
    : time(t), uav_id(id), x_pos(x), y_pos(y) {}

std::vector<Inputs> SimCmds(const std::string& input_f) {
    /*
    to create a vector then each line has his paramters of the input file.
    */
    std::vector<Inputs> inputs;
    std::ifstream file(input_f);
    
    if (!file) {
        throw std::runtime_error("Can't open the file: " + input_f);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::istringstream line_input(line);
        double time, x_pos, y_pos;
        int uav_id;
        
        if (line_input >> time >> uav_id >> x_pos >> y_pos) {
            inputs.emplace_back(time, uav_id, x_pos, y_pos);
        }
    }
    
    file.close();

    std::sort(inputs.begin() , inputs.end(), [](const Inputs& a ,const Inputs&b){
        if (a.time == b.time){
            return a.uav_id <b.uav_id;
        }
        return a.time<b.time;
    });
    return inputs;
}