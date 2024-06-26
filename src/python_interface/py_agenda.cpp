#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/variant.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/function.h>
#include <parameters.h>
#include <workspace.h>

#include <algorithm>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "debug.h"
#include "hpy_arts.h"
#include "hpy_vector.h"
#include "python_interface.h"

extern Parameters parameters;

namespace Python {
Index create_workspace_gin_default_internal(Workspace& ws, const String& key);

std::filesystem::path correct_include_path(
    const std::filesystem::path& path_copy) {
  std::filesystem::path path = path_copy;
  for (auto& prefix : parameters.includepath) {
    if (std::filesystem::is_regular_file(
            path = std::filesystem::path(prefix.c_str()) / path_copy))
      break;
  }

  ARTS_USER_ERROR_IF(not std::filesystem::is_regular_file(path),
                     "Cannot find file: ",
                     path_copy,
                     '\n',
                     "Search path: ",
                     parameters.includepath)

  // Must add the direcory to include paths as controlfiles know where they are
  std::filesystem::path dir_path = path;
  dir_path.remove_filename();
  String new_inc = dir_path.string();
  if (std::find(parameters.includepath.begin(),
                parameters.includepath.end(),
                new_inc) == parameters.includepath.end())
    parameters.includepath.emplace_back(new_inc);

  return path;
}

void py_agenda(py::module_& m) try {
  py::class_<CallbackOperator> cbd(m, "CallbackOperator");
  workspace_group_interface(cbd);
  cbd.def(
         "__init__",
         [](CallbackOperator* cb,
            const std::function<void(const std::shared_ptr<Workspace>&)>& f,
            const std::vector<std::string>& i,
            const std::vector<std::string>& o) {
           new (cb) CallbackOperator(f, i, o);
         },
         py::arg("f"),
         py::arg("inputs")  = std::vector<std::string>{},
         py::arg("outputs") = std::vector<std::string>{},
         "Initialize as structured call")
      .def("__call__", [](CallbackOperator& f, Workspace& ws) { f(ws); });

  py::class_<Method>(m, "Method")
      .def(
          "__init__",
          [](Method* me,
             const std::string& n,
             const std::vector<std::string>& a,
             const std::unordered_map<std::string, std::string>& kw) {
            new (me) Method{n, a, kw};
          },
          py::arg("name"),
          py::arg("args"),
          py::arg("kwargs"),
          "A named method with args and kwargs")
      .def(
          "__init__",
          [](Method* me, const std::string& n, const PyWSV& v) {
            new (me) Method{n,
                            std::visit([](auto a) { return Wsv(std::move(a)); },
                                       from_py(v).value)};
          },
          py::arg("name"),
          py::arg("wsv"),
          "A method that sets a workspace variable")
      .def_prop_ro(
          "val",
          [](const Method& method) -> py::object {
            const auto& x = method.get_setval();
            if (x) return py::cast(x.value());
            return py::none();
          },
          "The value (if any) of a set method")
      .def_prop_ro(
          "name",
          [](const Method& method) { return method.get_name(); },
          "The name of the method")
      .def("__str__", [](const Method& method) { return var_string(method); })
      .doc() = "The method class of ARTS";

  py::class_<Agenda> ag(m, "Agenda");
  workspace_group_interface(ag);
  ag.def(py::init<std::string>(), py::arg("name"), "Create with name")
      .def("add",
           &Agenda::add,
           py::arg("method").none(false),
           R"--(
Adds a method to the Agenda

All workspace variables are defaulted, and all GIN with defaults
create anonymous workspace variables.  All input that are not 
workspace variables are added to the workspace

The input order takes priority over the named argument order,
so Copy(a, out=b) will not even see the b variable.
)--")
      .def(
          "execute",
          [](Agenda& a, Workspace& ws) { a.execute(ws); },
          py::arg("ws"),
          "Executes the agenda on the provided workspace")
      .def(
          "finalize",
          [](Agenda& a, bool fix) { a.finalize(fix); },
          py::arg("fix") = false,
          "Finalize the agenda, making it possible to use it in the workspace")
      .def_prop_ro(
          "name",
          [](const Agenda& agenda) { return agenda.get_name(); },
          "The name of the agenda")
      .def_prop_ro(
          "methods",
          [](const Agenda& agenda) { return agenda.get_methods(); },
          "The methods of the agenda");

  auto aag = py::bind_vector<ArrayOfAgenda, py::rv_policy::reference_internal>(
      m, "ArrayOfAgenda");
  workspace_group_interface(aag);
  aag.def(
         "__init__",
         [](ArrayOfAgenda* a, std::vector<Agenda> va) {
           for (auto& ag_ : va) {
             ARTS_USER_ERROR_IF(
                 ag_.get_name() not_eq va.front().get_name(),
                 "An ArrayOfAgenda must only consist of agendas with the same name\n"
                 "You have input a list of agendas that contains disimilar names.\n"
                 "\nThe first item is named: \"",
                 va.front().get_name(),
                 '"',
                 '\n',
                 "A later item in the list is names: \"",
                 ag_.get_name(),
                 '"',
                 '\n')
           }
           new (a) ArrayOfAgenda(std::move(va));
         },
         "Create from :class:`list`")
      .def(
          "finalize",
          [](ArrayOfAgenda& aa) {
            for (auto& a : aa) a.finalize();
          },
          "Checks if the agenda works")
      .def_prop_rw(
          "name",
          [](ArrayOfAgenda& a) -> String {
            if (a.size() == 0) return "";
            return a.front().get_name();
          },
          [](ArrayOfAgenda& aa, const String& name) {
            for (auto& a : aa) a.set_name(name);
          },
          ":class:`~pyarts.arts.String` Name of the array of agenda");
} catch (std::exception& e) {
  throw std::runtime_error(
      var_string("DEV ERROR:\nCannot initialize agendas\n", e.what()));
}
}  // namespace Python
