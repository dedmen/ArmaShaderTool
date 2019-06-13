#include "ArmaShaderTool.h"
#include <fstream>
#include <string>
#include <set>
#include "pbo.hpp"
#include <optional>
#include <sstream>

struct ShaderInfo {
    std::string name;
    std::filesystem::path file;
    std::vector<shaderTarget> type;
};

std::shared_ptr<PboReader> inputFile;
std::unique_ptr<PboWriter> outputFile;

std::optional<PboEntryBuffer> getInputFile(std::string filename){
    auto& inputFiles = inputFile->getFiles();

    for (const PboEntry& it : inputFiles) { //#TODO find_if
        if (it.name == filename) return inputFile->getFileBuffer(it);
    }

    return {};
}

struct {
    std::filesystem::path inputPbo;
    std::filesystem::path outputPbo;

    std::vector<ShaderInfo> shaders;

    std::optional<ShaderInfo> dumpShader;
    std::optional<std::filesystem::path> dumpPath;
    std::optional<shaderTarget> listType;


    std::set<shaderTarget> collectTypes(){
        std::set<shaderTarget> ret;
        for (auto& it : shaders)
            for (auto& type : it.type)
                ret.insert(type);
        return ret;
    }
} cmdOptions;

void doReplaceType(shaderTarget type){
    std::string sourceName;
    switch (type){
        case shaderTarget::ps_5_0: sourceName = "Shaders_5_0_PS.shdc"sv; break;
        case shaderTarget::vs_5_0: sourceName = "Shaders_5_0_VS.shdc"sv; break;
        case shaderTarget::ps_4_0: sourceName = "Shaders_4_0_PS.shdc"sv; break;
        case shaderTarget::vs_4_0: sourceName = "Shaders_4_0_VS.shdc"sv; break;
        case shaderTarget::pp_5_0: sourceName = "Shaders_5_0_PP.shdc"sv; break;
        case shaderTarget::pp_4_0: sourceName = "Shaders_4_0_PP.shdc"sv; break;
        default: ;
    }

    auto inputBuffer = getInputFile(sourceName);
    if (!inputBuffer) {
        std::cerr << "input file is not a valid dta/bin.pbo" << std::endl;
        exit(0);
    }


    ShaderCacheFile file;
    std::istream input(&(*inputBuffer));
    file.read(input);

    for (auto& shader : cmdOptions.shaders) {
        if (std::find(shader.type.begin(), shader.type.end(), type) == shader.type.end()) continue; //Not correct type

        if (auto block = file.findBlock(shader.name)) {
            
            block->shaders.resize(1); //strip compressed
            block->replaceWith(shader.file, type);
        } else {
            std::cerr << "Shader (" <<shader.name << ") not found in " << sourceName << std::endl;
            exit(0);
        }
    }

    std::stringstream buf;
    file.write(buf);

    outputFile->addFile(std::make_shared<PboFTW_FromString>(sourceName, std::move(buf.str())));
}

void doDumpShader(shaderTarget type, std::string shaderName, std::optional<std::filesystem::path> outputPath){
    std::string sourceName;
    switch (type){
        case shaderTarget::ps_5_0: sourceName = "Shaders_5_0_PS.shdc"sv; break;
        case shaderTarget::vs_5_0: sourceName = "Shaders_5_0_VS.shdc"sv; break;
        case shaderTarget::ps_4_0: sourceName = "Shaders_4_0_PS.shdc"sv; break;
        case shaderTarget::vs_4_0: sourceName = "Shaders_4_0_VS.shdc"sv; break;
        case shaderTarget::pp_5_0: sourceName = "Shaders_5_0_PP.shdc"sv; break;
        case shaderTarget::pp_4_0: sourceName = "Shaders_4_0_PP.shdc"sv; break;
        default: ;
    }

    auto inputBuffer = getInputFile(sourceName);
    if (!inputBuffer) {
        std::cerr << "input file is not a valid dta/bin.pbo" << std::endl;
        exit(0);
    }


    ShaderCacheFile file;
    std::istream input(&(*inputBuffer));
    file.read(input);

    if (auto block = file.findBlock(shaderName)) {
        auto decompiledString = block->shaders[0].decompileToString();

        if (outputPath) {
            std::ofstream output(*outputPath);
            output << decompiledString;
        } else {
            std::cout << decompiledString;
        }
    } else {
        std::cerr << "Shader (" << shaderName << ") not found in " << sourceName << std::endl;
        exit(0);
    }
}

