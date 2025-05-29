

from setuptools import setup, Extension
import pybind11
import os

INCLUDE_DIR = r"D:\repos\FlightLab\3rd-party\vcpkg\installed\x64-windows\include"
LIB_DIR = r"D:\repos\FlightLab\3rd-party\vcpkg\installed\x64-windows\debug\lib"

module = Extension(
    'flight_lab',
    sources=[
        'flight_lab.cpp',
        '../Objects/Aircraft.cpp',
        '../Objects/Waypoint.cpp',
        '../Objects/Missile.cpp',
        '../Objects/Radar.cpp',
        '../Randerer/RenderManager.cpp',
        '../Engine/Simulation.cpp',
        '../Engine/World/CoordinateSystem.cpp',
    ],
    include_dirs=[
        pybind11.get_include(),
        INCLUDE_DIR
    ],
    libraries=['INIReader', 'SDL2d', 'SDL2_imaged', 'gdald', 'inih'],
    library_dirs=[LIB_DIR],
    extra_compile_args=['/MDd','/std:c++17'],
    # extra_compile_args=["/MDd"],  # Use /MDd for Debug (Multithreaded Debug DLL)
    # extra_compile_args=["/MD"],  # Use /MD for Release (Multithreaded DLL)
    extra_link_args=['/LIBPATH:' + LIB_DIR],
    language='c++'
)

setup(
    name='FlightLab',
    version='1.0',
    ext_modules=[module],
)
