Import("env")

env.Append(
	LINKFLAGS=["-Wl,-u,vfprintf,-lprintf_flt"]
)

# print(env.Dump())