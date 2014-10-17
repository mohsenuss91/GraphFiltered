#include <iostream>

#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>

#define GRAPH_WITH_FILTER 1
#define GRAPH_WITHOUT_FILTER 0

struct Node
{
    int number;
};

struct Edge
{
    int distance;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, Node, Edge> graph_t;
typedef boost::graph_traits<graph_t>::vertex_descriptor v;
typedef boost::graph_traits<graph_t>::edge_descriptor e;

struct FilterEdge
{
    graph_t *gr;
    FilterEdge() {}
    FilterEdge(graph_t& g): gr(&g) {}
    bool operator() (const e &_e) const {
        if((*gr)[_e].distance < 5) return 0;
        return 1;
    }
};

struct FilterVertex
{
    graph_t *gr;
    FilterVertex() {}
    FilterVertex(graph_t& g): gr(&g) {}
    bool operator() (const v &_v) const {return 1;}
};

int main()
{
    bool b_use_filtered_graph = GRAPH_WITHOUT_FILTER;
    std::vector <v> nodes;
    std::vector <e> edges;
    graph_t g;

    for(int i = 0; i < 7; i++)
        nodes.push_back(boost::add_vertex(g));

    //задаём номера вершин
    g[nodes[0]].number = 1;
    g[nodes[1]].number = 2;
    g[nodes[2]].number = 3;
    g[nodes[3]].number = 4;
    g[nodes[4]].number = 5;
    g[nodes[5]].number = 6;
    g[nodes[6]].number = 7;

    //массив пар - какие вершины связаны с какими
    std::pair<int, int> edge_array[] = {{1,2}, {1,3}, {1,4}, {2,3}, {2,6}, {3,4}, {3,6}, {4,5}, {4,7}, {5,6}};

    //
    for(int i = 0; i < (int)sizeof(edge_array) / (int)sizeof(edge_array[0]); i++)
        edges.push_back(boost::add_edge(nodes[edge_array[i].first - 1], nodes[edge_array[i].second - 1], g).first);

    std::cout << "Count of edges: " << (int)sizeof(edge_array) / (int)sizeof(edge_array[0]) << std::endl;

    //задаём веса ребёр
    g[edges[0]].distance = 7;
    g[edges[1]].distance = 9;
    g[edges[2]].distance = 14;
    g[edges[3]].distance = 10;
    g[edges[4]].distance = 13;
    g[edges[5]].distance = 2;
    g[edges[6]].distance = 11;
    g[edges[7]].distance = 9;
    g[edges[8]].distance = 2;
    g[edges[9]].distance = 6;

    FilterEdge filterEdge(g);
    FilterVertex filterVertex(g);
    boost::filtered_graph <graph_t, FilterEdge, FilterVertex> fg(g, filterEdge, filterVertex);

    //ищем кратчайший путь от 1 до остальных вершин
    std::vector<int> d(num_vertices(g));
    std::vector<v> p(boost::num_vertices(g));

    if(b_use_filtered_graph) {
        boost::dijkstra_shortest_paths(fg, nodes[0], boost::weight_map(get(&Edge::distance, fg))
                .distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, fg)))
                .predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, fg))));
        std::cout << "Path length from vertice " << fg[nodes[0]].number << " to vertice " << fg[nodes[4]].number
                << ": " << d[4] << std::endl;
        std::cout << "Path: ";
        boost::graph_traits<graph_t>::vertex_descriptor u = nodes[4];
        while(p[u] != u) {
            std::cout << fg[p[u]].number << ' ';
            u = p[u];
        }
        std::cout<< std::endl;
        boost::print_graph(fg);
        boost::print_edges(fg, "1234567");
    }
    else {
        boost::dijkstra_shortest_paths(g, nodes[0], boost::weight_map(get(&Edge::distance, g))
                .distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g)))
                .predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g))));
        std::cout << "Path length from vertice " << g[nodes[0]].number << " to vertice " << g[nodes[4]].number
                << ": " << d[4] << std::endl;
        std::cout << "Path: ";
        boost::graph_traits<graph_t>::vertex_descriptor u = nodes[4];
        while(p[u] != u) {
            std::cout << g[p[u]].number << ' ';
            u = p[u];
        }
        std::cout<< std::endl;
        boost::print_graph(g);
        boost::print_edges(g, "1234567");
    }
}
