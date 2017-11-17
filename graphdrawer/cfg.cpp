#include <cfg.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace graphdrawer {

cfg::cfg(int ac, char* av[]) : _vm(),_config_valid(true) {
    std::string config_file;
    po::options_description generic("Generic options");
    generic.add_options()
        ("version,v", "print version string")
        ("help", "produce help message")
        ("config,c", po::value<std::string>(&config_file)->default_value("graphdrawer.cfg"),
                "configuration file.")
        ;

    // Declare a group of options that will be 
    // allowed both on command line and in
    // config file
    po::options_description config("Configuration");
    config.add_options()
        ("gml", "output a .gml file")
        ("out", po::value<std::string>()->default_value("converted-graph"), "output filename")
        ("cull", po::value<int>(), "remove nodes with degree > arg")
        ("width,w", po::value<int>()->default_value(50), "width of drawn nodes")
        ("height,h", po::value<int>()->default_value(50), "height of drawn nodes")
        ;

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input-file", po::value<std::string>(), "input file in gml format")
        ;

    
    po::options_description cmdline_options;
    cmdline_options.add(generic).add(config).add(hidden);

    po::options_description config_file_options;
    config_file_options.add(config).add(hidden);

    po::options_description visible("Allowed options");
    visible.add(generic).add(config);
    
    po::positional_options_description p;
    p.add("input-file", -1);
    
    store(po::command_line_parser(ac, av).
            options(cmdline_options).positional(p).run(), _vm);
    notify(_vm);
    
    std::ifstream ifs(config_file.c_str());
    if (!ifs)
    {
        std::cout << "can not open config file: " << config_file << "\n";
        _config_valid = false;
    }
    else
    {
        store(parse_config_file(ifs, config_file_options), _vm);
        notify(_vm);
    }
   _config_valid &= validate_config();

   if(!_config_valid) {
       std::cout << visible << std::endl;
   }
}

std::string cfg::input_filename() const {
    return _vm["input-file"].as<std::string>();
}

std::string cfg::output_filename() const {
    auto type = output_filetype();
    std::string filename = _vm["out"].as<std::string>();
    switch(type) {
        case filetype::GML : return filename + ".gml";
        case filetype::SVG : return filename + ".svg";
    }
}

filetype cfg::output_filetype() const {
    if(_vm.count("gml")) {
        return filetype::GML;
    }
    return filetype::SVG;
}

bool cfg::validate_config() const {
    if (_vm.count("help")) {
        return false;
    }

    if (_vm.count("input-file")) {
        std::cout << "Input file is "
                << _vm["input-file"].as<std::string>() << ".\n";
    } else {
        std::cout << "No input file specified" << std::endl;
        return false;
    }
    return true;
}

int cfg::cull_graph() const {
    if (_vm.count("cull")) {
        int val = _vm["cull"].as<int>();
        return val > 0 ? val : 0;
    }
    return 0;
}

bool cfg::config_valid() const {
    return _config_valid;
}

int cfg::node_width() const {
    return _vm["width"].as<int>();
}
int cfg::node_height() const {
    return _vm["height"].as<int>();
}

}
