import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import FancyArrowPatch

def read_uav_data(file_path):
    "Array of the data of each UAV"
    data = []
    with open(file_path, 'r') as file:
        lines = file.readlines()[1:]  
        for line in lines:
            parts = line.split()
            time = float(parts[0])
            x = float(parts[2])
            y = float(parts[4])
            azimuth = float(parts[6])
            dest_str = parts[10][1:-1]  
            dest_x, dest_y = map(float, dest_str.split(','))
            
            data.append((time, x, y, azimuth, dest_x, dest_y))    
    return data

def plot_multiple_uavs(num_files):
    """Plots multiple UAVs from their respective files in one interactive animation."""
    plt.ion()  # interactive mode!
    fig, ax = plt.subplots(figsize=(10, 6))

    # sub plot on the left for uav location
    ax_left = fig.add_axes([0.01, 0.15, 0.19, 0.7])  
    ax_left.set_xlim(0, 1)
    ax_left.set_ylim(0, num_files)
    ax_left.set_xticks([])
    ax_left.set_yticks([])
    ax_left.set_title('UAV Locations', fontsize=10)

    # main plot
    ax.set_xlim(-500, 550)
    ax.set_ylim(-500, 550)
    ax.set_aspect('equal')
    ax.set_xlabel("X Position")
    ax.set_ylabel("Y Position")

    # Load data from all UAV files
    # all data will be an array that each element is the UAV_i files.
    # so we will go over all_data[i][t] -> go over all "i" UAV at time "t"
    all_data = []
    for i in range(0, num_files):
        file_path = f"UAV{i}.txt"
        data = read_uav_data(file_path)
        all_data.append(data)

    # size of each on that i want
    arrows = [None] * num_files
    labels = [None] * num_files
    lines = [None] * num_files 
    location_labels = [None] * num_files  


    max_timesteps = max(len(data) for data in all_data)
    # every time t
    for t in range(max_timesteps):
        # every UAV we have
        for i in range(num_files):
            # Remove the previous values
            if arrows[i]:
                arrows[i].remove()
            if labels[i]:
                labels[i].remove()
            if lines[i]:  
                for line in lines[i]:
                    line.remove()

            # Get data from UAV "i" at time "t"
            if t < len(all_data[i]):  # Ensure data exists for this UAV
                time, x, y, azimuth , dest_x , dest_y = all_data[i][t]

                # convert to math notation for calcualtion 
                math_azimuth = (90 - azimuth) % 360

                # Calculate the arrow direction based on azimuth (where we look at)
                dx = np.cos(np.radians(math_azimuth))  
                dy = np.sin(np.radians(math_azimuth))  
                # FancyArrowPatch, built in function in matlb
                arrows[i] = FancyArrowPatch(
                    posA=(x, y), 
                    # size of the arrow head
                    posB=(x + dx * 10, y + dy * 10),  
                    arrowstyle='-|>',  
                    mutation_scale=20, 
                    color=f'C{i}',  
                    linewidth=3 
                )
                # arrow head for each uav
                ax.add_patch(arrows[i])

                # Add the location label below the arrow
                #https://matplotlib.org/stable/api/_as_gen/matplotlib.axes.Axes.text.html
                labels[i] = ax.text(
                    # -3 that it will be below
                    x, y - 3,  
                    f"UAV {i}",
                    color=f'C{i}',  
                    fontsize=10,  
                    ha='center',  
                    va='top'  
                )
                # line from UAV location to destination
                lines[i] = ax.plot([x, dest_x], [y, dest_y], color=f'C{i}', linestyle='--', linewidth=2)

                # Left ax the UAVs locations
                if location_labels[i]:
                    location_labels[i].remove()  # Remove previous values
                location_labels[i] = ax_left.text(
                    0.5, num_files - i - 0.5,  # Position on the left side
                    f"UAV {i}: ({x:.2f}, {y:.2f})",  # Show the coordinates of the UAV
                    color=f'C{i}',
                    fontsize=10,
                    ha='center',
                    va='center'
                )

        ax.set_title(f"Multiple UAV Movements\nTime: {time:.2f} sec")
        plt.draw()
        plt.pause(0.01)  

    plt.ioff()  
    plt.show()

def main():
    num_files = 4  # Number of UAV 
    plot_multiple_uavs(num_files)

if __name__ == '__main__':
    main()
