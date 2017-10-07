//! \file main.cpp Draw to screen using linux direct rendering manager
#include <cstdint>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <drm/drm.h>
// drm/drm.h needs to be patched for C++ to compile, one of the datastructures
// has an element called virtual
#include "cxxopts.hpp"
#include <array>
#include <cstring>
#include <drm/drm_mode.h>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

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
        "d,device", "device file",
        cxxopts::value<std::string>()->default_value("/dev/dri/card0"))(
        "r,rate", "frame rate (Hz,double)",
        cxxopts::value<double>()->default_value("60.0"));
    options.parse(argc, argv);
    if (options.count("help")) {
      (std::cout << options.help() << std::endl);
      exit(0);
    }
    (std::cout << "requested frame rate = " << options["rate"].as<double>()
               << " Hz device=" << options["device"].as<std::string>()
               << std::endl);
    {
      auto dri_fd(open(options["device"].as<std::string>().c_str(), O_RDWR));
      if ((ioctl(dri_fd, DRM_IOCTL_SET_MASTER, 0) < 0)) {
        (std::cerr << "ioctl DRM_IOCTL_SET_MASTER failed." << std::endl);
      }
      {
        drm_mode_card_res res{};
        memset(&res, 0, sizeof(res));
        if ((ioctl(dri_fd, DRM_IOCTL_MODE_GETRESOURCES, &res) < 0)) {
          (std::cerr << "ioctl DRM_IOCTL_MODE_GETRESOURCES failed."
                     << std::endl);
        }
        (std::cout << "count_fbs = " << res.count_fbs << std::endl);
        (std::cout << "count_crtcs = " << res.count_crtcs << std::endl);
        (std::cout << "count_connectors = " << res.count_connectors
                   << std::endl);
        (std::cout << "count_encoders = " << res.count_encoders << std::endl);
        (std::cout << "min_width = " << res.min_width << std::endl);
        (std::cout << "max_width = " << res.max_width << std::endl);
        (std::cout << "min_height = " << res.min_height << std::endl);
        (std::cout << "max_height = " << res.max_height << std::endl);
      }
      {
        drm_mode_card_res res{};
        std::array<uint64_t, 10> fb_array{};

        res.fb_id_ptr = reinterpret_cast<uint64_t>(fb_array.data());
        std::array<uint64_t, 10> crtc_array{};

        res.crtc_id_ptr = reinterpret_cast<uint64_t>(crtc_array.data());
        std::array<uint64_t, 10> connector_array{};

        res.connector_id_ptr =
            reinterpret_cast<uint64_t>(connector_array.data());
        std::array<uint64_t, 10> encoder_array{};

        res.encoder_id_ptr = reinterpret_cast<uint64_t>(encoder_array.data());
        if ((ioctl(dri_fd, DRM_IOCTL_MODE_GETRESOURCES, &res) < 0)) {
          (std::cerr << "ioctl DRM_IOCTL_MODE_GETRESOURCES failed."
                     << std::endl);
        }
      }
    }
  } catch (const cxxopts::OptionException &e) {

    (std::cout << "error parsing options: " << e.what() << std::endl);
    exit(1);
  }
  return 0;
}