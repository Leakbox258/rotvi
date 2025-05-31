#pragma once
#ifndef ROTVI_TOOLS_HPP
#define ROTVI_TOOLS_HPP

#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#define nop void(0)

inline std::time_t timmer = 0;
inline std::size_t counter = 0;
inline std::fstream *logStream;

#define timmerOn                                                                                                       \
    {                                                                                                                  \
        timmer = std::time(nullptr);                                                                                   \
        std::cerr << counter << " timmer On: " << __FILE__ << ":" << __LINE__ << std::endl;                            \
    }

#define timmerOff                                                                                                      \
    {                                                                                                                  \
        std::cerr << counter << " timmer Off" << __FILE__ << ":" << __LINE__ << std::endl;                             \
        std::cerr << "timmer " << counter << ": " << std::difftime(std::time(nullptr), timmer) << " seconds"           \
                  << std::endl;                                                                                        \
        timmer = 0;                                                                                                    \
    }

using string = std::string;

#endif