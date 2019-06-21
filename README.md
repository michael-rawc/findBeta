# findBeta
Bouldering beta calculator using Reinforcement Learning

## How to use:

Navigate to main folder  
Compile with: g++ src\findBeta.cpp -o findBeta  
Run with: findBeta climbers\name_of_climber_file.txt problems\name_of_problem_file.txt

## Climber and Problem Files:

### Climber files are formatted as follows:

climber weight_in_kg height_in_cm ape_index_as_multiple_of_height

### Problem files are formatted as follows:

wall width_in_m height_in_m angle_to_horizontal

(As of yet, only an angle to the horizontal of 0 is supported)

After the initial line, there needs to be a set of lines of either 0, 1, 2, 3, or 4. Each number represents a 10x10cm area on the wall.  
The meanings of the numbers are as follows:  
* 0 - empty space on wall.
* 1 - starting hand hold.
* 2 - starting foot hold.
* 3 - generic hold on problem.
* 4 - finish hold for problem.
