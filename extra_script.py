# Import("env")

# my_flags = env.ParseFlags(env['BUILD_FLAGS'])
# print(my_flags.get("CPPDEFINES"))

# env.Replace(PROGNAME="firmware_%s" % build_tag)

Import("env")

version_from_buildflags = "not_found"
my_flags = env.ParseFlags(env['BUILD_FLAGS'])
# some flags are just defines without value
# some are key, value pairs
for x in my_flags.get("CPPDEFINES"):
#   print(x)
    if isinstance(x, list):
        # grab as key, value
        k, v = x
        if k == "VERSION":
            version_from_buildflags = v
            build_number = "2010290"
            # no need to iterate further
            break

env.Replace(PROGNAME="firmware_v%s" % (version_from_buildflags))