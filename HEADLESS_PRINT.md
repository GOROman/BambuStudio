# Headless printing from printer storage

`--headless-print job.json` submits a print request for a file already on the printer. It calls the same networking plugin function as **Device > Files > Print**. The command requires a Studio build with the networking plugin loader, a compatible installed plugin, and a Bambu account session the plugin can restore. The plugin and printer must accept the application's authorization.

Use the printer's stored path and select the plate and filament mapping for that file. The following mapping is an example for the first slot of AMS 3H on an H3C.

```json
{
  "device_id": "PRINTER_SERIAL",
  "printer_file": "/userdata/model/history/plate-1.gcode.3mf",
  "plate": 1,
  "job_name": "plate-1",
  "country_code": "JP",
  "use_ams": true,
  "ams_mapping": [28],
  "ams_mapping2": [{"ams_id": 32, "slot_id": 0}],
  "confirm": false
}
```

The optional `nozzle_mapping` array can pass through the job's explicit nozzle assignments. Use the country code for your Bambu account and the printer path as reported by the file browser.

```sh
"/path/to/BambuStudio.app/Contents/MacOS/BambuStudio" \
  --datadir "$HOME/Library/Application Support/BambuStudio" \
  --headless-print job.json
```

With `confirm` unset or false, the command validates the manifest without contacting the printer. After checking the build plate, material, and nozzle, set `confirm` to true to submit the print request. A successful exit indicates plugin acceptance. Check the printer to verify that printing has started.
