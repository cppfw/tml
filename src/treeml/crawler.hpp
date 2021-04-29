#pragma once

#include "tree.hpp"

namespace treeml{

class crawler{
    forest& b;
    forest::iterator i;
public:
    crawler(forest& b) :
            b(b),
            i(b.begin())
    {
        if(b.size() == 0){
            throw std::logic_error("crawler::crawler() failed, reached end of node list");
        }
    }

    crawler(const forest& b) :
            crawler(const_cast<forest&>(b))
    {}

    tree& get()noexcept{
        ASSERT(this->i != this->b.end())
        return *this->i;
    }

    const tree& get()const noexcept{
        ASSERT(this->i != this->b.end())
        return *this->i;
    }

    crawler& to(const std::string& str);

    const crawler& to(const std::string& str)const{
        return const_cast<crawler*>(this)->to(str);
    }

    template <class predicate_type> crawler& to_if(predicate_type p){
        this->i = std::find_if(this->i, this->b.end(), p);
        if(this->i != this->b.end()){
            return *this;
        }
        throw std::runtime_error("crawler::to_if() failed, reached end of node list");
    }

    template <class predicate_type> const crawler& to_if(predicate_type p)const{
        return const_cast<crawler*>(this)->to_if(p);
    }

    crawler& next();

    const crawler& next()const{
        return const_cast<crawler*>(this)->next();
    }

    crawler in();

    const crawler in()const{
        return const_cast<crawler*>(this)->in();
    }
};

typedef const crawler const_crawler;

}
