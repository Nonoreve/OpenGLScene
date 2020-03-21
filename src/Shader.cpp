#include "Shader.h"

Shader::Shader() : m_programID(0), m_vertexID(0), m_fragID(0)
{}

Shader::~Shader()
{
    glDeleteProgram(m_programID);
    glDeleteShader(m_vertexID);
    glDeleteShader(m_fragID);
}

Shader* Shader::loadFromFiles(FILE* vertexFile, FILE* fragFile)
{
    uint32_t vertexFileSize = 0;
    uint32_t fragFileSize   = 0;
    char* vertexCodeC;
    char* fragCodeC;

    /* Determine the vertex and fragment shader sizes */
    fseek(vertexFile, 0, SEEK_END);
    vertexFileSize = ftell(vertexFile);
    fseek(vertexFile, 0, SEEK_SET);

    fseek(fragFile, 0, SEEK_END);
    fragFileSize = ftell(fragFile);
    fseek(fragFile, 0, SEEK_SET);

    /* Read the files */
    vertexCodeC = (char*)malloc(vertexFileSize+1);
    fragCodeC   = (char*)malloc(fragFileSize+1);

    fread(vertexCodeC, 1, vertexFileSize, vertexFile);
    vertexCodeC[vertexFileSize] = '\0';
    fread(fragCodeC, 1, fragFileSize, fragFile);
    fragCodeC[fragFileSize] = '\0';

    /* Return the shader and free everything*/
    Shader* s = loadFromStrings(std::string(vertexCodeC), std::string(fragCodeC));

    free(vertexCodeC);
    free(fragCodeC);

    return s;
}

Shader* Shader::loadFromStrings(const std::string& vertexString, const std::string& fragString)
{
    Shader* shader = new Shader();

    /* Create a program and compile each shader component (vertex, fragment) */
    shader->m_programID = glCreateProgram();
    shader->m_vertexID = loadShader(vertexString, GL_VERTEX_SHADER);
    shader->m_fragID = loadShader(fragString, GL_FRAGMENT_SHADER);

    /* Attach the shader components to the program */
    glAttachShader(shader->m_programID, shader->m_vertexID);
    glAttachShader(shader->m_programID, shader->m_fragID);

    /* Do the attributes binding */
    shader->bindAttributes();

    /* Link the program. */
    glLinkProgram(shader->m_programID);

    /* Check for errors and print error message */
    int linkStatus;
    glGetProgramiv(shader->m_programID, GL_LINK_STATUS, &linkStatus);
    if(linkStatus == GL_FALSE)
    {
        char* error = (char*) malloc(ERROR_MAX_LENGTH * sizeof(char));
        int length=0;
        glGetProgramInfoLog(shader->m_programID, ERROR_MAX_LENGTH, &length, error);
        ERROR("Could not link shader-> : \n %s", error);

        delete shader;
        return NULL;
    }

    return shader;
}

int Shader::loadShader(const std::string& code, int type)
{
    /* Create a shader component and compile it */
    int shader = glCreateShader(type);
    const GLchar* s = code.c_str();
    glShaderSource(shader, 1, &s, 0);
    glCompileShader(shader);

    /* Check for errors and print error message */
    int compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if(compiled == GL_FALSE)
    {
        char* error = (char*) malloc(ERROR_MAX_LENGTH * sizeof(char));
        int length=0;
        glGetShaderInfoLog(shader, ERROR_MAX_LENGTH, &length, error);

        ERROR("Could not compile shader %d : \n %s", type, error);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

int Shader::getProgramID() const
{
    return m_programID;
}

int Shader::getVertexID() const
{
    return m_vertexID;
}

int Shader::getFragID() const
{
    return m_fragID;
}

void Shader::bindAttributes()
{
}
