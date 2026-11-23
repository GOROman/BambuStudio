#pragma once

#include "bambu_networking.hpp"
#include "nlohmann/json.hpp"
#include <limits>
#include <stdexcept>

namespace Slic3r {

struct HeadlessPrintJob {
    BBL::PrintParams params{};
    std::string country_code;
    bool confirm = false;
};

inline HeadlessPrintJob parse_headless_print_job(const nlohmann::json& manifest)
{
    HeadlessPrintJob job;
    auto& params = job.params;
    params.dev_id = manifest.at("device_id").get<std::string>();
    params.dst_file = manifest.at("printer_file").get<std::string>();
    const auto& plate = manifest.at("plate");
    if (!plate.is_number_integer() || plate < 1 || plate > std::numeric_limits<int>::max())
        throw std::runtime_error("Plate must be a positive integer.");
    params.plate_index = plate.get<int>();
    params.task_name = manifest.at("job_name").get<std::string>();
    job.country_code = manifest.at("country_code").get<std::string>();
    job.confirm = manifest.value("confirm", false);
    if (params.dev_id.empty() || params.dst_file.empty() || params.task_name.empty() || job.country_code.empty())
        throw std::runtime_error("A device ID, printer path, job name and country code are required.");

    const auto& mapping = manifest.at("ams_mapping");
    const auto& mapping2 = manifest.at("ams_mapping2");
    if (!mapping.is_array() || !mapping2.is_array() || mapping.size() != mapping2.size() ||
        (manifest.at("use_ams").get<bool>() && mapping.empty()))
        throw std::runtime_error("The AMS mapping arrays must have matching lengths.");
    for (size_t i = 0; i < mapping.size(); ++i) {
        if (!mapping[i].is_number_integer() || !mapping2[i].is_object() ||
            !mapping2[i].at("ams_id").is_number_integer() || !mapping2[i].at("slot_id").is_number_integer())
            throw std::runtime_error("Invalid AMS mapping entry.");
    }
    params.task_use_ams = manifest.at("use_ams").get<bool>();
    params.ams_mapping = mapping.dump();
    params.ams_mapping2 = mapping2.dump();
    if (manifest.contains("nozzle_mapping")) {
        if (!manifest.at("nozzle_mapping").is_array())
            throw std::runtime_error("Nozzle mapping must be an array.");
        params.nozzle_mapping = manifest.at("nozzle_mapping").dump();
    }

    // Match the existing PrintJob route for a model selected in Device > Files.
    if (params.dst_file.front() == '/')
        params.dst_file.erase(0, 1);
    if (params.dst_file.empty())
        throw std::runtime_error("A file on the printer is required.");
    params.project_name = params.task_name;
    params.print_type = "from_sdcard_view";
    params.connection_type = "cloud";
    params.use_ssl_for_ftp = true;
    params.use_ssl_for_mqtt = true;
    params.task_bed_leveling = true;
    params.auto_bed_leveling = 1;
    return job;
}

} // namespace Slic3r
