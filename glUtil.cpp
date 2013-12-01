#include "glUtil.h"
/*

static const GLenum types[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, 
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

static const char* pVSName = "VS";
static const char* pFSName = "FS";

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
                                                                                    \n\
out vec3 TexCoord0;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    vec4 WVP_Pos = gWVP * vec4(Position, 1.0);                                      \n\
    gl_Position = WVP_Pos.xyww;                                                     \n\
    TexCoord0   = Position;                                                         \n\
}";
 
static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec3 TexCoord0;                                                                  \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
uniform samplerCube gCubemapTexture;                                                \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = texture(gCubemapTexture, TexCoord0);                                \n\
}";

const char* Type2Name(GLuint type) {
    switch(type) {
        case GL_VERTEX_SHADER:
            return pVSName;
        case GL_FRAGMENT_SHADER:
            return pFSName;
        default:
            assert(0);
    }
    return NULL;
}
*/
Triangle::Triangle(Vector3f v1, Vector3f v2, Vector3f v3) {
    this->v1 = v1;
    this->v2 = v2;
    this->v3 = v3;
}

void Triangle::draw() {
    if (wireframe) {
        glBegin(GL_LINE_LOOP);
        glVertex3f(v1(0), v1(1), v1(2));
        glVertex3f(v2(0), v2(1), v2(2));
        glVertex3f(v3(0), v3(1), v3(2));
        glEnd();
    } else {
        glBegin(GL_TRIANGLES);
        Vector3f normal = (v1-v2).cross(v1-v3);
        if (normal.dot(Vector3f(0, 1, 0)) < 0) {
            normal *= -1;
        }
        glNormal3f(normal(0), normal(1), normal(2));
        glVertex3f(v1(0), v1(1), v1(2));
        glVertex3f(v2(0), v2(1), v2(2));
        glVertex3f(v3(0), v3(1), v3(2));
        glEnd();
    }
}

Tetrahedron::Tetrahedron(Vector3f *v1, Vector3f *v2, Vector3f *v3, Vector3f *v4) {
    this->v1 = v1; this->v2 = v2; this->v3 = v3; this->v4 = v4;
}

