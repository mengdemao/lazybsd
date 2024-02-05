#include <pybind11/pybind11.h>
#include <lazybsd.h>
#include <lazybsd_version.h>

auto version(void) {
    return lazybsd::version::lazybsd_version_string();
}

PYBIND11_MODULE(lazybsd, m) {
    m.doc() = "lazybsd python plugin"; // optional module docstring
    m.def("version", &version, "A function which print lazybsd version");
}