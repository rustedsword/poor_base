cmake_minimum_required(VERSION 3.5)

project(misc LANGUAGES C)

add_library(misc INTERFACE)
target_include_directories(misc INTERFACE include/)