//Polygonization
void Tetrahedron::addTriangles(vector_tri *triangles, float (*func)(Vector3f)) {
    float v1v = func(*v1);
    float v2v = func(*v2);
    float v3v = func(*v3);
    float v4v = func(*v4);
    bool v1p = v1v > 0;
    bool v2p = v2v > 0;
    bool v3p = v3v > 0;
    bool v4p = v4v > 0;

    Vector3f cp1, cp2, cp3, cp4;

    if ((v1p&&v2p&&v3p&&v4p)||(!v1p&&!v2p&&!v3p&&!v4p)) {
        return;
    } else if ((!v1p&&v2p&&v3p&&v4p)||(v1p&&!v2p&&!v3p&&!v4p)) {
        cp1 = *v1+(*v2-*v1)*(-v1v/(v2v-v1v));
        cp2 = *v1+(*v3-*v1)*(-v1v/(v3v-v1v));
        cp3 = *v1+(*v4-*v1)*(-v1v/(v4v-v1v));
        triangles->push_back(Triangle(cp1, cp2, cp3));
    } else if ((v1p&&!v2p&&v3p&&v4p)||(!v1p&&v2p&&!v3p&&!v4p)) {
        cp1 = *v2+(*v1-*v2)*(-v2v/(v1v-v2v));
        cp2 = *v2+(*v3-*v2)*(-v2v/(v3v-v2v));
        cp3 = *v2+(*v4-*v2)*(-v2v/(v4v-v2v));
        triangles->push_back(Triangle(cp1, cp2, cp3));       
    } else if ((v1p&&v2p&&!v3p&&v4p)||(!v1p&&!v2p&&v3p&&!v4p)) {
        cp1 = *v3+(*v1-*v3)*(-v3v/(v1v-v3v));
        cp2 = *v3+(*v2-*v3)*(-v3v/(v2v-v3v));
        cp3 = *v3+(*v4-*v3)*(-v3v/(v4v-v3v));
        triangles->push_back(Triangle(cp1, cp2, cp3));
    } else if ((v1p&&v2p&&v3p&&!v4p)||(!v1p&&!v2p&&!v3p&&v4p)) {
        cp1 = *v4+(*v1-*v4)*(-v4v/(v1v-v4v));
        cp2 = *v4+(*v2-*v4)*(-v4v/(v2v-v4v));
        cp3 = *v4+(*v3-*v4)*(-v4v/(v3v-v4v));
        triangles->push_back(Triangle(cp1, cp2, cp3));
    } else if ((!v1p&&!v2p&&v3p&&v4p)||(v1p&&v2p&&!v3p&&!v4p)) {
        cp1 = *v1+(*v3-*v1)*(-v1v/(v3v-v1v));
        cp2 = *v1+(*v4-*v1)*(-v1v/(v4v-v1v));
        cp3 = *v2+(*v3-*v2)*(-v2v/(v3v-v2v));
        cp4 = *v2+(*v4-*v2)*(-v2v/(v4v-v2v));
        triangles->push_back(Triangle(cp1, cp2, cp3));
        triangles->push_back(Triangle(cp2, cp3, cp4)); 
        //(&tris2)->push_back(Triangle(cp1, cp2, cp3));
        //(&tris2)->push_back(Triangle(cp2, cp3, cp4));    
    } else if ((!v1p&&v2p&&!v3p&&v4p)||(v1p&&!v2p&&v3p&&!v4p)) {        
        cp1 = *v1+(*v2-*v1)*(-v1v/(v2v-v1v));
        cp2 = *v1+(*v4-*v1)*(-v1v/(v4v-v1v));
        cp3 = *v3+(*v2-*v3)*(-v3v/(v2v-v3v));
        cp4 = *v3+(*v4-*v3)*(-v3v/(v4v-v3v));           
        triangles->push_back(Triangle(cp1, cp2, cp3));
        triangles->push_back(Triangle(cp2, cp3, cp4));
        //(&tris2)->push_back(Triangle(cp1, cp2, cp3));
        //(&tris2)->push_back(Triangle(cp2, cp3, cp4)); 
    } else if ((!v1p&&v2p&&v3p&&!v4p)||(v1p&&!v2p&&!v3p&&v4p)) {
        cp1 = *v1+(*v2-*v1)*(-v1v/(v2v-v1v));
        cp2 = *v1+(*v3-*v1)*(-v1v/(v3v-v1v));
        cp3 = *v4+(*v2-*v4)*(-v4v/(v2v-v4v));
        cp4 = *v4+(*v3-*v4)*(-v4v/(v3v-v4v));
        triangles->push_back(Triangle(cp1, cp2, cp3));
        triangles->push_back(Triangle(cp2, cp3, cp4));
        //(&tris2)->push_back(Triangle(cp1, cp2, cp3));
        //(&tris2)->push_back(Triangle(cp2, cp3, cp4)); 
    }
}

/*
//               6       7         
//              +------+
//          5 .'|  8 .'|
//          +---+--+'  |
//          |   |  |   |           Vertices as labelled
//          |  2+--+---+3
//          | .'   | .'
//          +------+'
//          1       4 */

