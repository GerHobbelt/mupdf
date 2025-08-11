
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/intrusive/any_hook.hpp>
#include <boost/intrusive/avl_set.hpp>
#include <boost/intrusive/avl_set_hook.hpp>
#include <boost/intrusive/avltree.hpp>
#include <boost/intrusive/avltree_algorithms.hpp>
#include <boost/intrusive/bs_set.hpp>
#include <boost/intrusive/bs_set_hook.hpp>
#include <boost/intrusive/bstree.hpp>
#include <boost/intrusive/bstree_algorithms.hpp>
#include <boost/intrusive/circular_list_algorithms.hpp>
#include <boost/intrusive/circular_slist_algorithms.hpp>
#include <boost/intrusive/derivation_value_traits.hpp>
#include <boost/intrusive/hashtable.hpp>
#include <boost/intrusive/intrusive_fwd.hpp>
#include <boost/intrusive/linear_slist_algorithms.hpp>
#include <boost/intrusive/link_mode.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/list_hook.hpp>
#include <boost/intrusive/member_value_traits.hpp>
#include <boost/intrusive/options.hpp>
#include <boost/intrusive/pack_options.hpp>
#include <boost/intrusive/parent_from_member.hpp>
#include <boost/intrusive/pointer_plus_bits.hpp>
#include <boost/intrusive/pointer_rebind.hpp>
#include <boost/intrusive/pointer_traits.hpp>
#include <boost/intrusive/priority_compare.hpp>
#include <boost/intrusive/rbtree.hpp>
#include <boost/intrusive/rbtree_algorithms.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/intrusive/set_hook.hpp>
#include <boost/intrusive/sg_set.hpp>
#include <boost/intrusive/sgtree.hpp>
#include <boost/intrusive/sgtree_algorithms.hpp>
#include <boost/intrusive/slist.hpp>
#include <boost/intrusive/slist_hook.hpp>
#include <boost/intrusive/splay_set.hpp>
#include <boost/intrusive/splaytree.hpp>
#include <boost/intrusive/splaytree_algorithms.hpp>
#include <boost/intrusive/treap.hpp>
#include <boost/intrusive/treap_algorithms.hpp>
#include <boost/intrusive/treap_set.hpp>
#include <boost/intrusive/trivial_value_traits.hpp>
#include <boost/intrusive/unordered_set.hpp>
#include <boost/intrusive/unordered_set_hook.hpp>

