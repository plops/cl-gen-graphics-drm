//! \file main.cpp Draw to screen using linux direct rendering manager
#include <cassert>
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
        assert((res.count_fbs < 10));
        (std::cout << "count_crtcs = " << res.count_crtcs << std::endl);
        assert((res.count_crtcs < 10));
        (std::cout << "count_connectors = " << res.count_connectors
                   << std::endl);
        assert((res.count_connectors < 10));
        (std::cout << "count_encoders = " << res.count_encoders << std::endl);
        assert((res.count_encoders < 10));
        (std::cout << "min_width = " << res.min_width << std::endl);
        assert((res.min_width < 10));
        (std::cout << "max_width = " << res.max_width << std::endl);
        assert((res.max_width < 10));
        (std::cout << "min_height = " << res.min_height << std::endl);
        assert((res.min_height < 10));
        (std::cout << "max_height = " << res.max_height << std::endl);
        assert((res.max_height < 10));
      }
      {
        drm_mode_card_res resources{};
        std::array<uint64_t, 10> fb_array{};

        resources.fb_id_ptr = reinterpret_cast<uint64_t>(fb_array.data());
        std::array<uint64_t, 10> crtc_array{};

        resources.crtc_id_ptr = reinterpret_cast<uint64_t>(crtc_array.data());
        std::array<uint64_t, 10> connector_array{};

        resources.connector_id_ptr =
            reinterpret_cast<uint64_t>(connector_array.data());
        std::array<uint64_t, 10> encoder_array{};

        resources.encoder_id_ptr =
            reinterpret_cast<uint64_t>(encoder_array.data());
        if ((ioctl(dri_fd, DRM_IOCTL_MODE_GETRESOURCES, &resources) < 0)) {
          (std::cerr << "ioctl DRM_IOCTL_MODE_GETRESOURCES failed."
                     << std::endl);
        }
        for (unsigned int i = 0; (i < resources.count_connectors); i += 1) {
          {
            drm_mode_get_connector connector{};
            if ((ioctl(dri_fd, DRM_IOCTL_MODE_GETCONNECTOR, &connector) < 0)) {
              (std::cerr << "ioctl DRM_IOCTL_MODE_GETCONNECTOR failed."
                         << std::endl);
            }
            (std::cout << "count_modes = " << connector.count_modes
                       << std::endl);
            (std::cout << "count_props = " << connector.count_props
                       << std::endl);
            (std::cout << "count_encoders = " << connector.count_encoders
                       << std::endl);
          }
        }
      }
    }
  } catch (const cxxopts::OptionException &e) {

    (std::cout << "error parsing options: " << e.what() << std::endl);
    exit(1);
  }
  return 0;
}