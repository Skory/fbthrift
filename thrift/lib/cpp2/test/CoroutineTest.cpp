/*
 * Copyright 2019 Facebook, Inc.
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

#include <exception>

#include <folly/portability/GTest.h>

#include <thrift/lib/cpp2/test/gen-cpp2/Coroutine.h>
#include <thrift/lib/cpp2/util/ScopedServerInterfaceThread.h>

using apache::thrift::Cpp2RequestContext;
using apache::thrift::RequestParams;
using apache::thrift::ScopedServerInterfaceThread;
using apache::thrift::concurrency::ThreadManager;
using folly::EventBase;
using folly::EventBaseManager;

using apache::thrift::test::CoroutineAsyncClient;
using apache::thrift::test::CoroutineSvIf;
using apache::thrift::test::CoroutineSvNull;
using apache::thrift::test::SumRequest;
using apache::thrift::test::SumResponse;

const static int kNoParameterReturnValue = 123;
static int voidReturnValue;

#if FOLLY_HAS_COROUTINES

class CoroutineServiceHandlerCoro : virtual public CoroutineSvIf {
 public:
  void computeSumNoCoro(
      SumResponse& response,
      std::unique_ptr<SumRequest> request) override {
    response.sum = request->x + request->y;
  }

  folly::coro::Task<std::unique_ptr<SumResponse>> co_computeSum(
      std::unique_ptr<SumRequest> request) override {
    auto response = std::make_unique<SumResponse>();
    response->sum = request->x + request->y;
    co_return response;
  }

  folly::coro::Task<std::unique_ptr<SumResponse>> co_computeSumEb(
      std::unique_ptr<SumRequest> request) override {
    auto response = std::make_unique<SumResponse>();
    response->sum = request->x + request->y;
    co_return response;
  }

  folly::coro::Task<int32_t> co_computeSumPrimitive(int32_t x, int32_t y)
      override {
    co_return x + y;
  }

  folly::coro::Task<void> co_computeSumVoid(int32_t x, int32_t y) override {
    voidReturnValue = x + y;
    co_return;
  }

  folly::coro::Task<std::unique_ptr<SumResponse>> co_computeSumThrows(
      std::unique_ptr<SumRequest> /* request */) override {
    co_await std::experimental::suspend_never{};
    throw std::runtime_error("Not implemented");
  }

  folly::coro::Task<int32_t> co_computeSumThrowsPrimitive(int32_t, int32_t)
      override {
    co_await std::experimental::suspend_never{};
    throw std::runtime_error("Not implemented");
  }

  folly::coro::Task<int32_t> co_noParameters() override {
    co_return kNoParameterReturnValue;
  }

  folly::Future<std::unique_ptr<SumResponse>> future_implementedWithFutures()
      override {
    auto result = std::make_unique<SumResponse>();
    result->sum = kNoParameterReturnValue;
    return folly::makeFuture(std::move(result));
  }

  folly::Future<int32_t> future_implementedWithFuturesPrimitive() override {
    return folly::makeFuture(kNoParameterReturnValue);
  }

  folly::coro::Task<int32_t> co_takesRequestParams(
      RequestParams params) override {
    Cpp2RequestContext* requestContext = params.getRequestContext();
    ThreadManager* threadManager = params.getThreadManager();
    EventBase* eventBase = params.getEventBase();
    // It's hard to check that these pointers are what we expect them to be; we
    // can at least make sure that they point to valid memory, though.
    *(volatile char*)requestContext;
    *(volatile char*)threadManager;
    *(volatile char*)eventBase;
    co_return 0;
  }
};

#endif

class CoroutineServiceHandlerFuture : virtual public CoroutineSvIf {
 public:
  void computeSumNoCoro(
      SumResponse& response,
      std::unique_ptr<SumRequest> request) override {
    response.sum = request->x + request->y;
  }

  folly::Future<std::unique_ptr<SumResponse>> future_computeSum(
      std::unique_ptr<SumRequest> request) override {
    auto response = std::make_unique<SumResponse>();
    response->sum = request->x + request->y;
    return folly::makeFuture(std::move(response));
  }

  folly::Future<std::unique_ptr<SumResponse>> future_computeSumEb(
      std::unique_ptr<SumRequest> request) override {
    auto response = std::make_unique<SumResponse>();
    response->sum = request->x + request->y;
    return folly::makeFuture(std::move(response));
  }

  folly::Future<int32_t> future_computeSumPrimitive(int32_t x, int32_t y)
      override {
    return folly::makeFuture(x + y);
  }

