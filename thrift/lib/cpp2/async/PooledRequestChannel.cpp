/*
 * Copyright 2018-present Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <thrift/lib/cpp2/async/PooledRequestChannel.h>

#include <thrift/lib/cpp2/async/FutureRequest.h>

#include <folly/futures/Future.h>

namespace apache {
namespace thrift {

uint16_t PooledRequestChannel::getProtocolId() {
  auto executor = executor_.lock();
  if (!executor) {
    throw std::logic_error("IO executor already destroyed.");
  }
  folly::call_once(protocolIdInitFlag_, [&] {
    auto evb = executor->getEventBase();
    evb->runImmediatelyOrRunInEventBaseThreadAndWait(
        [&] { protocolId_ = impl(*evb).getProtocolId(); });
  });

  return protocolId_;
}

void PooledRequestChannel::sendRequestImpl(
    RpcKind rpcKind,
    RpcOptions& options,
    std::unique_ptr<folly::IOBuf> buf,
    std::shared_ptr<transport::THeader> header,
    RequestClientCallback::Ptr cob) {
  auto executor = executor_.lock();
  if (!executor) {
    throw std::logic_error("IO executor already destroyed.");
  }
  auto evb = executor->getEventBase();

  evb->runInEventBaseThread([this,
                             keepAlive = getKeepAliveToken(evb),
                             options = std::move(options),
                             rpcKind,
                             cob = std::move(cob),
                             buf = std::move(buf),
                             header = std::move(header)]() mutable {
    switch (rpcKind) {
      case RpcKind::SINGLE_REQUEST_NO_RESPONSE:
        impl(*keepAlive)
            .sendRequestNoResponse(
                options, std::move(buf), std::move(header), std::move(cob));
        break;
      case RpcKind::SINGLE_REQUEST_SINGLE_RESPONSE:
        impl(*keepAlive)
            .sendRequestResponse(
                options, std::move(buf), std::move(header), std::move(cob));
        break;
      case RpcKind::SINGLE_REQUEST_STREAMING_RESPONSE:
        impl(*keepAlive)
            .sendRequestStream(
                options, std::move(buf), std::move(header), std::move(cob));
        break;
      default:
        folly::assume_unreachable();
        break;
    };
  });
}

namespace {
template <bool oneWay>
class ExecutorRequestCallback final : public RequestClientCallback {
 public:
  ExecutorRequestCallback(
      RequestClientCallback::Ptr cb,
      folly::Executor::KeepAlive<> executorKeepAlive)
      : executorKeepAlive_(std::move(executorKeepAlive)), cb_(std::move(cb)) {
    CHECK(executorKeepAlive_);
  }

  void onRequestSent() noexcept override {
    if (oneWay) {
      executorKeepAlive_.get()->add(
          [cb = std::move(cb_)]() mutable { cb.release()->onRequestSent(); });
      delete this;
    } else {
      requestSent_ = true;
    }
  }
  void onResponse(ClientReceiveState&& rs) noexcept override {
    executorKeepAlive_.get()->add([requestSent = requestSent_,
                                   cb = std::move(cb_),
                                   rs = std::move(rs)]() mutable {
      if (requestSent) {
        cb->onRequestSent();
      }
      cb.release()->onResponse(std::move(rs));
    });
    delete this;
  }
  void onResponseError(folly::exception_wrapper ex) noexcept override {
    executorKeepAlive_.get()->add([requestSent = requestSent_,
                                   cb = std::move(cb_),
                                   ex = std::move(ex)]() mutable {
      if (requestSent) {
        cb->onRequestSent();
      }
      cb.release()->onResponseError(std::move(ex));
    });
    delete this;
  }

 private:
  bool requestSent_{false};
  folly::Executor::KeepAlive<> executorKeepAlive_;
  RequestClientCallback::Ptr cb_;
};
} // namespace

void PooledRequestChannel::sendRequestResponse(
    RpcOptions& options,
    std::unique_ptr<folly::IOBuf> buf,
    std::shared_ptr<transport::THeader> header,
    RequestClientCallback::Ptr cob) {
  if (!cob->isInlineSafe()) {
    cob = RequestClientCallback::Ptr(new ExecutorRequestCallback<false>(
        std::move(cob), getKeepAliveToken(callbackExecutor_)));
  }
  sendRequestImpl(
      RpcKind::SINGLE_REQUEST_SINGLE_RESPONSE,
      options,
      std::move(buf),
      std::move(header),
      std::move(cob));
}

void PooledRequestChannel::sendRequestNoResponse(
    RpcOptions& options,
    std::unique_ptr<folly::IOBuf> buf,
    std::shared_ptr<transport::THeader> header,
    RequestClientCallback::Ptr cob) {
  if (!cob->isInlineSafe()) {
    cob = RequestClientCallback::Ptr(new ExecutorRequestCallback<true>(
        std::move(cob), getKeepAliveToken(callbackExecutor_)));
  }
  sendRequestImpl(
      RpcKind::SINGLE_REQUEST_NO_RESPONSE,
      options,
      std::move(buf),
      std::move(header),
      std::move(cob));
}

void PooledRequestChannel::sendRequestStream(
    RpcOptions& options,
    std::unique_ptr<folly::IOBuf> buf,
    std::shared_ptr<transport::THeader> header,
    RequestClientCallback::Ptr cob) {
  if (!cob->isInlineSafe()) {
    cob = RequestClientCallback::Ptr(new ExecutorRequestCallback<false>(
        std::move(cob), getKeepAliveToken(callbackExecutor_)));
  }
  sendRequestImpl(
      RpcKind::SINGLE_REQUEST_STREAMING_RESPONSE,
      options,
      std::move(buf),
      std::move(header),
      std::move(cob));
}

PooledRequestChannel::Impl& PooledRequestChannel::impl(folly::EventBase& evb) {
  DCHECK(evb.inRunningEventBaseThread());

  return impl_.getOrCreateFn(evb, [this, &evb] { return implCreator_(evb); });
}
} // namespace thrift
} // namespace apache
