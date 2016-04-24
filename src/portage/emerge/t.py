a = open("worldraw", "r").readlines()
b = []
for l in a:
	x = len("[ebuild   R")
	if l[0:x] != "[ebuild   R" and l != "\n":
		b.append ( l )

c = open ("world_edit", "w+")
for x in b:
	c.write ( "%s\n" % x )
