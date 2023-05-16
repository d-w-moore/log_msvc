/// \file

#include "irods/irods_error.hpp"
#include "irods/irods_logger.hpp"
#include "irods/irods_ms_plugin.hpp"
#include "irods/irods_re_structs.hpp"
#include "irods/msParam.h"
#include "irods/rodsErrorTable.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <iterator>

namespace
{
    auto msi_log(MsParam* _log_level, MsParam* _log_text, ruleExecInfo_t* _rei) -> int
    {
        using log_msi = irods::experimental::log::microservice;
        using log_legacy = irods::experimental::log::legacy;

        // Check input parameters.
        if (!_log_level || !_log_text || !_rei) { // NOLINT(readability-implicit-bool-conversion)
            log_msi::error("At least one input argument is null.");
            return SYS_INTERNAL_NULL_INPUT_ERR;
        }

        const auto* log_level_cstring = parseMspForStr(_log_level);
        const auto* log_text = parseMspForStr(_log_text);

        if (!log_text || !log_level_cstring) { // NOLINT(readability-implicit-bool-conversion)
            log_msi::error("Could not parse microservice parameter into a string.");
            return SYS_INVALID_INPUT_PARAM;
        }

        std::string log_level {log_level_cstring};
        std::transform(log_level.begin(), log_level.end(), log_level.begin(), ::toupper);

        if (log_level == "TRACE") { log_legacy::trace("{}",log_text); }
        else if (log_level == "DEBUG")  { log_legacy::debug("{}",log_text); }
        else if (log_level == "INFO")  { log_legacy::info("{}",log_text); }
        else if (log_level == "WARN")  { log_legacy::warn("{}",log_text); }
        else if (log_level == "ERROR")  { log_legacy::error("{}",log_text); }
        else if (log_level == "CRITICAL")  { log_legacy::critical("{}",log_text); }
        else {
            log_msi::error("Invalid log level was specified: {}", log_level);
            return SYS_INVALID_INPUT_PARAM;
        }
        return 0;
    } // msi_impl

    template <typename... Args, typename Function>
    auto make_msi(const std::string& name, Function func) -> irods::ms_table_entry*
    {
        auto* msi = new irods::ms_table_entry{sizeof...(Args)}; // NOLINT(cppcoreguidelines-owning-memory) 
        msi->add_operation(name, std::function<int(Args..., ruleExecInfo_t*)>(func));
        return msi;
    } // make_msi
} // anonymous namespace

extern "C"
auto plugin_factory() -> irods::ms_table_entry*
{
    return make_msi<MsParam*, MsParam*>("msi_log", msi_log);
} // plugin_factory

#ifdef IRODS_FOR_DOXYGEN
/// Outputs a string to the system logs using the syslog "legacy" category.
///
/// \param[in]     _log_level  A string indicating one of the available log levels, eg. "INFO".
/// \param[in]     _log_text   The string to be written to the log.
/// \param[in,out] _rei    A ::RuleExecInfo object that is automatically handled by the
///                        rule engine plugin framework. Users must ignore this parameter.
///
/// \return An integer.
/// \retval 0        On success.
/// \retval Non-zero On failure.
///
/// \since X.Y.Z
int msi_rotate_string(MsParam* _input, MsParam* _output, ruleExecInfo_t* _rei);
#endif // IRODS_FOR_DOXYGEN
