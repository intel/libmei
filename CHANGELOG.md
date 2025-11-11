## [1.8.0]

### Changed
 - account for driver disabled state

### Fixed
 -  make kind null-terminated

## [1.7.0]

Note: ABI only binary incompatibility
      struct mei size increased.

### Added
 - add initialization api with new logger callback

## [1.6.4]

### Added
 - add device kind api

## [1.6.3]

### Fixed
 -  mark __dump_buffer as debug-only
 -  cast fwsts_num to off_t to suppress warning

## [1.6.2]

### Changed
 - dump traffic only in debug builds

## [1.6.1]

### Fixed
 - fix variable type for readlink output

## [1.6.0]

Note: backward-incompatible API breakage:
      mei_set_log_callback now returns int instead of uint32_t.

### Fixed
 - convert size to required type
 - rewrite dump_hex_buffer

### Added
 - CMake: enable several extended compilation warnings

### Changed
 - change mei_set_log_callback return type

## [1.5.6]

### Added
 - add api to get trc value

## [1.5.5]

### Added
 - add log callback

## [1.5.4]

### Fixed
 - Allow connect in error state

### Added
 - README: explain build steps
 - set security policy for the project

## [1.5.3]

### Added
 - add log level api

## [1.5.2]

### Fixed
 - terminate readlink output
 - expand error handling of readlink and snprintf
 - print size_t with unsigned format

## [1.5.1]

### Fixed
 - allow to reconnect if client previously not found

## [1.5.0]

### Added
 - Add API to set vtag in connection call.
 - CMake: add secure compile flags.

## [1.4]

### Added
 - Add option to log to syslog (use USE_SYSLOG parameter in CMake).

### Changed

### Fixed
 - Cleanup code.
 - Use symbolic constants to improve code readability.
 - Use O_CLOEXEC when opening device file.

## [1.3]

### Added

 - Add configuration files for gitlint, cmake-format.
 - Add internal checkpatch.pl from the Linux kernel.
 - Add API to utilize already open file handle.

### Changed

1. CMake cleanups.

### Fixed

1. Account to strdup failure in init.
2. Fix a typo in enum.
3. dump_hex_buffer() remove variable length array.

## [1.2]

### Added

 - Add mei_fwstatus API to obtain FW status.
 - Add static library build.
 - Add compiler security hardening flags.

### Changed
 - Set default device to mei0.

### Fixed

## [1.0]

### Added

### Changed

### Fixed
