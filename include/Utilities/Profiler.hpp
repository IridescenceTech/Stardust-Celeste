#pragma once
#include "Singleton.hpp"
#include "Timer.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

namespace Stardust_Celeste::Utilities {

class Profiler : public Singleton {
  public:
    void begin_session(const std::string &name,
                       const std::string &filepath = "results.json") {
        m_FsOut.open(filepath);

        start = true;
        if (m_FsOut.is_open()) {
            m_FsOut << "{\"events\":[\n";
            m_FsOut.flush();
        }
    }

    void begin_profile(const std::string &name, int line, const char *file) {
        profName = name;
        ln = line;
        sfile = file;
        t.reset();
    }

    void end_profile() {

        auto dt = t.get_delta_time();

        std::stringstream json;

        json << std::setprecision(6) << std::fixed;
        if (start) {
            start = false;
        } else {
            json << ",";
        }
        json << "{";
        json << R"("cat":"function",)";
        json << R"("duration":)" << (dt) << ',';
        json << R"("name":")" << profName << "\",";
        json << R"("line":)" << ln << ',';
        json << R"("file":")" << sfile << "\"";
        json << "}\n";
        m_FsOut << json.str();
        m_FsOut.flush();
    }

    void end_session() {
        m_FsOut << "]}";
        m_FsOut.close();
    }

    static Profiler &get() {
        static Profiler instance;
        return instance;
    }

  private:
    Utilities::Timer t;
    std::ofstream m_FsOut;
    std::string profName;
    int ln{};
    std::string sfile;
    bool start = false;
};
} // namespace Stardust_Celeste::Utilities

#define SC_PROFILE_BEGIN_SESSION(name, filepath)                               \
    ::Stardust_Celeste::Utilities::Profiler::get().begin_session(name, filepath)
#define SC_PROFILE_END_SESSION()                                               \
    ::Stardust_Celeste::Utilities::Profiler::get().end_session()
#define SC_PROFILE_FUNCTION(fname, line, file, ...)                            \
    ::Stardust_Celeste::Utilities::Profiler::get().begin_profile(#fname, line, \
                                                                 file);        \
    fname(__VA_ARGS__);                                                        \
    ::Stardust_Celeste::Utilities::Profiler::get().end_profile();
