
#include <string>     // to use std::string, std::to_string() and "+" operator acting on strings 
#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/property_map/dynamic_property_map.hpp>
#include <boost/property_map/property_map.hpp>
#include <fstream>

#include <z3++.h>

using namespace z3;

struct GraphData { std::string Name; };
struct VertexProperty { std::string Name; };
struct EdgeProperty { std::string Name; };

using Graph = boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS, VertexProperty, EdgeProperty, GraphData>;

Graph readGraph(std::string file_name);
void solveMinimum(Graph g);

int main (int argc, char * const argv[]) {

  Graph g = readGraph("graph.xml");

  solveMinimum(g);

	return 0;
}

void solveMinimum(Graph g) {
    context c;
    optimize opt(c);
    params p(c);
    p.set("priority",c.str_symbol("pareto"));
    opt.set(p);

    std::vector<expr> colors = {};
    expr minColor = c.int_const("minColor");
    optimize::handle handleMinColor = opt.minimize(minColor);

    auto name = get(&VertexProperty::Name, g);
    auto vs = boost::vertices(g);

    for (auto vit = vs.first; vit != vs.second; ++vit) {
        std::string colorName = std::string(name[*vit]) + "_color";
        char* charColorName = new char [colorName.length()+1];
        std::strcpy (charColorName, colorName.c_str());

        expr color = c.int_const(charColorName);
        colors.push_back(color);

        opt.add(color >= 0);
        opt.add(color < minColor);
    }

    for (auto vit = vs.first; vit != vs.second; ++vit) {
        
        expr colorNode = colors[*vit];
        
        auto neighbors = boost::adjacent_vertices(*vit, g);
        for (auto nit = neighbors.first; nit != neighbors.second; ++nit){
            expr colorNeighbor = colors[*nit];
            opt.add(colorNode != colorNeighbor);            
        }
    }

    if (sat == opt.check()) {
        std::cout << minColor << ": " << opt.lower(handleMinColor) << std::endl;
    }
}

Graph readGraph(std::string file_name){

  // attach an input stream to the wanted file
  std::ifstream input_stream(file_name);

  // check stream status
  if (!input_stream) std::cerr << "Can't open input file!";

  Graph graph;
  boost::dynamic_properties dp(boost::ignore_other_properties);
  dp.property("Name", boost::get(&VertexProperty::Name, graph));

  boost::read_graphml(input_stream, graph, dp);

  return graph;
}