/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <memory>
#include <fnordmetric/query/backends/csv/csvtableref.h>
#include <fnordmetric/query/backends/csv/csvinputstream.h>
#include <fnordmetric/query/svalue.h>
#include <fnordmetric/query/tableref.h>
#include <fnordmetric/query/tablescan.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace query {
namespace csv_backend {

CSVTableRef::CSVTableRef(
    std::unique_ptr<CSVInputStream>&& csv,
    bool headers /* = false */) :
    csv_(std::move(csv)),
    num_cols_(-1),
    min_cols_(0),
    row_index_(0) {
  if (headers) {
    readHeaders();
  }
}

int CSVTableRef::getColumnIndex(const std::string& name) {
  const auto& header = headers_.find(name);

  if (header != headers_.end()) {
    return header->second;
  }

  return -1;
}

void CSVTableRef::executeScan(TableScan* scan) {
  for (;;) {
    std::vector<SValue> row;

    if (!readNextRow(&row)) {
      return;
    }

    if (!scan->nextRow(row.data(), row.size())) {
      return;
    }
  }
}

bool CSVTableRef::readNextRow(std::vector<SValue>* target) {
  std::vector<std::string> row;

  if (!csv_->readNextRow(&row) || row.size() == 0) {
    return false;
  }

  if (row.size() < min_cols_) {
    RAISE(
        util::RuntimeException,
        "csv row #%i does not have enough columns -- "
        "columns found=%i, required=%i\n",
        row_index_,
        row.size(),
        min_cols_); // FIXPAUL filename
  }

  if (num_cols_ == -1) {
    num_cols_ = row.size();
  } else if (row.size() != num_cols_) {
    RAISE(
        util::RuntimeException,
        "csv row #%i does not have the same number of columns as the previous "
        "line -- number of columns found=%i, previous=%i\n",
        row_index_,
        row.size(),
        num_cols_); // FIXPAUL filename
  }

  return true;
}

void CSVTableRef::readHeaders() {
  std::vector<std::string> headers;

  if (!csv_->readNextRow(&headers) || headers.size() == 0) {
    RAISE(util::RuntimeException, "no headers found in CSV file"); // FIXPAUL filename
  }

  size_t col_index = 0;
  for (const auto& header : headers) {
    headers_.emplace(header, col_index++);
  }

  num_cols_ = col_index;
  min_cols_ = col_index;
  row_index_++;
}

}
}
}
