#include <string>
#include <boost/program_options.hpp>
#include <ogdfWrapper/types.hpp>

namespace graphdrawer {

class cfg {
public:
    cfg(int ac, char* av[]);

    cfg(const cfg& other) = delete;
    const cfg& operator=(const cfg& other) = delete;

    std::string input_filename() const;
    std::string output_filename() const;
    filetype output_filetype() const;
    bool validate_config() const;

private:
    boost::program_options::variables_map _vm;
    boost::program_options::options_description _desc;

    void init_options();
};

}