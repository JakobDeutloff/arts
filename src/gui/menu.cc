#include "menu.h"

#include <debug.h>
#include <imgui.h>
#include <logic.h>

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>

#include "jacobian.h"
#include "math_funcs.h"

namespace ARTSGUI::MainMenu {
void fullscreen(Config& cfg, GLFWwindow* window) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem(" Fullscreen ", "F11")) {
        if (not cfg.fullscreen) {
          glfwGetWindowSize(window, &cfg.width, &cfg.height);
          glfwGetWindowPos(window, &cfg.xpos, &cfg.ypos);
          auto* monitor = get_current_monitor(window);
          const auto* mode = glfwGetVideoMode(monitor);
          glfwSetWindowMonitor(
              window, monitor, 0, 0, mode->width, mode->height, 0);
        } else
          glfwSetWindowMonitor(
              window, nullptr, cfg.xpos, cfg.ypos, cfg.width, cfg.height, 0);

        cfg.fullscreen = not cfg.fullscreen;
      }
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (ImGui::IsKeyPressed(GLFW_KEY_F11) or
      (cfg.fullscreen and ImGui::IsKeyPressed(GLFW_KEY_ESCAPE))) {
    if (not cfg.fullscreen) {
      glfwGetWindowSize(window, &cfg.width, &cfg.height);
      glfwGetWindowPos(window, &cfg.xpos, &cfg.ypos);
      glfwGetWindowPos(window, &cfg.xpos, &cfg.ypos);
      auto* monitor = get_current_monitor(window);
      const auto* mode = glfwGetVideoMode(monitor);
      glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0);
    } else
      glfwSetWindowMonitor(
          window, nullptr, cfg.xpos, cfg.ypos, cfg.width, cfg.height, 0);
    cfg.fullscreen = not cfg.fullscreen;
  }
}

void quitscreen(const Config& cfg, GLFWwindow* window) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem(" Quit ", "Ctrl+X"))
        glfwSetWindowShouldClose(window, 1);
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (cfg.io.KeyCtrl and ImGui::IsKeyPressed(GLFW_KEY_X)) {
    glfwSetWindowShouldClose(window, 1);
  }
}

void exportdata(const Config& cfg, ImGui::FileBrowser& fileBrowser) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem(" Export Data ", "Ctrl+S")) {
        fileBrowser.Open();
      }
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (cfg.io.KeyCtrl and ImGui::IsKeyPressed(GLFW_KEY_S)) {
    fileBrowser.Open();
  }
}

bool change_item(const char* name) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Value")) {
      if (ImGui::BeginMenu(name, false)) {
        ImGui::Separator();
        ImGui::EndMenu();
      }
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  return false;
}

bool change_item(const char* name, ArrayOfRetrievalQuantity& jac) {
  bool did_something = false;

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Value")) {
      auto tpred = [](auto& j) { return j == Jacobian::Atm::Temperature; };
      auto fpred = [](auto& j) { return j == Jacobian::Atm::WindMagnitude; };

      if (ImGui::BeginMenu(name)) {
        if (bool has = std::any_of(jac.begin(), jac.end(), tpred);
            ImGui::Selectable(
                "\tTemperature\t", has, ImGuiSelectableFlags_DontClosePopups)) {
          if (has) {
            std::remove_if(jac.begin(), jac.end(), tpred);
            jac.pop_back();
          } else {
            auto& x = jac.emplace_back();
            x.Target() = Jacobian::Target(Jacobian::Atm::Temperature);
            x.Target().perturbation = 0.1;
          }
          did_something = true;
        }
        ImGui::Separator();

        if (bool has = std::any_of(jac.begin(), jac.end(), fpred);
            ImGui::Selectable("\tWind Magnitude\t",
                              has,
                              ImGuiSelectableFlags_DontClosePopups)) {
          if (has) {
            std::remove_if(jac.begin(), jac.end(), fpred);
            jac.pop_back();
          } else {
            auto& x = jac.emplace_back();
            x.Target() = Jacobian::Target(Jacobian::Atm::WindMagnitude);
            x.Target().perturbation = 100;
          }
          did_something = true;
        }
        ImGui::Separator();
        ImGui::EndMenu();
      }
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  return did_something;
}

bool change_item(const char* name, Vector& vec, const ArrayOfString& keys) {
  const Index n = vec.nelem();
  ARTS_ASSERT(n == keys.nelem())
  bool did_something = false;

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Value")) {
      if (ImGui::BeginMenu(name)) {
        for (Index i = 0; i < n; i++) {
          ImGui::Text("\t");
          ImGui::SameLine();
          if (ImGui::InputDouble(keys[i].c_str(), &vec[i], 0, 0, "%g"))
            did_something = true;
          ImGui::Separator();
        }
        ImGui::EndMenu();
      }
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  return did_something;
}

