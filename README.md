# ArmaShaderTool
A Shader modification tool for Arma 3

```

Examples:

Dump shader assembly:
 ArmaShaderTool --dump -i "C:\Steam\Arma 3\dta\bin.pbo" -o P:\LodDiagDump.hlsl --shadername PSLODDiag --shadertype ps4
Replace one shader:
 ArmaShaderTool --replace -i "C:\Steam\Arma 3\dta\bin.pbo -o P:\new_bin.pbo --shadername PSLODDiag --shadertype ps4 --shaderfile P:\newshader.hlsl
Replace multiple shaders:
 ArmaShaderTool --replace -i "C:\Steam\Arma 3\dta\bin.pbo" -o P:\new_bin.pbo --shadername PSLODDiag --shadertype ps4 --shaderfile P:\newshaderps4.hlsl --shadername PSLODDiag --shadertype ps5 --shaderfile P:\newshaderps5.hlsl
List pixelshaders v5:
 ArmaShaderTool --list -i "C:\Steam\Arma 3\dta\bin.pbo" --shadertype ps4

Parameters:
    --dump     dumps one shader's assembly to file (doesn't support
               combined shadertypes) (can take -o argument to dump to file)
    --list     lists all shaders of specifc type (doesn't support combined
               shadertypes)
    --replace  replace shader with new hlsl

Main options:
    -i, --input -i "C:\Steam\Arma 3\dta\bin.pbo"
                                  The path to the input bin.pbo
    -o, --output -o "C:\Steam\Arma 3\dta\bin.pbo"
                                  The path to the new output file (will be a
                                  new bin.pbo)
    --help                        Print help
    --listshaders                 print a list of all shaders of <shadertype>


ShaderInfo options:  
    --shadername --shadername   PSLODDiag
                                Name of the shader you want to replace (See
                                listshaders)
    --shaderfile --shaderfile   "C:\Steam\Arma 3\dta\myshader.hlsl"
                                Path to a hlsl file that this shader should
                                be replaced with
    --shadertype --shadertype   ps45
                                Type of shader
                                (ps4,ps5,vs4,vs5,pp4,pp5,ps45,vs45,pp45) (combined shadertypes will compile
                                the file twice, with the "AST_TYPE" macro set
```