  folly::Future<folly::Unit> future_computeSumVoid(int32_t x, int32_t y)
      override {
    voidReturnValue = x + y;
    return folly::makeFuture(folly::Unit{});
  }

  folly::Future<std::unique_ptr<SumResponse>> future_computeSumThrows(
      std::unique_ptr<SumRequest> /* request */) override {
    return folly::makeFuture<std::unique_ptr<SumResponse>>(
        folly::exception_wrapper(
            folly::in_place, std::runtime_error("Not implemented")));
  }

  folly::Future<int32_t> future_computeSumThrowsPrimitive(int32_t, int32_t)
      override {
    return folly::makeFuture<int32_t>(folly::exception_wrapper(
        folly::in_place, std::runtime_error("Not implemented")));
  }

  folly::Future<int32_t> future_noParameters() override {
    return folly::makeFuture(kNoParameterReturnValue);
  }

  folly::Future<std::unique_ptr<SumResponse>> future_implementedWithFutures()
      override {
    auto result = std::make_unique<SumResponse>();
    result->sum = kNoParameterReturnValue;
    return folly::makeFuture(std::move(result));
  }

  folly::Future<int32_t> future_implementedWithFuturesPrimitive() override {
    return folly::makeFuture(kNoParameterReturnValue);
  }

  folly::Future<int32_t> future_takesRequestParams() override {
    // Future functionality is tested elsewhere; we only need this method to
    // make the code compile.
    return folly::makeFuture(0);
  }
};

template <class Handler>
class CoroutineTest : public testing::Test {
 public:
  CoroutineTest()
      : ssit_(std::make_shared<Handler>()),
        client_(ssit_.newClient<CoroutineAsyncClient>(
            *EventBaseManager::get()->getEventBase())) {}

 protected:
  template <typename Func>
  void expectSumResults(Func computeSum) {
    for (int i = 0; i < 10; ++i) {
      for (int j = 0; j < 10; ++j) {
        EXPECT_EQ(i + j, computeSum(i, j));
      }
    }
  }
  ScopedServerInterfaceThread ssit_;
  std::unique_ptr<CoroutineAsyncClient> client_;
};

TYPED_TEST_CASE_P(CoroutineTest);

TYPED_TEST_P(CoroutineTest, SumNoCoro) {
  this->expectSumResults([&](int x, int y) {
    SumRequest request;
    request.x = x;
    request.y = y;
    SumResponse response;
    this->client_->sync_computeSumNoCoro(response, request);
    return response.sum;
  });
}

TYPED_TEST_P(CoroutineTest, Sum) {
  this->expectSumResults([&](int x, int y) {
    SumRequest request;
    request.x = x;
    request.y = y;
    SumResponse response;
    this->client_->sync_computeSum(response, request);
    return response.sum;
  });
}

TYPED_TEST_P(CoroutineTest, SumPrimitive) {
  this->expectSumResults([&](int x, int y) {
    return this->client_->sync_computeSumPrimitive(x, y);
  });
}

TYPED_TEST_P(CoroutineTest, SumVoid) {
  this->expectSumResults([&](int x, int y) {
    this->client_->sync_computeSumVoid(x, y);
    return voidReturnValue;
  });
}

TYPED_TEST_P(CoroutineTest, SumEb) {
  this->expectSumResults([&](int x, int y) {
    SumRequest request;
    request.x = x;
    request.y = y;
    SumResponse response;
    this->client_->sync_computeSumEb(response, request);
    return response.sum;
  });
}

TYPED_TEST_P(CoroutineTest, SumUnimplemented) {
  for (int i = 0; i < 10; ++i) {
    bool error = false;
    try {
      SumRequest request;
      request.x = i;
      request.y = i;
      SumResponse response;
      this->client_->sync_computeSumUnimplemented(response, request);
    } catch (...) {
      error = true;
    }
    EXPECT_TRUE(error);
  }
  this->expectSumResults([&](int x, int y) {
    return this->client_->sync_computeSumPrimitive(x, y);
  });
}

TYPED_TEST_P(CoroutineTest, SumUnimplementedPrimitive) {
  for (int i = 0; i < 10; ++i) {
    bool error = false;
    try {
      this->client_->sync_computeSumUnimplementedPrimitive(i, i);
    } catch (...) {
      error = true;
    }
    EXPECT_TRUE(error);
  }
  this->expectSumResults([&](int x, int y) {
    return this->client_->sync_computeSumPrimitive(x, y);
  });
}

