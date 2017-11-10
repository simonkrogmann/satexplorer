#include <cfg.hpp>
#include <string>
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace graphdrawer {

cfg::cfg(int ac, char* av[]) : _desc("Allowed options") {
    init_options();
    po::store(po::parse_command_line(ac, av, _desc), _vm);
    po::notify(_vm);
}

void cfg::init_options() {
    _desc.add_options()
        ("help", "produce help message")
        ("gml", "output a .gml file")
        ("file", po::value<std::string>(), "input file in graphml format")
        ("out", po::value<std::string>(), "output filename")
        ("cull", po::value<int>(), "remove nodes with degree > arg");
    ;
}

std::string cfg::input_filename() const {
    return _vm["file"].as<std::string>();
}

std::string cfg::output_filename() const {
    auto type = output_filetype();
    std::string filename = "converted-graph";
    if(_vm.count("out")) {
        filename = _vm["out"].as<std::string>();
    }
    switch(type) {
        case filetype::GML : return filename + ".gml";
        case filetype::SVG : 
        default : return filename + ".svg"; 
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
        std::cout << _desc << std::endl;
        return 1;
    }

    if (_vm.count("file")) {
        std::cout << "Input file is "
                << _vm["file"].as<std::string>() << ".\n";
    } else {
        std::cout << "No input file specified" << std::endl;
        std::cout << _desc << std::endl;
        return 1;
    }
    return 0;
}

int cfg::cull_graph() const {
    if (_vm.count("cull")) {
        int val = _vm["cull"].as<int>();
        return val > 0 ? val : 0;
    }
    return 0;
}

}