void doListShaders(shaderTarget type) {
    std::string sourceName;
    switch (type){
        case shaderTarget::ps_5_0: sourceName = "Shaders_5_0_PS.shdc"sv; break;
        case shaderTarget::vs_5_0: sourceName = "Shaders_5_0_VS.shdc"sv; break;
        case shaderTarget::ps_4_0: sourceName = "Shaders_4_0_PS.shdc"sv; break;
        case shaderTarget::vs_4_0: sourceName = "Shaders_4_0_VS.shdc"sv; break;
        case shaderTarget::pp_5_0: sourceName = "Shaders_5_0_PP.shdc"sv; break;
        case shaderTarget::pp_4_0: sourceName = "Shaders_4_0_PP.shdc"sv; break;
        default: ;
    }

    auto inputBuffer = getInputFile(sourceName);
    if (!inputBuffer) {
        std::cerr << "input file is not a valid dta/bin.pbo" << std::endl;
        exit(0);
    }


    ShaderCacheFile file;
    std::istream input(&(*inputBuffer));
    file.read(input);

    std::set<std::string> alreadyListed; //Don't list duplicates

    for (auto& it : file.blocks) {

        if (alreadyListed.count(it.sectionNames[1])) continue;
        alreadyListed.insert(it.sectionNames[1]);

        std::cout << it.sectionNames[1] << std::endl;
    }
}

#include "cxxopts.hpp"
#include "pbo.hpp"

cxxopts::ParseResult parse(int argc, char* argv[]) {


    std::vector<std::string> shaderNames;
    std::vector<std::string> shaderFiles;
    std::vector<std::string> shaderTypes;



    try {
        cxxopts::Options options("Arma Shader Tool", "");

        options
        .allow_unrecognised_options()
        .add_options("Main")
        ("i,input", "The path to the input bin.pbo", cxxopts::value<std::string>(), "-i \"C:\\Steam\\Arma 3\\dta\\bin.pbo\"")
        ("o,output", "The path to the new output file (will be a new bin.pbo)", cxxopts::value<std::string>(), "-o \"C:\\Steam\\Arma 3\\dta\\bin.pbo\"")
        
       
        ("help", "Print help")
        ("listshaders", "print a list of all shaders of <shadertype>")
        ;

        options.add_options("ShaderInfo")
            ("shadername", "Name of the shader you want to replace (See listshaders)", cxxopts::value(shaderNames), "--shadername PSLODDiag")
            ("shaderfile", "Path to a hlsl file that this shader should be replaced with", cxxopts::value(shaderFiles), "--shaderfile \"C:\\Steam\\Arma 3\\dta\\myshader.hlsl\"")
            ("shadertype", "Type of shader (ps4,ps5,vs4,vs5,pp4,pp5,ps45,vs45,pp45) (combined shadertypes will compile the file twice, with the \"AST_TYPE\" macro set", cxxopts::value(shaderTypes), "--shadertype ps45")
        ;


        options.add_options("")
            ("dump", "dumps one shader's assembly to file (doesn't support combined shadertypes) (can take -o argument to dump to file)", cxxopts::value<bool>())
            ("list", "lists all shaders of specifc type (doesn't support combined shadertypes)", cxxopts::value<bool>())
            ("replace", "replace shader with new hlsl", cxxopts::value<bool>())
        ;

        auto result = options.parse(argc, argv);

        std::string customHelp =
            "\nExamples\n"
            "\nDump shader assembly:\n ArmaShaderTool --dump -i \"C:\\Steam\\Arma 3\\dta\\bin.pbo\" -o P:\\LodDiagDump.hlsl --shadername PSLODDiag --shadertype ps4"
            "\nReplace one shader:\n ArmaShaderTool --replace -i \"C:\\Steam\\Arma 3\\dta\\bin.pbo -o P:\\new_bin.pbo --shadername PSLODDiag --shadertype ps4 --shaderfile P:\\newshader.hlsl"
            "\nReplace multiple shaders:\n ArmaShaderTool --replace -i \"C:\\Steam\\Arma 3\\dta\\bin.pbo\" -o P:\\new_bin.pbo --shadername PSLODDiag --shadertype ps4 --shaderfile P:\\newshaderps4.hlsl --shadername PSLODDiag --shadertype ps5 --shaderfile P:\\newshaderps5.hlsl"
            "\nList pixelshaders v5:\n ArmaShaderTool --list -i \"C:\\Steam\\Arma 3\\dta\\bin.pbo\" --shadertype ps4"
            "\n\n";
        options.custom_help(customHelp);


        if (result.count("help")) {
            std::cout << options.help({"", "Main", "ShaderInfo"}) << std::endl;
            exit(0);
        }

        if (result.count("i") == 1) {
            cmdOptions.inputPbo = result["i"].as<std::string>();
        } else {
            std::cout << "You need to define ONE input path" << std::endl;
            std::cout << options.help({"", "Main", "ShaderInfo"}) << std::endl;
            exit(0);
        }

        std::map<std::string, std::vector<shaderTarget>> shaderTypeMap;
        shaderTypeMap.insert({"ps4",{shaderTarget::ps_4_0}});
        shaderTypeMap.insert({"ps5",{shaderTarget::ps_5_0}});
        shaderTypeMap.insert({"vs4",{shaderTarget::vs_4_0}});
        shaderTypeMap.insert({"vs5",{shaderTarget::vs_5_0}});
        shaderTypeMap.insert({"pp4",{shaderTarget::pp_4_0}});
        shaderTypeMap.insert({"pp5",{shaderTarget::pp_5_0}});

        if (result.count("dump")) {
            if (result.count("shadername") != 1) {
                std::cout << "dump needs ONE shader argument" << std::endl;
                std::cout << options.help({"", "ShaderDump"}) << std::endl;
                exit(0);
            }

             if (result.count("shadertype") != 1) {
                std::cout << "dump needs ONE shader type argument" << std::endl;
                std::cout << options.help({"", "ShaderDump"}) << std::endl;
                exit(0);
            }

            std::string type = shaderTypes.front();

            if (shaderTypeMap.find(type) == shaderTypeMap.end()){
                std::cerr << "Shader type (" << type << ") is invalid" << std::endl;
                std::cout << options.help({"ShaderDump"}) << std::endl;
                exit(0);
            }

            cmdOptions.dumpShader = ShaderInfo {
                shaderNames.front(),
                "",
                shaderTypeMap[type]
            };
          
            if (result.count("o")) {
                cmdOptions.dumpPath = result["o"].as<std::string>();
            }

            return result;
        }

        if (result.count("list")) {
            if (result.count("shadertype") != 1) {
                std::cout << "list needs ONE shader type argument" << std::endl;
                std::cout << options.help({"", "ShaderDump"}) << std::endl;
                exit(0);
            }

            std::string type = shaderTypes.front();

            if (shaderTypeMap.find(type) == shaderTypeMap.end()){
                std::cerr << "Shader type (" << type << ") is invalid" << std::endl;
                std::cout << options.help({"ShaderDump"}) << std::endl;
                exit(0);
            }

            cmdOptions.listType = shaderTypeMap[type].front();

            return result;
        }



        if (result.count("o")) {
            cmdOptions.outputPbo = result["o"].as<std::string>();
        } else {
            std::cout << "You need to define ONE output path" << std::endl;
            std::cout << options.help({"", "Main", "ShaderInfo"}) << std::endl;
            exit(0);
        }

        if (shaderNames.size() != shaderFiles.size() || shaderFiles.size() != shaderTypes.size()){
            std::cout << "Every shader needs sn and sf and st arguments" << std::endl;
            std::cout << options.help({"", "Main", "ShaderInfo"}) << std::endl;
            exit(0);
        }


        shaderTypeMap.insert({"ps45",{shaderTarget::ps_4_0,shaderTarget::ps_5_0}});
        shaderTypeMap.insert({"vs45",{shaderTarget::vs_4_0,shaderTarget::vs_5_0}});
        shaderTypeMap.insert({"pp45",{shaderTarget::pp_4_0,shaderTarget::pp_5_0}});

        for (size_t i = 0; i < shaderNames.size(); i++) {


            std::string name = shaderNames[i];
            std::filesystem::path file = shaderFiles[i];
            std::string type = shaderTypes[i];

            if (shaderTypeMap.find(type) == shaderTypeMap.end()){
                std::cerr << "Shader type (" << type << ") is invalid" << std::endl;
                std::cout << options.help({"", "Main", "ShaderInfo"}) << std::endl;
                exit(0);
            }

            if (!std::filesystem::exists(file)){
                std::cerr << "Shader file (" << file << ") not found" << std::endl;
                std::cout << options.help({"", "Main", "ShaderInfo"}) << std::endl;
                exit(0);
            }


            ShaderInfo newShader;
            newShader.name = name;
            newShader.file = file;
            newShader.type = shaderTypeMap[type];

            cmdOptions.shaders.emplace_back(std::move(newShader));
        }


        return result;

    } catch (const cxxopts::OptionException& e) {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }
}

