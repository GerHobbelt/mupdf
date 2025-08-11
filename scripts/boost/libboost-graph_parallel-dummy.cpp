
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/graph/accounting.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/distributed/adjlist/handlers.hpp>
#include <boost/graph/distributed/adjlist/initialize.hpp>
#include <boost/graph/distributed/adjlist/redistribute.hpp>
#include <boost/graph/distributed/adjlist/serialization.hpp>
#include <boost/graph/distributed/betweenness_centrality.hpp>
#include <boost/graph/distributed/boman_et_al_graph_coloring.hpp>
#include <boost/graph/distributed/breadth_first_search.hpp>
#include <boost/graph/distributed/compressed_sparse_row_graph.hpp>
#include <boost/graph/distributed/concepts.hpp>
#include <boost/graph/distributed/connected_components.hpp>
#include <boost/graph/distributed/connected_components_parallel_search.hpp>
#include <boost/graph/distributed/crauser_et_al_shortest_paths.hpp>
#include <boost/graph/distributed/dehne_gotz_min_spanning_tree.hpp>
#include <boost/graph/distributed/delta_stepping_shortest_paths.hpp>
#include <boost/graph/distributed/depth_first_search.hpp>
#include <boost/graph/distributed/dijkstra_shortest_paths.hpp>
#include <boost/graph/distributed/distributed_graph_utility.hpp>
#include <boost/graph/distributed/eager_dijkstra_shortest_paths.hpp>
#include <boost/graph/distributed/filtered_graph.hpp>
#include <boost/graph/distributed/fruchterman_reingold.hpp>
#include <boost/graph/distributed/graphviz.hpp>
#include <boost/graph/distributed/hohberg_biconnected_components.hpp>
#include <boost/graph/distributed/local_subgraph.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/distributed/named_graph.hpp>
#include <boost/graph/distributed/one_bit_color_map.hpp>
#include <boost/graph/distributed/page_rank.hpp>
#include <boost/graph/distributed/queue.hpp>
#include <boost/graph/distributed/reverse_graph.hpp>
#include <boost/graph/distributed/rmat_graph_generator.hpp>
#include <boost/graph/distributed/selector.hpp>
#include <boost/graph/distributed/shuffled_distribution.hpp>
#include <boost/graph/distributed/st_connected.hpp>
#include <boost/graph/distributed/strong_components.hpp>
#include <boost/graph/distributed/two_bit_color_map.hpp>
#include <boost/graph/distributed/unsafe_serialize.hpp>
#include <boost/graph/distributed/vertex_list_adaptor.hpp>
#include <boost/graph/parallel/algorithm.hpp>
#include <boost/graph/parallel/basic_reduce.hpp>
#include <boost/graph/parallel/container_traits.hpp>
#include <boost/graph/parallel/distribution.hpp>
#include <boost/graph/parallel/process_group.hpp>
#include <boost/graph/parallel/properties.hpp>
#include <boost/graph/parallel/simple_trigger.hpp>

