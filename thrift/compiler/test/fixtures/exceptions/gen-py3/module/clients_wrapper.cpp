/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

#include <src/gen-py3/module/clients_wrapper.h>

namespace cpp2 {


RaiserClientWrapper::RaiserClientWrapper(
    std::shared_ptr<::cpp2::RaiserAsyncClient> async_client,
    std::shared_ptr<apache::thrift::RequestChannel> channel) :
    ::thrift::py3::ClientWrapper(std::move(async_client), std::move(channel)) {}


folly::Future<folly::Unit>
RaiserClientWrapper::doBland(
    apache::thrift::RpcOptions& rpcOptions) {
  folly::Promise<folly::Unit> _promise;
  auto _future = _promise.getFuture();
  auto* client = static_cast<::cpp2::RaiserAsyncClient*>(async_client_.get());
  auto callback = std::make_unique<::thrift::py3::FutureCallback<folly::Unit>>(
    std::move(_promise), rpcOptions, client->recv_wrapped_doBland, channel_);
  client->doBland(
    rpcOptions,
    std::move(callback)
  );
  return _future;
}

folly::Future<folly::Unit>
RaiserClientWrapper::doRaise(
    apache::thrift::RpcOptions& rpcOptions) {
  folly::Promise<folly::Unit> _promise;
  auto _future = _promise.getFuture();
  auto* client = static_cast<::cpp2::RaiserAsyncClient*>(async_client_.get());
  auto callback = std::make_unique<::thrift::py3::FutureCallback<folly::Unit>>(
    std::move(_promise), rpcOptions, client->recv_wrapped_doRaise, channel_);
  client->doRaise(
    rpcOptions,
    std::move(callback)
  );
  return _future;
}

folly::Future<std::string>
RaiserClientWrapper::get200(
    apache::thrift::RpcOptions& rpcOptions) {
  folly::Promise<std::string> _promise;
  auto _future = _promise.getFuture();
  auto* client = static_cast<::cpp2::RaiserAsyncClient*>(async_client_.get());
  auto callback = std::make_unique<::thrift::py3::FutureCallback<std::string>>(
    std::move(_promise), rpcOptions, client->recv_wrapped_get200, channel_);
  client->get200(
    rpcOptions,
    std::move(callback)
  );
  return _future;
}

folly::Future<std::string>
RaiserClientWrapper::get500(
    apache::thrift::RpcOptions& rpcOptions) {
  folly::Promise<std::string> _promise;
  auto _future = _promise.getFuture();
  auto* client = static_cast<::cpp2::RaiserAsyncClient*>(async_client_.get());
  auto callback = std::make_unique<::thrift::py3::FutureCallback<std::string>>(
    std::move(_promise), rpcOptions, client->recv_wrapped_get500, channel_);
  client->get500(
    rpcOptions,
    std::move(callback)
  );
  return _future;
}


} // namespace cpp2
