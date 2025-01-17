/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

#pragma once
#include <src/gen-cpp2/Raiser.h>

#include <folly/futures/Future.h>
#include <folly/futures/Promise.h>
#include <folly/Unit.h>
#include <thrift/lib/py3/clientcallbacks.h>
#include <thrift/lib/py3/client_wrapper.h>

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace cpp2 {

class RaiserClientWrapper : public ::thrift::py3::ClientWrapper {
  public:
    explicit RaiserClientWrapper(
      std::shared_ptr<::cpp2::RaiserAsyncClient> async_client,
      std::shared_ptr<apache::thrift::RequestChannel> channel);

    folly::Future<folly::Unit> doBland(
      apache::thrift::RpcOptions& rpcOptions);
    folly::Future<folly::Unit> doRaise(
      apache::thrift::RpcOptions& rpcOptions);
    folly::Future<std::string> get200(
      apache::thrift::RpcOptions& rpcOptions);
    folly::Future<std::string> get500(
      apache::thrift::RpcOptions& rpcOptions);
};


} // namespace cpp2
