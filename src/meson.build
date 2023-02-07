project('code-preprocessor','c','cpp')

incdir = include_directories(['../include/generator/','../include/preprocessor','../include/utils','../include/platform-specifics/','sds/'])


# todas las plataformas usan el mismo include (mismas funciones declaradas)
# pero distintas sources para las funciones (distinta implementación)
cli_src = ['platform-specifics/cli/logger.c','platform-specifics/cli/config.c','platform-specifics/cli/main_tui.c']


#distintas librerías que son utilizadas por todas las plataformas
utils_src =['utils/utils.c','utils/compile_code.c']
sds_src = ['sds/sds.c']
generator_src =['generator/generator.c','generator/generators.c']
preprocessor_src = ['preprocessor/preprocessor.c']

sources=[utils_src,sds_src,generator_src,preprocessor_src]

executable('generator_tui',cli_src,sources,include_directories : incdir,c_args: '-Duse_ansi_colors')

imgui_src = [
            '../imgui/imgui.cpp',
            '../imgui/imgui_demo.cpp',
            '../imgui/imgui_draw.cpp',
            '../imgui/imgui_tables.cpp',
            '../imgui/imgui_widgets.cpp'
            ]

gui_src=[
    'platform-specifics/gui/config.c',
    'platform-specifics/gui/logger.c',
    ]
gl=dependency('gl')
glfw_dep = subproject('glfw').get_variable('glfw_dep')
imgui = subproject('imgui').get_variable('imgui_dep')
executable('generator_gui','platform-specifics/gui/gui_opengl.cpp',
        'platform-specifics/gui/ImGuiColorTextEdit/TextEditor.cpp',
        gui_src,sources,sds_src,
        include_directories : [incdir], # hay que incluir sds
        dependencies : [glfw_dep,imgui,gl])
