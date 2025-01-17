/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

#include <gen-py3/hsmodule/clients_wrapper.h>

namespace cpp2 {


HsTestServiceClientWrapper::HsTestServiceClientWrapper(
    std::shared_ptr<::cpp2::HsTestServiceAsyncClient> async_client,
    std::shared_ptr<apache::thrift::RequestChannel> channel) :
    ::thrift::py3::ClientWrapper(std::move(async_client), std::move(channel)) {}


folly::Future<int64_t>
HsTestServiceClientWrapper::init(
    apache::thrift::RpcOptions& rpcOptions,
    int64_t arg_int1) {
  folly::Promise<int64_t> _promise;
  auto _future = _promise.getFuture();
  auto* client = static_cast<::cpp2::HsTestServiceAsyncClient*>(async_client_.get());
  auto callback = std::make_unique<::thrift::py3::FutureCallback<int64_t>>(
    std::move(_promise), rpcOptions, client->recv_wrapped_init, channel_);
  client->init(
    rpcOptions,
    std::move(callback),
    arg_int1
  );
  return _future;
}


} // namespace cpp2
