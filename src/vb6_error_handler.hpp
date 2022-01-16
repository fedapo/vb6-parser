//: vb6_error_handler.hpp

// vb6_parser
// Copyright (c) 2022 Federico Aponte
// This code is licensed under GNU Software License (see LICENSE.txt for details)

#pragma once

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>

namespace vb6_grammar {

  namespace x3 = boost::spirit::x3;

  // x3::position_tagged
  // annotation_base
  // error_handler_base

  // our error handler
  template <typename Iterator>
  using vb6_error_handler = x3::error_handler<Iterator>;

  // tag used to get our error handler from the context
  //using vb6_error_handler_tag = x3::error_handler_tag;
  struct vb6_error_handler_tag;

#if 0
  struct error_handler_base
  {
    template <typename Iterator, typename Exception, typename Context>
    x3::error_handler_result on_error(Iterator& /*first*/, Iterator const& /*last*/,
                                      Exception const& x, Context const& context)
    {
      std::string message = "Error! Expecting: " + x.which() + " here:";
      auto& error_handler = x3::get<vb6_error_handler_tag>(context).get();
      error_handler(x.where(), message);
      return x3::error_handler_result::fail;
    }
  };
#endif
}