int main(int argc, char* argv[]) {

    auto result = parse(argc, argv);
    auto arguments = result.arguments();

    std::ifstream inputPbo(cmdOptions.inputPbo, std::ifstream::binary);

    inputFile = std::make_shared<PboReader>(inputPbo);
    inputFile->readHeaders();

    if (cmdOptions.dumpShader) {
        auto& dump = *cmdOptions.dumpShader;

        doDumpShader(dump.type.front(), dump.name, cmdOptions.dumpPath);
        return 0;
    }

    if (cmdOptions.listType) {
        doListShaders(*cmdOptions.listType);
        return 0;
    }

    outputFile = std::make_unique<PboWriter>();
    for (auto& it : inputFile->getProperties())
        outputFile->addProperty(it); //Copy properties

    auto types = cmdOptions.collectTypes();

    for (auto& type : types) {
        doReplaceType(type);
    }

    for (auto& f : inputFile->getFiles()) {
        auto found = std::find_if(outputFile->getFiles().begin(), outputFile->getFiles().end(), [name = f.name](const std::shared_ptr<PboFileToWrite>& file) {
            return file->getEntryInformation().name == name;
        });

        if (found == outputFile->getFiles().end()) {
            outputFile->addFile(std::make_shared<PboFTW_FromReader>(f.name, inputFile));
        }
    }

    std::ofstream output(cmdOptions.outputPbo, std::ofstream::binary);

    outputFile->writePbo(output);

    return 0;
}
