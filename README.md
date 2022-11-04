# st_wall
stranger thing wall build on zephyr RTOS

## Building the Project 
Project was used west tools and zephyer RTOS. below are the minimal commands to build the project
1. virtualenv -p python3 env
2. source env/bin/activate
3. pip3 install west
4. west init -m https://github.com/wima88/st_wall
5. west update
6. pip3 install -r zephyer/scripts/requirments,txt
7. west build -b stm32_min_dev_blue 
8. west flash --runner jlink