void Grid::addTriangles(vector_tri *triangles, float (*func)(Vector3f)) {
    for (int i = 0; i < numSteps(0)-1; i++) {
        for (int j = 0; j < numSteps(1)-1; j++) {
            for (int k = 0; k < numSteps(2)-1; k++) {
                int ic = numSteps(1)*numSteps(2);
                int jc = numSteps(2);

                Vector3f *v1 = &vertices[i*ic+j*jc+k];
                Vector3f *v2 = &vertices[i*ic+j*jc+k+1]; 
                Vector3f *v3 = &vertices[(i+1)*ic+j*jc+k+1];
                Vector3f *v4 = &vertices[(i+1)*ic+j*jc+k];
                Vector3f *v5 = &vertices[i*ic+(j+1)*jc+k];
                Vector3f *v6 = &vertices[i*ic+(j+1)*jc+k+1];
                Vector3f *v7 = &vertices[(i+1)*ic+(j+1)*jc+k+1];
                Vector3f *v8 = &vertices[(i+1)*ic+(j+1)*jc+k];

                Tetrahedron t1(v2, v1, v5, v4);
                Tetrahedron t2(v2, v4, v5, v8);
                Tetrahedron t3(v2, v5, v6, v8);
                Tetrahedron t4(v3, v6, v7, v8);
                Tetrahedron t5(v3, v2, v4, v8);
                Tetrahedron t6(v3, v2, v6, v8);

                //Tetrahedron t1(v5, v6, v8, v1);
                //Tetrahedron t2(v7, v6, v3, v8);
                //Tetrahedron t3(v2, v1, v6, v3);
                //Tetrahedron t4(v4, v8, v3, v1);
                //Tetrahedron t5(v1, v3, v6, v8);
                t1.addTriangles(triangles, func);
                t2.addTriangles(triangles, func);
                t3.addTriangles(triangles, func);
                t4.addTriangles(triangles, func);
                t5.addTriangles(triangles, func);
                t6.addTriangles(triangles, func);
            }
        }
    }   
}

