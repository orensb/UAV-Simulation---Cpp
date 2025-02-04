

#include "UAV.h"
#include "Inputs.h"
#include <iostream>
#include <iomanip> //for 2 decimal in my print, and the txt style.
#include <fstream> //read and the file
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <cmath>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>

std::map<std::string, double> readINIfile(const std::string& ini_file);
std::mutex create_uav_lock;
std::mutex print_mutex;

void UAV_Operations(UAV &uav,std::vector<Inputs>& inputs, double step_t , 
 int time , std::unordered_map<int,std::stringstream> &uav_data, 
 std::unordered_map<int,std::ofstream> &uav_files){
    double t = 0.0;
    size_t inputs_idx = 0;
    while(t<time){
        std::cout << "Time: " << t << "s |UAV ID: "<<uav.get_uav_id()<<" | Position: (" << uav.get_pos_x() << ", " << uav.get_pos_y() <<  ") |Distance to location" <<uav.get_distance() <<std::endl;
        std::cout<< "Azimute: "<<uav.get_azimuth()<<" deg"<<std::endl;
        while  (inputs_idx < inputs.size() && inputs[inputs_idx].time-step_t <t && inputs[inputs_idx].time+step_t >t){
            if (inputs[inputs_idx].uav_id == uav.get_uav_id()){
                uav.set_destination(inputs[inputs_idx].x_pos,inputs[inputs_idx].y_pos);
                std::cout<<"NEW DESTINATION!"<<std::endl;
            }
            inputs_idx++;
        }
        std:: string option = uav.distance_from_target();
        if (option == "Hold") {
            std::cout << "UAV is within the target radius. Holding position.\n";
            uav.Hold_x_y();  // Move the UAV in circle around the point
        } 
        else if (option == "Transit") {
            std::cout << "UAV is moving towards the target.\n";
            uav.transit_x_y();  // UAV flying through the target
        }
        else{
            std::cout<< "UAV is near the destination, going to radius. \n";
            uav.near_dest(); //UAV to close to the centre
        }
        int id = uav.get_uav_id();
        uav_data[id] <<std::left<< std::setw(10) << (t+step_t)
                << "| " << std::setw(12) << uav.get_pos_x()
                << "| " << std::setw(12) << uav.get_pos_y()
                << "| " << std::setw(10) << uav.get_azimuth() 
                << "| " << std::setw(11) << uav.distance_from_target()
                << "| " <<"("<< uav.get_des_x() <<"," << uav.get_des_y()<< std::setw(10) <<")"  
                <<  "| "  << uav.get_distance()
                << std::endl;
        {
            std::lock_guard<std::mutex> guard(create_uav_lock);
            if (uav_data[id].str().size() > 1000){
                uav_files[id] << uav_data[id].str();
                uav_data[id].str("");
                uav_data[id].clear();
            }
        }
        t += step_t;
    }
}



int main() {

    std::map<std::string,double> parameters;
    parameters = readINIfile("SimParams.ini");

    // UAV initial parameters
    int N_uav = parameters["N_uav "];
    double step_t = parameters["Dt "];   
    double time = parameters["TimeLim "];     
    double radius = parameters["R "];   
    double pos_x = parameters["X0 "];    
    double pos_y = parameters["Y0 "];       
    double height = parameters["Z0 "];   
    double velocity = parameters["V0 "];   
    double azimuth = parameters["Az "];    

    //Vector of all the inputs, sorted by the time and UAV_ID 
    std::vector<Inputs> inputs;
    inputs = SimCmds("SimCmds.txt");

    // create a vector of all UAVS objects.
    // create a file for each on of them.
    auto start = std::chrono::high_resolution_clock::now();
    std::unordered_map<int,std::stringstream> uav_data;
    std::unordered_map<int,std::ofstream> uav_files;

    std::vector<UAV> UAVS;
    for (int i=0 ; i<N_uav ; i++){
        std:: string uav_name_file = "UAV" + std::to_string(i) + ".txt";
        UAV uav_i(uav_name_file,i, step_t, time, radius, pos_x, pos_y, height, velocity, azimuth);
        UAVS.emplace_back(uav_i);
        uav_files[i].open(uav_name_file,std::ios::out);
        uav_data[i] << std::left<< std::setw(10) << "Time"
                << "| " << std::setw(12) << "X-position"
                << "| " << std::setw(12) << "Y-position"
                << "| " << std::setw(10) << "Azimuth"
                << "| " << std::setw(10) << "Flight Mode"
                << "| " << std::setw(15) << "Destination"
                << "| " << std::setw(10) << "Distance to target"
                << std::endl;
        uav_files[i] << uav_data[i].str();
        uav_data[i].str("");
        uav_data[i].clear();
    }   
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double,std::milli> duration = end - start;
    std::cout << "Time of creating the UAVS: " << duration.count() << " ms" << std::endl;


    // Start simulation loop
    auto start1 = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads_uav;

    for (auto& uav : UAVS){
        std::cout << std::fixed << std::setprecision(2);
        threads_uav.emplace_back(UAV_Operations,std::ref(uav), std::ref(inputs), step_t , time , std::ref(uav_data), std::ref(uav_files));

    }
    for (auto& thread : threads_uav) {
    thread.join();
    }   
    for (auto& [id,info] : uav_data){
        uav_files[id] << info.str();
        uav_files[id].close();
    }
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double,std::milli> duration1 = end1 - start1;
    // Due to files writing -> I/O opeartation, it's better to minimize the systam call for it.
    std::cout<< "Total Main loop time: " << duration1.count() << std::endl;
    std::cout << "Time of creating the UAVS: " << duration.count() << " ms" << std::endl;

    return 0;
}

std::map<std::string,double> readINIfile(const std::string& ini_file){
    /*
    function to read from the ini file.
    we have a dic way, key = string and vlaue = double
    want a map of each paramater and it's value.
    */
    std::map<std::string,double> parameters;
    std::ifstream file(ini_file);
    if (!file){
        throw std::runtime_error("Cant open the file" + ini_file);
    }
    // line for read each line
    // we need a string and a value
    std::string line;
    while(std::getline(file,line)){
        if (line.empty()) continue;
        // we want to separte the line input so create a stream object
        std::istringstream line_input(line);
        std::string key;
        double value;
        // read into key until '='.
        // now the pointer is after '=' 
        // insert into value the remain. both need to coexist togher.
        if(std::getline(line_input, key, '=') && (line_input >> value)){
            parameters[key] = value;
        }
    }
    file.close();
    return parameters;

}




