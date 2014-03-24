#include <gtest/gtest.h>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  logging::add_file_log(
    keywords::file_name = "cpp_%N.log",                                        /*< file name pattern >*/
    keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
    keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
    keywords::format = "[%TimeStamp%]: %Message%"                                 /*< log record format >*/
  );

  logging::core::get()->set_filter(
    logging::trivial::severity >= logging::trivial::trace
  );

  logging::add_common_attributes();

  return RUN_ALL_TESTS();
}
 
 
