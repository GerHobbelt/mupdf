
// fix ubiquitous warning C5204: 'boost::blah::blah<Blah>': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
//
// while this generally CAN/MAY be an issue, we brutally assume that the Boost Boys know their shit and NEVER make this mistake, i.e. the classes/structs in question are *implicitly `final`*!
// To play it kinda safe, we apply this 'please shut up' fix to this dummy=see-if-we-can-compile-Ev'ryTing source file only:

#if defined(_MSC_VER)
#pragma warning(disable: 5204)

// fix/shut up warning C4266: 'void boost::blah::blah::blah(...)': no override available for virtual member function from base 'boost::blah::blah'; function is hidden
#pragma warning(disable: 4266)
#endif

#include <boost/url.hpp>
#include <boost/url/authority_view.hpp>
#include <boost/url/decode_view.hpp>
#include <boost/url/encode.hpp>
#include <boost/url/encoding_opts.hpp>
#include <boost/url/error.hpp>
#include <boost/url/error_types.hpp>
#include <boost/url/format.hpp>
#include <boost/url/grammar.hpp>
#include <boost/url/grammar/all_chars.hpp>
#include <boost/url/grammar/alnum_chars.hpp>
#include <boost/url/grammar/alpha_chars.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/ci_string.hpp>
#include <boost/url/grammar/dec_octet_rule.hpp>
#include <boost/url/grammar/delim_rule.hpp>
#include <boost/url/grammar/digit_chars.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/hexdig_chars.hpp>
#include <boost/url/grammar/literal_rule.hpp>
#include <boost/url/grammar/lut_chars.hpp>
#include <boost/url/grammar/not_empty_rule.hpp>
#include <boost/url/grammar/optional_rule.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/range_rule.hpp>
#include <boost/url/grammar/recycled.hpp>
#include <boost/url/grammar/string_token.hpp>
#include <boost/url/grammar/string_view_base.hpp>
#include <boost/url/grammar/token_rule.hpp>
#include <boost/url/grammar/tuple_rule.hpp>
#include <boost/url/grammar/type_traits.hpp>
#include <boost/url/grammar/unsigned_rule.hpp>
#include <boost/url/grammar/variant_rule.hpp>
#include <boost/url/grammar/vchars.hpp>
#include <boost/url/host_type.hpp>
#include <boost/url/ignore_case.hpp>
#include <boost/url/ipv4_address.hpp>
#include <boost/url/ipv6_address.hpp>
#include <boost/url/optional.hpp>
#include <boost/url/param.hpp>
#include <boost/url/params_base.hpp>
#include <boost/url/params_encoded_base.hpp>
#include <boost/url/params_encoded_ref.hpp>
#include <boost/url/params_encoded_view.hpp>
#include <boost/url/params_ref.hpp>
#include <boost/url/params_view.hpp>
#include <boost/url/parse.hpp>
#include <boost/url/parse_path.hpp>
#include <boost/url/parse_query.hpp>
#include <boost/url/pct_string_view.hpp>
#include <boost/url/rfc/absolute_uri_rule.hpp>
#include <boost/url/rfc/authority_rule.hpp>
#include <boost/url/rfc/gen_delim_chars.hpp>
#include <boost/url/rfc/ipv4_address_rule.hpp>
#include <boost/url/rfc/ipv6_address_rule.hpp>
#include <boost/url/rfc/origin_form_rule.hpp>
#include <boost/url/rfc/pchars.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/rfc/relative_ref_rule.hpp>
#include <boost/url/rfc/reserved_chars.hpp>
#include <boost/url/rfc/sub_delim_chars.hpp>
#include <boost/url/rfc/unreserved_chars.hpp>
#include <boost/url/rfc/uri_reference_rule.hpp>
#include <boost/url/rfc/uri_rule.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/segments_base.hpp>
#include <boost/url/segments_encoded_base.hpp>
#include <boost/url/segments_encoded_ref.hpp>
#include <boost/url/segments_encoded_view.hpp>
#include <boost/url/segments_ref.hpp>
#include <boost/url/segments_view.hpp>
#include <boost/url/src.hpp>
#include <boost/url/static_url.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/url.hpp>
#include <boost/url/url_base.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url/url_view_base.hpp>
#include <boost/url/urls.hpp>
#include <boost/url/variant.hpp>

