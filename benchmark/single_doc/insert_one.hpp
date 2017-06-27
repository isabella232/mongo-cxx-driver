// Copyright 2017 MongoDB Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "../microbench.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace benchmark {

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

class insert_one : public microbench {
   public:
    insert_one() = delete;

    insert_one(std::int32_t iter) : microbench{iter}, _conn{mongocxx::uri{}}, _iter{iter} {}

    void setup(bsoncxx::stdx::string_view);

    void before_task();

    void teardown();

   protected:
    void task();

   private:
    mongocxx::client _conn;
    std::int32_t _iter;
    bsoncxx::stdx::optional<bsoncxx::document::value> _doc;
    mongocxx::collection _coll;
};

void insert_one::setup(bsoncxx::stdx::string_view json_file) {
    _doc = parse_json_file_to_documents(json_file)[0];
    mongocxx::database db = _conn["perftest"];
    db.drop();
}

void insert_one::before_task() {
    auto coll = _conn["perftest"]["corpus"];
    coll.drop();
    _conn["perftest"].create_collection("corpus");
    _coll = _conn["perftest"]["corpus"];
}

void insert_one::teardown() {
    mongocxx::database db = _conn["perftest"];
    db.drop();
}

void insert_one::task() {
    for (std::int32_t i = 0; i < _iter; i++) {
        _coll.insert_one(_doc->view());
    }
}
}