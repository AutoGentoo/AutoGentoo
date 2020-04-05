from client import Host
from script import *
import stage3


def validate_chroot(_job_name: str, host: Host):
	if stat("/.stage3") == 0:
		res = stage3.script(_job_name, host)
		if res != 0:
			return res
	
	return 0
