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

#include "levenshtein.h"
#include <numeric>
#include <algorithm>

using namespace impala_udf;
using namespace std;

IntVal Levenshtein(
    FunctionContext* ctx, const StringVal& s1, const StringVal& s2) {
  // Adapted from https://bit.ly/2SbDgN4
  // under the Creative Commons Attribution-ShareAlike License

  int s1len = s1.len;
  int s2len = s2.len;

  // error if either input exceeds 255 characters
  if (s1len > 255 || s2len > 255) {
    ctx->SetError("levenshtein argument exceeds maximum length of 255 characters");
    return IntVal(-1);
  }

  // short cut cases:
  // - null strings
  // - zero length strings
  // - identical length and value strings
  if (s1.is_null || s2.is_null) return IntVal::null();
  if (s1len == 0) return IntVal(s2len);
  if (s2len == 0) return IntVal(s1len);
  if (s1len == s2len && memcmp(s1.ptr, s2.ptr, s1len) == 0) return IntVal(0);

  int column_start = 1;

  auto column = reinterpret_cast<int*>(ctx->Allocate(sizeof(int) * (s1len + 1)));

  std::iota(column + column_start - 1, column + s1len + 1, column_start - 1);

  for (int x = column_start; x <= s2len; x++) {
    column[0] = x;
    int last_diagonal = x - column_start;
    for (int y = column_start; y <= s1len; y++) {
      int old_diagonal = column[y];
      auto possibilities = {column[y] + 1, column[y - 1] + 1,
          last_diagonal + (s1.ptr[y - 1] == s2.ptr[x - 1] ? 0 : 1)};
      column[y] = std::min(possibilities);
      last_diagonal = old_diagonal;
    }
  }
  int result = column[s1len];
  ctx->Free(reinterpret_cast<uint8_t*>(column));

  return IntVal(result);
}
