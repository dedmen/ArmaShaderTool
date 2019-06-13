//Example pixelshader

struct inputType {
    //you need to find out what's in here by yourself
    float4 vPos                           : SV_POSITION;
};


//AST_VERSION macro is set to 4 or 5
//AST_TYPE macro is set to <shadertype>_<version> like ps_5_0 or vs_4_0, PP shaders are also ps_
//AST_NAME is the name of the shader, and doubles as the entry point name

float4 AST_NAME(inputType input) : SV_Target {
    return float4(1,0,0,1);
}