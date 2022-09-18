# 检查编译器
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckCXXSourceRuns)
include(CheckCXXSymbolExists)
include(CheckFunctionExists)
include(CheckIncludeFileCXX)
include(CheckLibraryExists)
include(CheckStructHasMember)
include(CheckTypeSize)

check_include_file_cxx(dlfcn.h HAVE_DLFCN_H)
check_include_file_cxx(execinfo.h HAVE_EXECINFO_H)
check_include_file_cxx(glob.h HAVE_GLOB_H)
check_include_file_cxx(inttypes.h HAVE_INTTYPES_H)
check_include_file_cxx(memory.h HAVE_MEMORY_H)
check_include_file_cxx(pwd.h HAVE_PWD_H)
check_include_file_cxx(stdint.h HAVE_STDINT_H)
check_include_file_cxx(strings.h HAVE_STRINGS_H)
check_include_file_cxx(sys/stat.h HAVE_SYS_STAT_H)
check_include_file_cxx(sys/syscall.h HAVE_SYS_SYSCALL_H)
check_include_file_cxx(sys/time.h HAVE_SYS_TIME_H)
check_include_file_cxx(sys/types.h HAVE_SYS_TYPES_H)
check_include_file_cxx(sys/utsname.h HAVE_SYS_UTSNAME_H)
check_include_file_cxx(sys/wait.h HAVE_SYS_WAIT_H)
check_include_file_cxx(syscall.h HAVE_SYSCALL_H)
check_include_file_cxx(ucontext.h HAVE_UCONTEXT_H)
check_include_file_cxx(unistd.h HAVE_UNISTD_H)

check_include_file_cxx("ext/hash_map" HAVE_EXT_HASH_MAP)
check_include_file_cxx("ext/hash_set" HAVE_EXT_HASH_SET)
check_include_file_cxx("ext/slist" HAVE_EXT_SLIST)
check_include_file_cxx("tr1/unordered_map" HAVE_TR1_UNORDERED_MAP)
check_include_file_cxx("tr1/unordered_set" HAVE_TR1_UNORDERED_SET)
check_include_file_cxx("unordered_map" HAVE_UNORDERED_MAP)
check_include_file_cxx("unordered_set" HAVE_UNORDERED_SET)

check_type_size("unsigned __int16" HAVE___UINT16 LANGUAGE CXX)
check_type_size(u_int16_t HAVE_U_INT16_T LANGUAGE CXX)
check_type_size(uint16_t HAVE_UINT16_T LANGUAGE CXX)

check_function_exists(dladdr HAVE_DLADDR)
check_function_exists(fcntl HAVE_FCNTL)
check_function_exists(pread HAVE_PREAD)
check_function_exists(pwrite HAVE_PWRITE)
check_function_exists(sigaction HAVE_SIGACTION)
check_function_exists(sigaltstack HAVE_SIGALTSTACK)

check_cxx_compiler_flag(-Wdeprecated HAVE_NO_DEPRECATED)
check_cxx_compiler_flag(-Wunnamed-type-template-args
    HAVE_NO_UNNAMED_TYPE_TEMPLATE_ARGS)
    
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(CMakePushCheckState)

check_cxx_source_compiles ("
#include <cstdlib>
static void foo(void) __attribute__ ((unused));
int main(void) { return 0; }
" HAVE___ATTRIBUTE__)

check_cxx_source_compiles ("
#include <cstdlib>
static void foo(void) __attribute__ ((visibility(\"default\")));
int main(void) { return 0; }
" HAVE___ATTRIBUTE__VISIBILITY_DEFAULT)

check_cxx_source_compiles ("
#include <cstdlib>
static void foo(void) __attribute__ ((visibility(\"hidden\")));
int main(void) { return 0; }
" HAVE___ATTRIBUTE__VISIBILITY_HIDDEN)

check_cxx_source_compiles ("
int main(void) { if (__builtin_expect(0, 0)) return 1; return 0; }
" HAVE___BUILTIN_EXPECT)

check_cxx_source_compiles ("
int main(void)
{
  int a; if (__sync_val_compare_and_swap(&a, 0, 1)) return 1; return 0;
}
" HAVE___SYNC_VAL_COMPARE_AND_SWAP)

cmake_push_check_state (RESET)
check_cxx_source_compiles ("
#define _XOPEN_SOURCE 500
#include <pthread.h>
int main(void)
{
  pthread_rwlock_t l;
  pthread_rwlock_init(&l, NULL);
  pthread_rwlock_rdlock(&l);
  return 0;
}
" HAVE_RWLOCK)
cmake_pop_check_state ()

check_cxx_source_compiles ("
__declspec(selectany) int a;
int main(void) { return 0; }
" HAVE___DECLSPEC)

check_cxx_source_compiles ("
#include <vector>
vector<int> t; int main() { }
" STL_NO_NAMESPACE)

check_cxx_source_compiles ("
#include <vector>
std::vector<int> t; int main() { }
" STL_STD_NAMESPACE)

check_cxx_source_compiles ("
#include <iostream>
std::ostream& operator<<(std::ostream&, struct s);
using ::operator<<;
int main() { }
" HAVE_USING_OPERATOR)

check_cxx_source_compiles ("
namespace Outer { namespace Inner { int i = 0; }}
using namespace Outer::Inner;;
int main() { return i; }
" HAVE_NAMESPACES)

check_cxx_source_compiles ("
__thread int tls;
int main() { }
" HAVE_GCC_TLS)

check_cxx_source_compiles ("
__declspec(thread) int tls;
int main() { }
" HAVE_MSVC_TLS)

check_cxx_source_compiles ("
thread_local int tls;
int main() { }
" HAVE_CXX20_TLS)

check_cxx_source_compiles ("
#include <type_traits>
std::aligned_storage<sizeof(char), alignof(char)>::type data;
int main() { }
" HAVE_ALIGNED_STORAGE)

check_cxx_source_compiles ("
#include <atomic>
std::atomic<int> i;
int main() { }
" HAVE_CXX20_ATOMIC)

check_cxx_source_compiles ("
constexpr int x = 0;
int main() { }
" HAVE_CXX20_CONSTEXPR)

check_cxx_source_compiles ("
#include <chrono>
std::chrono::seconds s;
int main() { }
" HAVE_CXX20_CHRONO)

check_cxx_source_compiles ("
#include <cstddef>
void foo(std::nullptr_t) {}
int main(void) { foo(nullptr); }
" HAVE_CXX20_NULLPTR_T)
