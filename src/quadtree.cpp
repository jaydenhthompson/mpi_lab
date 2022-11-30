#include "quadtree.hpp"

#include <iostream>

std::tuple<double, double> window::getCenter()
{
    double x = (x_max + x_min) / 2.0;
    double y = (y_max + y_min) / 2.0;
    return std::make_tuple(x, y);
}

node::node()
{
    this->nw = nullptr;
    this->ne = nullptr;
    this->sw = nullptr;
    this->se = nullptr;
}

node::node(body * b)
{
    this->type = BODY;
    this->self_body = b;
    this->self_window = nullptr;
    node();
}

node::node(window *w)
{
    this->type = BODY;
    this->self_body = nullptr;
    this->self_window = w;
    node();
}

quadtree::quadtree(std::vector<body*> init, window *wind)
{
    root = new node(wind);
    root->self_body = new body();
    std::tie(root->self_body->x, root->self_body->y) = wind->getCenter();
    for(auto& e : init) 
    {
        addNode(e);
    }
}

void quadtree::addNode(body* b)
{
    if (!b) {
        std::cerr << "unexpected null body" << std::endl;
        exit(1);
    }
    node* cur = this->root;
    while(cur) 
    {
        cur->self_body->mass += b->mass;

    }
}

// double x_cen, y_cen;
// std::tie(x_cen, y_cen) = wind->getCenter();
// root->nw = new node(new window(wind->x_min, x_cen, y_cen, wind->y_max));
// root->ne = new node(new window(x_cen, wind->x_max, y_cen, wind->y_max));
// root->sw = new node(new window(wind->x_min, x_cen, wind->y_min, y_cen));
// root->se = new node(new window(x_cen, wind->x_max, wind->y_min, y_cen));
