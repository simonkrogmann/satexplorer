#include <boost/program_options.hpp>
#include <ogdfWrapper/types.hpp>
#include <string>

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
    int cull_graph() const;
    int node_width() const;
    int node_height() const;
    bool config_valid() const;

private:
    boost::program_options::variables_map _vm;
    bool _config_valid;

    void init_options();
};
}  // namespace graphdrawer
