#pragma once

class CMCMapper
{
private:

public:
	CMCMapper();

	bool initialize();
	void shutdown();

	bool run( int argc, char* argv[] );
};