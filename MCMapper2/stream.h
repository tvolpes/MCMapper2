#pragma once

#include <boost\filesystem.hpp>
#include <boost\filesystem\fstream.hpp>
#include <boost\iostreams\filtering_stream.hpp>
#pragma warning( disable:4244 )
#include <boost\iostreams\filter\gzip.hpp>
#pragma warning( default:4244 )

typedef boost::iostreams::filtering_istream InputStream;