// Only works for vectors with nonzero components
Grid::Grid(Vector3f center, Vector3f step, Vector3f gridMax) {
    Vector3f shiftToCenter = center - gridMax / 2;
    this->numSteps= Vector3i(gridMax(0)/step(0), gridMax(1)/step(1), gridMax(2)/step(2)) + Vector3i(1, 1, 1);
    for (int i = 0; i < numSteps(0); i++) {
        for (int j = 0; j < numSteps(1); j++) {
            for (int k = 0; k < numSteps(2); k++) {
                vertices.push_back(Vector3f(i*step(0), j*step(1), k*step(2)) + shiftToCenter);
            }
        }
    }
}
/*

//----------------------------------------------------------------------------------------------
//
//CubeMapTex Implementation
//
//----------------------------------------------------------------------------------------------

bool CubeMapTex::load() {
    glGenTextures(1, &m_textureObj);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);

    Magick::Image* pImage = NULL;
    Magick::Blob blob;

    for (int i = 0; i < (int)ARRAY_SIZE_IN_ELEMENTS(types); i++) {
        pImage = new Magick::Image(fileNames[i]);

        try {
            pImage->write(&blob, "RGBA");
        } catch (Magick::Error& error) {
            cout << "Error loading texture '" << fileNames[i] << "': " << error.what() << endl;
            delete pImage;
            return false;
        }

        glTexImage2D(types[i], 0, GL_RGB, pImage->columns(), pImage->rows(), 0,
            GL_RGBA, GL_UNSIGNED_BYTE, blob.data());

        delete pImage;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;

}

void CubeMapTex::bind(GLenum TextureUnit) {
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);
}

CubeMapTex::CubeMapTex(const string& Directory,
                               const string& PXFilename,
                               const string& NXFilename,
                               const string& PYFilename,
                               const string& NYFilename,
                               const string& PZFilename,
                               const string& NZFilename) {
    string::const_iterator it = Directory.end();
    it--;
    string BaseDir = (*it == '/') ? Directory : Directory + "/";
     
    fileNames[0] = BaseDir + PXFilename;
    fileNames[1] = BaseDir + NXFilename;
    fileNames[2] = BaseDir + PYFilename;
    fileNames[3] = BaseDir + NYFilename;
    fileNames[4] = BaseDir + PZFilename;
    fileNames[5] = BaseDir + NZFilename;
    m_textureObj = 0;
}

CubeMapTex::~CubeMapTex() {
    if (m_textureObj != 0) {
        glDeleteTextures(1, &m_textureObj);
    }
}

//----------------------------------------------------------------------------------------------
//
//Technique Implementation
//
//----------------------------------------------------------------------------------------------

Technique::Technique() {
    m_shaderProj = 0;
}

Technique::~Technique()
{
    // Delete the intermediate shader objects that have been added to the program
    // The list will only contain something if shaders were compiled but the object itself
    // was destroyed prior to linking.
    for (ShaderObjList::iterator it = m_shaderObjList.begin() ; it != m_shaderObjList.end() ; it++)
    {
        glDeleteShader(*it);
    }
 
    if (m_shaderProj != 0)
    {
        glDeleteProgram(m_shaderProj);
        m_shaderProj = 0;
    }
}

bool Technique::Init() {
    m_shaderProj = glCreateProgram();
 
    if (m_shaderProj == 0) {
        fprintf(stderr, "Error creating shader program\n");
        return false;
    }
 
    return true;
}

// Use this method to add shaders to the program. When finished - call finalize()
bool Technique::AddShader(GLenum Type, const char* pText)
{
    GLuint ShaderObj = glCreateShader(Type);
 
    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", Type);
        return false;
    }
 
    // Save the shader object - will be deleted in the destructor
    m_shaderObjList.push_back(ShaderObj);
 
    const GLchar* p[1];
    p[0] = pText;
    GLint Lengths[1];
    Lengths[0]= strlen(pText);
    glShaderSource(ShaderObj, 1, p, Lengths);
 
    glCompileShader(ShaderObj);
 
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
 
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling %s: '%s'\n", Type2Name(Type), InfoLog);
        return false;
    }
 
    glAttachShader(m_shaderProj, ShaderObj);
 
    return true;
}
 
 
// After all the shaders have been added to the program call this function
// to link and validate the program.
bool Technique::Finalize()
{
    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };
 
    glLinkProgram(m_shaderProj);
 
    glGetProgramiv(m_shaderProj, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(m_shaderProj, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        return false;
    }
 
    glValidateProgram(m_shaderProj);
    glGetProgramiv(m_shaderProj, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(m_shaderProj, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        return false;
    }
 
    // Delete the intermediate shader objects that have been added to the program
    for (ShaderObjList::iterator it = m_shaderObjList.begin() ; it != m_shaderObjList.end() ; it++)
    {
        glDeleteShader(*it);
    }
 
    m_shaderObjList.clear();
 
    return true;
}

void Technique::Enable()
{
    glUseProgram(m_shaderProj);
}
 
 
GLint Technique::GetUniformLocation(const char* pUniformName)
{
    GLint Location = glGetUniformLocation(m_shaderProj, pUniformName);
 
    if (Location == (GLint)0xFFFFFFFF) {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
    }
 
    return Location;
}

//----------------------------------------------------------------------------------------------
//
//SkyBoxTechnique Implementation
//
//----------------------------------------------------------------------------------------------

SkyboxTechnique::SkyboxTechnique() {   
}
 
bool SkyboxTechnique::Init() {
    if (!Technique::Init()) {
        return false;
    }
 
    if (!AddShader(GL_VERTEX_SHADER, pVS)) {
        return false;
    }
 
    if (!AddShader(GL_FRAGMENT_SHADER, pFS)) {
        return false;
    }
 
    if (!Finalize()) {
        return false;
    }
 
    m_WVPLocation = GetUniformLocation("gWVP");
    m_textureLocation = GetUniformLocation("gCubemapTexture");
  
    if (m_WVPLocation == INVALID_UNIFORM_LOCATION || 
        m_textureLocation == INVALID_UNIFORM_LOCATION) {
        return false;
    }
 
    return true;
}
 
 
void SkyboxTechnique::SetWVP(const Matrix4f& WVP) {
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat*)WVP.data());    
}
 
 
void SkyboxTechnique::SetTextureUnit(unsigned int TextureUnit) {
    glUniform1i(m_textureLocation, TextureUnit);
}

*/
