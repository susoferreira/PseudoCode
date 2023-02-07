#include "imgui_internal.h"
#include "ImGuiColorTextEdit/TextEditor.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <cstdlib>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <utility>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

extern "C" {

#include "sds/sds.h"
#include "generator.h"
#include "parser.h"
#include "logger-gui.h"
#include "config.h"
#include "utils.h"
#include "logger.h"
#include "compile_code.h"

}

// returns printf format string to execute command in a graphical terminal
// examples: cmd.exe/c '%s;echo ejecucion finalizada;pause' (in windows) xterm -e '%s;echo ejecución finalizada;read' 
// read and pause are probably redundant because generated programs already have LEER as last line 
//
const char* get_terminal_emulator(){
    #if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
        return "start cmd.exe /c \"%s & echo ejecucion finalizada & pause\"";
    #endif
    char* linux_terminals[] =
    {
        (char*) "gnome-terminal",
        (char*) "konsole",
        (char*) "alacritty",
        (char*) "xfce4-terminal",
        (char*) "xterm",
    };
    char* terminal_commands[] =
    {
        (char*) "gnome-terminal -- sh -c '%s;echo ejecución finalizada;read'",
        (char*) "konsole -e sh -c '%s;echo ejecución finalizada;read'",
        (char*) "alacritty -e sh -c '%s;echo ejecución finalizada;read'",
        (char*) "xfce4-terminal -e sh -c '%s;echo ejecución finalizada;read'",
        (char*) "xterm -e sh -c '%s;echo ejecución finalizada;read'",
  
    };
    sds command=sdsempty();
    for (int i =0; i< sizeof(linux_terminals)/sizeof(char*);i++) {
        sdsfree(command);
        command = sdscat(sdsnew("which "),linux_terminals[i]);
        if (system(command) != 0){
            //no existe
            continue;
        }
        return terminal_commands[i];
    }
    logger((char*)"no se encuentra ninguna terminal soportada\n",LOG_ERROR);
    return (char*)"ERROR: TERMINAL NO ENCONTRADA EN LA LISTA DE TERMINALES";
}


// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ zrequires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
static GLFWwindow* setup_glfw(int width, int height,GLFWerrorfun error_callback){
        // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return NULL;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(width , height, "Pseudocódigo", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    return window;
}

static void setup_imgui(GLFWwindow* window,const char* glsl_version){
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
}

static void put_logs(TextEditor& logEditor,TextEditor::ErrorMarkers editorErrorMarkers){
    logEditor.SetText("");
    struct log* logs = get_logs();
    int log_size = get_log_size();

    for(int i=0;i<log_size;i++){
        std::string  line = logger_names[logs[i].type];

        if(logs[i].has_range){


            for (int j=logs[i].range.range_start_line; j <= logs[i].range.range_end_line; j++) {
                editorErrorMarkers.insert(std::make_pair(j,logs[i].text));
            }

            line+=",Desde linea "+std::to_string(logs[i].range.range_start_line)+", pos "+std::to_string(logs[i].range.range_start_pos);
            line+=" Hasta linea "+std::to_string(logs[i].range.range_end_line)+", pos "+std::to_string(logs[i].range.range_end_pos)+" - ";
        }

    line+=logs[i].text;
    bool ro = logEditor.IsReadOnly();
    logEditor.SetReadOnly(false);
    logEditor.InsertText(line);
    logEditor.SetReadOnly(ro);
}
}

//takes pseudocode from editor and generates cpp into generated
static void generate_to_editor(TextEditor& editor,TextEditor::ErrorMarkers& editorErrorMarkers,TextEditor& logEditor, TextEditor& generated){
 
    free_logs();

    sds processed;
    sds src = sdsnew(editor.GetText().c_str());
    set_src(src); // sets source for global logger use
    editor.GetText().copy(src,sizeof(char));
    sds generated_program;
    struct owl_tree* tree = NULL;


    bool correct = generate_from_string(src,&processed,&generated_program,&tree);
    if (correct){
        //generated_program only gets set if there are no errors
        generated.SetText(generated_program);
        sdsfree(generated_program);
    }
    put_logs(logEditor,editorErrorMarkers);

    sdsfree(processed);
    sdsfree(src);
}

