# //          Copyright © Federico Abrignani 2023 - 2024.
# //          Copyright © Salvatore Martorana 2023 - 2024.
# // Distributed under the Boost Software License, Version 1.0.
# //    (See accompanying file LICENSE_1_0.txt or copy at
# //          https://www.boost.org/LICENSE_1_0.txt) 

include(FetchContent)

FetchContent_Declare(
  dc
  GIT_REPOSITORY https://github.com/Sernior/deterministic-concurrency.git
  GIT_TAG v1.4.5
)

FetchContent_MakeAvailable(dc)