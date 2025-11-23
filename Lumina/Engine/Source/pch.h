#pragma once

// ===================
// Standard Library
// ===================
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <sstream>
#include <utility>
#include <cstdint>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <bitset>
#include <algorithm>
#include <functional>
#include <cmath>
#include <numeric>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <type_traits>
#include <optional>
#include <variant>
#include <span>
#include <limits>
#include <stdexcept>
#include <cassert>

// ===================
// EASTL
// ===================
#include <eastl/type_traits.h>
#include <eastl/random.h>
#include <eastl/utility.h>
#include <eastl/array.h>
#include <eastl/vector.h>
#include <eastl/map.h>
#include <eastl/unordered_map.h>
#include <eastl/set.h>
#include <eastl/unordered_set.h>
#include <eastl/string.h>
#include <eastl/algorithm.h>
#include <eastl/functional.h>
#include <eastl/memory.h>

// ===================
// Third-Party Libraries
// ===================
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <entt/entt.hpp>
#include <sol/sol.hpp>
#include <Jolt/Jolt.h>
#include <spdlog/spdlog.h>

// ===================
// Engine Core
// ===================
#include "Core/LuminaMacros.h"
#include "Lumina.h"
#include "Core/Math/Math.h"
#include "Core/Assertions/Assert.h"
#include "Log/Log.h"
#include "Memory/Memory.h"
#include "Containers/String.h"
#include "Containers/Name.h"
#include "Containers/Array.h"
#include "Platform/Platform.h"
#include "Platform/GenericPlatform.h"
#include "Renderer/RHIIncl.h"


