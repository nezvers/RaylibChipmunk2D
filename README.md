# Raylib with Chipmunk2D    

## Prerequisites    
- premake5 and any suported compiler or IDE    

## What does it do?    
Raylib example project using Chipmunk2D physics engine. Example code from [bedroomcoders](https://bedroomcoders.co.uk/raylib-and-chipmunk2d/)    

## Usage    
```sh
# clone this repository
git clone https://github.com/nezvers/RaylibTemplate.git
# change directory to the repository
cd RaylibTemplate
#initialize config
git submodule init
# get both raylib and Chipmunk2D - in my experience slow download
git submodule update --init --recursive --progress   
```

Then, in a cmd or powershell terminal:     
- premake5 [target project] (https://premake.github.io/docs/Using-Premake/)    
