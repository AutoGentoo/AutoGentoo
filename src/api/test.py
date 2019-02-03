import d_malloc
import traceback
import sys

k = d_malloc.DynamicBuffer(to_network=True)
k.append("ssisa(is)", ["hello", "world", 2, "goodbye", [(2, "ds"), (3, "dd"), (1, "36")]])

while True:
	try:
		print(eval(input("> ")))
	except KeyboardInterrupt:
		print("")
	except EOFError:
		print("^D")
		break
	except (NameError, SyntaxError, RuntimeError):
		traceback.print_exc(file=sys.stdout)