static void render_logs_editor(TextEditor& editor,const char* title){
    auto cpos = editor.GetCursorPosition();
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Edit"))

        {
            bool ro = editor.IsReadOnly();
            
                if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                    editor.SetReadOnly(ro);
            
            if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
                editor.Undo();
            if (ImGui::MenuItem("Redo", "Ctrl-Z", nullptr, !ro && editor.CanRedo()))
                editor.Redo();

            ImGui::Separator();

            if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
                editor.Copy();
            if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
                editor.Cut();
            if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
                editor.Delete();
            if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                editor.Paste();

            ImGui::Separator();

            if (ImGui::MenuItem("Select all", "Ctrl-A", nullptr))
                editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Dark palette"))
                editor.SetPalette(TextEditor::GetDarkPalette());
            if (ImGui::MenuItem("Light palette"))
                editor.SetPalette(TextEditor::GetLightPalette());
            if (ImGui::MenuItem("Retro blue palette"))
                editor.SetPalette(TextEditor::GetRetroBluePalette());
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
        editor.IsOverwrite() ? "Ovr" : "Ins",
        editor.CanUndo() ? "*" : " ",
        editor.GetLanguageDefinition().mName.c_str());
    editor.Render(title);
}

static void render_cppeditor(TextEditor& editor,TextEditor& logEditor, TextEditor::ErrorMarkers editorErrorMarkers,const char* title,TextEditor compErrors){
    auto cpos = editor.GetCursorPosition();
    if (ImGui::BeginMenuBar())
    {                
        if (ImGui::BeginMenu("Edit"))

        {
            bool ro = editor.IsReadOnly();
            
            if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                editor.SetReadOnly(ro);
            
            if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
                editor.Undo();
            if (ImGui::MenuItem("Redo", "Ctrl-Z", nullptr, !ro && editor.CanRedo()))
                editor.Redo();

            ImGui::Separator();

            if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
                editor.Copy();
            if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
                editor.Cut();
            if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
                editor.Delete();
            if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                editor.Paste();

            ImGui::Separator();

            if (ImGui::MenuItem("Select all", "Ctrl-A", nullptr))
                editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Dark palette"))
                editor.SetPalette(TextEditor::GetDarkPalette());
            if (ImGui::MenuItem("Light palette"))
                editor.SetPalette(TextEditor::GetLightPalette());
            if (ImGui::MenuItem("Retro blue palette"))
                editor.SetPalette(TextEditor::GetRetroBluePalette());
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();


    }

    ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
        editor.IsOverwrite() ? "Ovr" : "Ins",
        editor.CanUndo() ? "*" : " ",
        editor.GetLanguageDefinition().mName.c_str());

    ImGui::SameLine();


    if (ImGui::Button("Compilar código")){
        free_logs();

        std::string code_path(conf.output_path);
        code_path+="generator/out.cpp";
        std::ofstream outfile(code_path);
        outfile << editor.GetText();
        outfile.close();

        //TODO: make async ig but it takes only a second so not really needed
        compile((char*)"out.cpp");

        ImGui::OpenPopup("Compilación");
    }

    editor.Render(title);

    if(ImGui::BeginPopupModal("Compilación"))
    {

        ImGui::BeginPopup("Compilación");
        
        ImGui::Text("Intentando compilar... en linux debes tener instalado g++, en windows ejecuta 'descargar_compilador.ps1'");
        
        ImGui::SameLine();
        if (!can_execute()) ImGui::BeginDisabled(); 
        if(ImGui::Button("Ejecutar programa")){
            const char* command = get_terminal_emulator();
            char buf[strlen(command)+200];

            sds file = sdscat(sdsnew(get_exe_path()),"out.cpp");


            snprintf(buf,strlen(command)+200,command,file);

            FILE* exe = popen(buf,"r");
            printf("comando usado para ejecutar el programa: %s",buf);


            pclose(exe);
            
        }
        if (!can_execute()) ImGui::EndDisabled();

        ImGui::SameLine();
        if(ImGui::Button("Cerrar")){
            ImGui::CloseCurrentPopup();
        }

        ImGui::Separator();
        char* compiler = used_compiler();

        if (compiler){
            ImGui::Text("Compilador que se intentará usar: %s",compiler);
        }else{
            ImGui::Text("No se ha encontrado el compilador");
        }
        ImGui::Separator();



        //editorErrorMarkers are not really needed
        put_logs(compErrors,editorErrorMarkers);
        compErrors.Render("logs de compilación");




        ImGui::EndPopup();
    }

}