bool change_item(const char* name,
                 Vector& vec,
                 const ArrayOfArrayOfSpeciesTag& spec,
                 Options& menu) {
  ARTS_ASSERT(vec.nelem() == spec.nelem())
  bool did_something = false;

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Value")) {
      if (ImGui::BeginMenu(name)) {
        if (ImGui::BeginMenu("\tSelect VMR type\t")) {
          for (auto& x : enumtyps::VMRTypes) {
            if (ImGui::Selectable(
                    (" " + std::string{toString(x)} + " ").c_str(),
                    x == menu.vmr,
                    ImGuiSelectableFlags_DontClosePopups))
              menu.vmr = x;
          }
          ImGui::EndMenu();
        }
        ImGui::Separator();

        const std::string vmr_type{toString(menu.vmr)};
        constexpr Numeric max = 1.5;
        const Numeric scale = [](VMR& vmr) {
          switch (vmr) {
            case VMR::exact:
              return 1.0;
            case VMR::percent:
              return 100.0;
            case VMR::ppmv:
              return 1'000'000.0;
            case VMR::FINAL: { /* leave last */
            }
          }
          return 0.0;
        }(menu.vmr);
        ImGui::Text("\tVMR (range: [0, %g]; type: %s):\t",
                    scale * max,
                    vmr_type.c_str());

        for (Index i = 0; i < vec.nelem(); i++) {
          const std::string spec_name{var_string('\t', spec[i], '\t')};
          Numeric val = scale * vec[i];
          ImGui::Text("\t");
          ImGui::SameLine();
          if (ImGui::InputDouble(spec_name.c_str(), &val, 0, 0, "%g")) {
            val /= scale;
            vec[i] = std::clamp(val, 0.0, max);
            did_something = true;
          }
          ImGui::Separator();
        }

        ImGui::Separator();
        ImGui::EndMenu();
      }
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  return did_something;
}

bool change_item(const char* name, Vector& vec, Numeric min, Numeric max) {
  Index n = vec.nelem();
  ARTS_ASSERT(is_sorted(vec))
  ARTS_ASSERT(min < max)
  ARTS_ASSERT(n > 1)
  bool did_something = false;

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Value")) {
      if (ImGui::BeginMenu(name)) {
        Numeric start = vec[0];
        Numeric stop = vec[n - 1];

        bool change = false;
        ImGui::Text("\t");
        ImGui::SameLine();
        if (ImGui::InputDouble("\tStart\t", &start, 0, 0, "%g")) {
          start = std::clamp(start, min, stop);
          change = true;
        }
        ImGui::Text("\t");
        ImGui::SameLine();
        if (ImGui::InputDouble("\tStop\t", &stop, 0, 0, "%g")) {
          stop = std::clamp(stop, start, max);
          change = true;
        }
        ImGui::Text("\t");
        ImGui::SameLine();
        if (ImGui::InputScalar("\tnelem\t", ImGuiDataType_S64, &n)) {
          n = std::clamp(n, Index{2}, std::numeric_limits<Index>::max());
          change = true;
        }

        if (change) {
          nlinspace(vec, start, stop, n);
          did_something = true;
        }

        ImGui::Separator();
        ImGui::EndMenu();
      }
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  return did_something;
}

bool change_item(const char* name,
                 const char* value_name,
                 Numeric& val,
                 Numeric min,
                 Numeric max) {
  bool did_something = false;

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Value")) {
      if (ImGui::BeginMenu(name)) {
        Numeric v = val;
        ImGui::Text("\t");
        ImGui::SameLine();
        if (ImGui::InputDouble(value_name, &v, 0, 0, "%g")) {
          val = std::clamp(v, min, max);
          did_something = true;
        }
        ImGui::Separator();
        ImGui::EndMenu();
      }
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  return did_something;
}

bool change_item(const char* name,
                 ArrayOfSpeciesTag& out,
                 const ArrayOfArrayOfSpeciesTag& keys) {
  bool did_something = false;

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Value")) {
      if (ImGui::BeginMenu(name)) {
        if (ImGui::Selectable(" *All* ",
                              out.nelem() == 0,
                              ImGuiSelectableFlags_DontClosePopups)) {
          out.resize(0);
          did_something = true;
        }
        for (auto& key : keys) {
          ImGui::Separator();
          const std::string str{var_string(' ', key, ' ')};
          if (ImGui::Selectable(str.c_str(),
                                key == out,
                                ImGuiSelectableFlags_DontClosePopups)) {
            out = key;
            did_something = true;
          }
        }
        ImGui::EndMenu();
      }
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  return did_something;
}

void select_option(Index& ind, const ArrayOfRetrievalQuantity& jac) {
  if (ImGui::Selectable(
          "\tMain\t", ind == -1, ImGuiSelectableFlags_DontClosePopups)) {
    ind = -1;
  }

  for (Index i = 0; i < jac.nelem(); i++) {
    std::string opt{'\t'};

    switch (jac[i].Target().type) {
      case Jacobian::Type::Atm:
        opt += var_string(jac[i].Target().atm);
        break;
      case Jacobian::Type::Line:
        opt += var_string(jac[i].Target().line, ' ', jac[i].Target().qid);
        break;
      case Jacobian::Type::Sensor:
      case Jacobian::Type::Special:
        ARTS_USER_ERROR("Not implemented")
      case Jacobian::Type::FINAL: { /* leave last */
      }
    }

    opt += '\t';
    if (ImGui::Selectable(
            opt.c_str(), ind == i, ImGuiSelectableFlags_DontClosePopups)) {
      ind = i;
    }
  }
}
}  // namespace ARTSGUI::MainMenu