TYPED_TEST_P(CoroutineTest, SumThrows) {
  for (int i = 0; i < 10; ++i) {
    bool error = false;
    try {
      SumRequest request;
      request.x = i;
      request.y = i;
      SumResponse response;
      this->client_->sync_computeSumThrows(response, request);
    } catch (...) {
      error = true;
    }
    EXPECT_TRUE(error);
  }
  this->expectSumResults([&](int x, int y) {
    return this->client_->sync_computeSumPrimitive(x, y);
  });
}

TYPED_TEST_P(CoroutineTest, SumThrowsPrimitive) {
  for (int i = 0; i < 10; ++i) {
    bool error = false;
    try {
      this->client_->sync_computeSumThrowsPrimitive(i, i);
    } catch (...) {
      error = true;
    }
    EXPECT_TRUE(error);
  }
  this->expectSumResults([&](int x, int y) {
    return this->client_->sync_computeSumPrimitive(x, y);
  });
}

TYPED_TEST_P(CoroutineTest, NoParameters) {
  EXPECT_EQ(kNoParameterReturnValue, this->client_->sync_noParameters());
  EXPECT_EQ(kNoParameterReturnValue, this->client_->sync_noParameters());
  EXPECT_EQ(kNoParameterReturnValue, this->client_->sync_noParameters());
}

TYPED_TEST_P(CoroutineTest, ImplemetedWithFutures) {
  SumResponse response;
  response.sum = 0;
  this->client_->sync_implementedWithFutures(response);
  EXPECT_EQ(kNoParameterReturnValue, response.sum);

  response.sum = 0;
  this->client_->sync_implementedWithFutures(response);
  EXPECT_EQ(kNoParameterReturnValue, response.sum);

  response.sum = 0;
  this->client_->sync_implementedWithFutures(response);
  EXPECT_EQ(kNoParameterReturnValue, response.sum);
}

TYPED_TEST_P(CoroutineTest, ImplemetedWithFuturesPrimitive) {
  EXPECT_EQ(
      kNoParameterReturnValue,
      this->client_->sync_implementedWithFuturesPrimitive());
  EXPECT_EQ(
      kNoParameterReturnValue,
      this->client_->sync_implementedWithFuturesPrimitive());
  EXPECT_EQ(
      kNoParameterReturnValue,
      this->client_->sync_implementedWithFuturesPrimitive());
}

TYPED_TEST_P(CoroutineTest, TakesRequestParams) {
  this->client_->sync_takesRequestParams();
}

REGISTER_TYPED_TEST_CASE_P(
    CoroutineTest,
    SumNoCoro,
    Sum,
    SumPrimitive,
    SumVoid,
    SumEb,
    SumUnimplemented,
    SumUnimplementedPrimitive,
    SumThrows,
    SumThrowsPrimitive,
    NoParameters,
    ImplemetedWithFutures,
    ImplemetedWithFuturesPrimitive,
    TakesRequestParams);

INSTANTIATE_TYPED_TEST_CASE_P(
    CoroutineTest,
    CoroutineTest,
    decltype(testing::Types<
#if FOLLY_HAS_COROUTINES
             CoroutineServiceHandlerCoro,
#endif
             CoroutineServiceHandlerFuture>{}));

class CoroutineNullTest : public testing::Test {
 public:
  CoroutineNullTest()
      : ssit_(std::make_shared<CoroutineSvNull>()),
        client_(ssit_.newClient<CoroutineAsyncClient>(
            *EventBaseManager::get()->getEventBase())) {}
  ScopedServerInterfaceThread ssit_;
  std::unique_ptr<CoroutineAsyncClient> client_;
};

TEST_F(CoroutineNullTest, Basics) {
  SumRequest request;
  request.x = 123;
  request.y = 123;

  SumResponse response;

  response.sum = 123;
  client_->sync_computeSumNoCoro(response, request);
  EXPECT_EQ(0, response.sum);

  response.sum = 123;
  client_->sync_computeSum(response, request);
  EXPECT_EQ(0, response.sum);

  EXPECT_EQ(0, client_->sync_computeSumPrimitive(123, 456));

  client_->sync_computeSumVoid(123, 456);

  client_->sync_noParameters();
}

#if FOLLY_HAS_COROUTINES
class CoroutineClientTest : public testing::Test {
 protected:
  CoroutineClientTest()
      : ssit_(std::make_shared<CoroutineServiceHandlerCoro>()),
        client_(ssit_.newClient<CoroutineAsyncClient>(eventBase_)) {}

