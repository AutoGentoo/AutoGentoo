#include <iostream>

typedef struct PackageProperties
{
	bool _new;
	bool _slot;
	bool _updating;
	bool _downgrading;
	bool _reinstall;
	bool _replacing;
	bool _fetchbool_man;
	bool _fetchbool_auto;
	bool _interactive;
	bool _blockedbool_man;
	bool _blockedbool_auto;
}PackageProperties;


class Package
{
	public:
	Package()
	{
		;
	}
};
