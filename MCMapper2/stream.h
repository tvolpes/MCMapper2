/*
MCMapper2 Minecraft Mapping Utility
Copyright( C ) 2015 Timothy Volpe

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
( at your option ) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.

*/

#pragma once

#include <boost\filesystem.hpp>
#include <boost\filesystem\fstream.hpp>
#include <boost\iostreams\filtering_stream.hpp>
#pragma warning( disable:4244 )
#include <boost\iostreams\filter\gzip.hpp>
#pragma warning( default:4244 )

typedef boost::iostreams::filtering_istream InputStream;