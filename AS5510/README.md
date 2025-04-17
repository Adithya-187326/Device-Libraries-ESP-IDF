# _AS5600_

This is the component library for BNO055 communicating over I2C.

# IMPLEMENTATION

You can add this to component folder to your project's working directory work without any further modifications. Below is a short explanation of the folder structure:

```
├── CMakeLists.txt
├── components
|   ├── AS5600
|   |   ├── CMakeLists.txt
|   |   ├── include
|   |   ├── src
|   |   ├── README.md                This is the file you are currently reading
├── main
│   ├── CMakeLists.txt
│   └── main.c
└── README.md
```

# ADDING THE COMPONENT

Add one line of code in the CMakeLists.txt in the project folder (not the main folder), to add an extra component directory. It is really crucial that the order of the statements is not missed as it can lead to CMake errors.

```C
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "components/AS5600")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(oled-display)
```
