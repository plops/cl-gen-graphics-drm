//! \file main.cpp Draw to screen using linux direct rendering manager
#include "cxxopts.hpp"
#include <cstring>
#include <drm/drm.h>
#include <drm/drm_mode.h>
#include <fstream>
#include <iomanip>

//! \mainpage Draw using direct rendering manager
//! \section Introduction
//! This repository contains a minimal program to draw to a linux screen. See
//! http://betteros.org/tut/graphics1.php for a tutorial this code is based on.
//! \section Dependencies
//! - Linux kernel with DRM driver

//! - cxxopts for commandline argument parsing.

//! - sbcl to generate c++ code
//! - cmake to configure for build
//! - g++ to compile c++ code

//! - For the documentation (optional for use):
//!   + doxygen

//! - The C++ code can be opened as a cmake project in kdevelop, clion or
//! qtcreator (order of my preference).

//! \section References
//! 1. http://betteros.org/tut/graphics1.php

//! @brief main function
//!
//! @usage parse command line parameters and draw to screen
//!
//! @param argc input number of command line arguments
//! @param argv input
//!
//! @return Integer

int main(int argc, char **argv) {
  try {
    cxxopts::Options options("drm_draw", "draw to drm device");
    options.add_options()("h,help", "Print help")(
        "r,rate", "frame rate (Hz,double)",
        cxxopts::value<double>()->default_value("60.0"));
    options.parse(argc, argv);
    if (options.count("help")) {
      (std::cout << options.help() << std::endl);
      exit(0);
    }
    (std::cout << "requested frame rate = " << options["rate"].as<double>()
               << " Hz" << std::endl);
  } catch (const cxxopts::OptionException &e) {

    (std::cout << "error parsing options: " << e.what() << std::endl);
    exit(1);
  }
  return 0;
}