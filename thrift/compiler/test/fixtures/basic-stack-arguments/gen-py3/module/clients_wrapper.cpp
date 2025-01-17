/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

#include <src/gen-py3/module/clients_wrapper.h>

namespace cpp2 {


MyServiceClientWrapper::MyServiceClientWrapper(
    std::shared_ptr<::cpp2::MyServiceAsyncClient> async_client,
    std::shared_ptr<apache::thrift::RequestChannel> channel) :
    ::thrift::py3::ClientWrapper(std::move(async_client), std::move(channel)) {}


folly::Future<bool>
MyServiceClientWrapper::hasDataById(
    apache::thrift::RpcOptions& rpcOptions,
    int64_t arg_id) {
  folly::Promise<bool> _promise;
  auto _future = _promise.getFuture();
  auto* client = static_cast<::cpp2::MyServiceAsyncClient*>(async_client_.get());
  auto callback = std::make_unique<::thrift::py3::FutureCallback<bool>>(
    std::move(_promise), rpcOptions, client->recv_wrapped_hasDataById, channel_);
  client->hasDataById(
    rpcOptions,
    std::move(callback),
    arg_id
  );
  return _future;
}

folly::Future<std::string>
MyServiceClientWrapper::getDataById(
    apache::thrift::RpcOptions& rpcOptions,
    int64_t arg_id) {
  folly::Promise<std::string> _promise;
  auto _future = _promise.getFuture();
  auto* client = static_cast<::cpp2::MyServiceAsyncClient*>(async_client_.get());
  auto callback = std::make_unique<::thrift::py3::FutureCallback<std::string>>(
    std::move(_promise), rpcOptions, client->recv_wrapped_getDataById, channel_);
  client->getDataById(
    rpcOptions,
    std::move(callback),
    arg_id
  );
  return _future;
}

folly::Future<folly::Unit>
MyServiceClientWrapper::putDataById(
    apache::thrift::RpcOptions& rpcOptions,
    int64_t arg_id,
    std::string arg_data) {
  folly::Promise<folly::Unit> _promise;
  auto _future = _promise.getFuture();
  auto* client = static_cast<::cpp2::MyServiceAsyncClient*>(async_client_.get());
  auto callback = std::make_unique<::thrift::py3::FutureCallback<folly::Unit>>(
    std::move(_promise), rpcOptions, client->recv_wrapped_putDataById, channel_);
  client->putDataById(
    rpcOptions,
    std::move(callback),
    arg_id,
    arg_data
  );
  return _future;
}

folly::Future<folly::Unit>
MyServiceClientWrapper::lobDataById(
    apache::thrift::RpcOptions& rpcOptions,
    int64_t arg_id,
    std::string arg_data) {
  folly::Promise<folly::Unit> _promise;
  auto _future = _promise.getFuture();
  auto* client = static_cast<::cpp2::MyServiceAsyncClient*>(async_client_.get());
  auto callback = std::make_unique<::apache::thrift::OneWayFutureCallback>(
    std::move(_promise), channel_);
  client->lobDataById(
    rpcOptions,
    std::move(callback),
    arg_id,
    arg_data
  );
  return _future;
}


MyServiceFastClientWrapper::MyServiceFastClientWrapper(
    std::shared_ptr<::cpp2::MyServiceFastAsyncClient> async_client,
    std::shared_ptr<apache::thrift::RequestChannel> channel) :
    ::thrift::py3::ClientWrapper(std::move(async_client), std::move(channel)) {}


folly::Future<bool>
MyServiceFastClientWrapper::hasDataById(
    apache::thrift::RpcOptions& rpcOptions,
    int64_t arg_id) {
  folly::Promise<bool> _promise;
  auto _future = _promise.getFuture();
  auto* client = static_cast<::cpp2::MyServiceFastAsyncClient*>(async_client_.get());
  auto callback = std::make_unique<::thrift::py3::FutureCallback<bool>>(
    std::move(_promise), rpcOptions, client->recv_wrapped_hasDataById, channel_);
  client->hasDataById(
    rpcOptions,
    std::move(callback),
    arg_id
  );
  return _future;
}

folly::Future<std::string>
MyServiceFastClientWrapper::getDataById(
    apache::thrift::RpcOptions& rpcOptions,
    int64_t arg_id) {
  folly::Promise<std::string> _promise;
  auto _future = _promise.getFuture();
  auto* client = static_cast<::cpp2::MyServiceFastAsyncClient*>(async_client_.get());
  auto callback = std::make_unique<::thrift::py3::FutureCallback<std::string>>(
    std::move(_promise), rpcOptions, client->recv_wrapped_getDataById, channel_);
  client->getDataById(
    rpcOptions,
    std::move(callback),
    arg_id
  );
  return _future;
}

folly::Future<folly::Unit>
MyServiceFastClientWrapper::putDataById(
    apache::thrift::RpcOptions& rpcOptions,
    int64_t arg_id,
    std::string arg_data) {
  folly::Promise<folly::Unit> _promise;
  auto _future = _promise.getFuture();
  auto* client = static_cast<::cpp2::MyServiceFastAsyncClient*>(async_client_.get());
  auto callback = std::make_unique<::thrift::py3::FutureCallback<folly::Unit>>(
    std::move(_promise), rpcOptions, client->recv_wrapped_putDataById, channel_);
  client->putDataById(
    rpcOptions,
    std::move(callback),
    arg_id,
    arg_data
  );
  return _future;
}

folly::Future<folly::Unit>
MyServiceFastClientWrapper::lobDataById(
    apache::thrift::RpcOptions& rpcOptions,
    int64_t arg_id,
    std::string arg_data) {
  folly::Promise<folly::Unit> _promise;
  auto _future = _promise.getFuture();
  auto* client = static_cast<::cpp2::MyServiceFastAsyncClient*>(async_client_.get());
  auto callback = std::make_unique<::apache::thrift::OneWayFutureCallback>(
    std::move(_promise), channel_);
  client->lobDataById(
    rpcOptions,
    std::move(callback),
    arg_id,
    arg_data
  );
  return _future;
}


} // namespace cpp2
