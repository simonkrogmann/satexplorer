#include <vector>

namespace graphdrawer {

template <class T>
class ogdfVector : public std::vector<T> {
public:
    void pushBack(T value) {
        this->push_back(value);
    }
};

}