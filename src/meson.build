project('code-preprocessor','c')
incdir = include_directories(['../include/generator/','../include/preprocessor','../include/utils','../include/platform-specifics/'])

# todas las plataformas usan el mismo include(mismas funciones definidas)
# pero distintas sources para las funciones (distinta implementación)
cli_src=['platform-specifics/cli/logger.c','platform-specifics/cli/config.c']

utils_src =['utils/utils.c']
sds_src = ['sds/sds.h','sds/sds.c']
generator_src =['generator/generator.c','generator/generators.c']
preprocessor_src = ['preprocessor/preprocessor.c']




executable('generator_cli',sds_src,generator_src,preprocessor_src,utils_src,cli_src,include_directories : incdir)