  ScopedServerInterfaceThread ssit_;
  EventBase eventBase_;
  std::unique_ptr<CoroutineAsyncClient> client_;
};

TEST_F(CoroutineClientTest, SumCoroClient) {
  SumRequest request;
  request.x = 123;
  request.y = 123;

  client_->co_computeSum(request)
      .semi()
      .via(&eventBase_)
      .then([&](folly::Try<SumResponse> response) {
        EXPECT_EQ(246, response->sum);
      })
      .getVia(&eventBase_);
}

TEST_F(CoroutineClientTest, SumPrimitiveCoroClient) {
  client_->co_computeSumPrimitive(12, 408)
      .semi()
      .via(&eventBase_)
      .then([&](folly::Try<int32_t> result) { EXPECT_EQ(420, *result); })
      .getVia(&eventBase_);
}

TEST_F(CoroutineClientTest, SumEbCoroClient) {
  SumRequest request;
  request.x = 77;
  request.y = 941;

  client_->co_computeSumEb(request)
      .semi()
      .via(&eventBase_)
      .then([&](folly::Try<SumResponse> response) {
        EXPECT_EQ(1018, response->sum);
      })
      .getVia(&eventBase_);
}

TEST_F(CoroutineClientTest, SumVoidCoroClient) {
  client_->co_computeSumVoid(11, 22)
      .semi()
      .via(&eventBase_)
      .then([&](folly::Try<folly::Unit>) { EXPECT_EQ(33, voidReturnValue); })
      .getVia(&eventBase_);
}

TEST_F(CoroutineClientTest, SumUnimplementedCoroClient) {
  SumRequest request;
  request.x = 43;
  request.y = 179;
  client_->co_computeSumUnimplemented(request)
      .semi()
      .via(&eventBase_)
      .then([&](folly::Try<SumResponse> response) {
        EXPECT_THROW(
            response.throwIfFailed(), apache::thrift::TApplicationException);
      })
      .getVia(&eventBase_);
}

TEST_F(CoroutineClientTest, SumUnimplementedPrimitiveCoroClient) {
  client_->co_computeSumUnimplementedPrimitive(12, 34)
      .semi()
      .via(&eventBase_)
      .then([&](folly::Try<int32_t> response) {
        EXPECT_THROW(
            response.throwIfFailed(), apache::thrift::TApplicationException);
      })
      .getVia(&eventBase_);
}

TEST_F(CoroutineClientTest, SumThrowsCoroClient) {
  SumRequest request;
  request.x = 290;
  request.y = 321;
  client_->co_computeSumThrows(request)
      .semi()
      .via(&eventBase_)
      .then([&](folly::Try<SumResponse> response) {
        EXPECT_THROW(response.throwIfFailed(), std::exception);
      })
      .getVia(&eventBase_);
}

TEST_F(CoroutineClientTest, SumThrowsPrimitiveCoroClient) {
  client_->co_computeSumThrowsPrimitive(523, 8103)
      .semi()
      .via(&eventBase_)
      .then([&](folly::Try<int32_t> response) {
        EXPECT_THROW(response.throwIfFailed(), std::exception);
      })
      .getVia(&eventBase_);
}

TEST_F(CoroutineClientTest, noParametersCoroClient) {
  client_->co_noParameters()
      .semi()
      .via(&eventBase_)
      .then([&](folly::Try<int32_t> result) {
        EXPECT_EQ(kNoParameterReturnValue, *result);
      })
      .getVia(&eventBase_);
}

TEST_F(CoroutineClientTest, implementedWithFuturesCoroClient) {
  client_->co_implementedWithFutures()
      .semi()
      .via(&eventBase_)
      .then([&](folly::Try<SumResponse> response) {
        EXPECT_EQ(kNoParameterReturnValue, response->sum);
      })
      .getVia(&eventBase_);
}

TEST_F(CoroutineClientTest, implementedWitFuturesPrimitiveCoroClient) {
  client_->co_implementedWithFuturesPrimitive()
      .semi()
      .via(&eventBase_)
      .then([&](folly::Try<int32_t> result) {
        EXPECT_EQ(kNoParameterReturnValue, *result);
      })
      .getVia(&eventBase_);
}

TEST_F(CoroutineClientTest, takesRequestParamsCoroClient) {
  client_->co_takesRequestParams()
      .semi()
      .via(&eventBase_)
      .then([&](folly::Try<int32_t> result) { EXPECT_EQ(0, *result); })
      .getVia(&eventBase_);
}
#endif