static void render_pseudo_editor(TextEditor& editor,const char* title,TextEditor::ErrorMarkers* errors, TextEditor* generated,TextEditor* log_editor ){
    auto cpos = editor.GetCursorPosition();
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Edit"))

        {
            bool ro = editor.IsReadOnly();
            
            if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                editor.SetReadOnly(ro);
        
            if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
                editor.Undo();
            if (ImGui::MenuItem("Redo", "Ctrl-Z", nullptr, !ro && editor.CanRedo()))
                editor.Redo();

            ImGui::Separator();

            if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
                editor.Copy();
            if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
                editor.Cut();
            if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
                editor.Delete();
            if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                editor.Paste();

            ImGui::Separator();

            if (ImGui::MenuItem("Select all", "Ctrl-A", nullptr))
                editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Dark palette"))
                editor.SetPalette(TextEditor::GetDarkPalette());
            if (ImGui::MenuItem("Light palette"))
                editor.SetPalette(TextEditor::GetLightPalette());
            if (ImGui::MenuItem("Retro blue palette"))
                editor.SetPalette(TextEditor::GetRetroBluePalette());
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
        editor.IsOverwrite() ? "Ovr" : "Ins",
        editor.CanUndo() ? "*" : " ",
        editor.GetLanguageDefinition().mName.c_str());

    ImGui::SameLine();
    if (ImGui::Button("Generar código")){
        generate_to_editor(editor,*errors,*log_editor,*generated);
    }


    editor.Render(title);
}

int main(int, char**)
{

    GLFWwindow* window = setup_glfw(1280, 720,glfw_error_callback);
    const char* glsl_version = "#version 130";
    
    setup_imgui(window,glsl_version);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);




    TextEditor cppeditor;
    auto lang = TextEditor::LanguageDefinition::CPlusPlus();
    cppeditor.SetLanguageDefinition(lang);
    cppeditor.SetReadOnly(true);



    TextEditor pseudoeditor;
    TextEditor::ErrorMarkers pseudo_errors;
    //pseudocode se creó a partir de la definición de C de TextEditor.cpp
    auto pseudo = TextEditor::LanguageDefinition::PseudoCode();
    pseudoeditor.SetLanguageDefinition(pseudo);
    pseudoeditor.SetText("//estructura basica\nALGORITMO <NOMBRE>\nVARIABLES\n\nINICIO\n\nFIN");
    
    TextEditor log_editor;
    //TODO: crear una definición propia para que marque errores en rojo...
    auto err = TextEditor::LanguageDefinition::CPlusPlus();
    log_editor.SetLanguageDefinition(err);
    log_editor.SetReadOnly(false);

    TextEditor comp_errors;
    comp_errors.SetLanguageDefinition(lang);
    comp_errors.SetReadOnly(true);
    comp_errors.SetPalette(TextEditor::GetLightPalette());



    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();



        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {

            //layout variables
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            
            float code_editors_w = (float)display_w/2;
            float code_editors_h = display_h*0.7;

            float logs_w = display_w;
            float logs_h = display_h*0.3;





            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SetNextWindowSize(ImVec2(code_editors_w,code_editors_h));


            ImGui::Begin("Pseudocódigo origen",NULL,
            ImGuiConfigFlags_DpiEnableScaleViewports | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
            
            render_pseudo_editor(pseudoeditor, "Pseudocódigo Origen",&pseudo_errors,&cppeditor,&log_editor);
            pseudoeditor.SetErrorMarkers(pseudo_errors);
            ImGui::End();


            ImGui::SetNextWindowPos(ImVec2(code_editors_w, 0.0f));
            ImGui::SetNextWindowSize(ImVec2(code_editors_w,code_editors_h));

            ImGui::Begin("Cpp Generado",NULL,
            ImGuiConfigFlags_DpiEnableScaleViewports | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);

            //return true means user asked for compile
            render_cppeditor(cppeditor,log_editor,pseudo_errors,"Cpp generado",comp_errors);
            ImGui::End();



            ImGui::SetNextWindowPos(ImVec2(0,code_editors_h));
            ImGui::SetNextWindowSize(ImVec2(logs_w,logs_h));

            ImGui::Begin("Logs",NULL,
            ImGuiConfigFlags_DpiEnableScaleViewports | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);

            render_logs_editor(log_editor, "Logs");

            ImGui::End();


            
        }
        
        // Rendering
        ImGui::Render();
        
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    	

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
