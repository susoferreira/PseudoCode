project('code-preprocessor','c')
incdir = include_directories(['../include/generator/','../include/preprocessor'])
sds_sources = ['sds/sds.h','sds/sds.c']
generator_sources =['generator/generator.c','generator/generators.c']
preprocessor_sources = ['preprocessor/preprocessor.c']

executable('generator',sds_sources,generator_sources,preprocessor_sources,include_directories : incdir)
