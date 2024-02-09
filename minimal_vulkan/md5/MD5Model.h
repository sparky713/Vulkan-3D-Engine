#pragma once;

#include <vector>
#include <map>

#include <glm/gtc/quaternion.hpp>
#include <vulkan/vulkan.h>

#include "common.h"
#include "MD5Animation.h"
class MD5Animation;
class RenderInfo;

#include "game_object.h"

class MD5Model: public GameObject {
public:
    MD5Model();
    virtual ~MD5Model();

    bool LoadModel( const std::string& filename );
    bool LoadAnim( const std::string& filename );
    virtual void update( float fDeltaTime );
    void render(float fDeltaTime, RenderInfo* renderInfo);

    //void Update(float fDeltaTime);
    //void Render();

protected:
    typedef std::vector<glm::vec3> PositionBuffer;
    typedef std::vector<glm::vec3> NormalBuffer;
    typedef std::vector<glm::vec2> Tex2DBuffer;
    //typedef std::vector<GLuint> IndexBuffer;
    typedef std::vector<unsigned int> IndexBuffer;

    struct MD5Vertex
    {
        glm::vec3   m_Pos;
        glm::vec3   m_Normal;
        glm::vec2   m_Tex0;
        int         m_StartWeight;
        int         m_WeightCount;
    };
    typedef std::vector<MD5Vertex> VertexList;

    struct Triangle {
        int             m_Indices[3];
    };
    typedef std::vector<Triangle> TriangleList;

    struct Weight
    {
        int             m_JointID;
        float           m_Bias;
        glm::vec3       m_Pos;
    };
    typedef std::vector<Weight> WeightList;

    struct Joint {
        std::string     m_Name;
        int             m_ParentID;
        glm::vec3       m_Pos;
        glm::quat       m_Orient;
    };
    typedef std::vector<Joint> JointList;

public:
    struct Mesh {
        std::string     m_Shader;
        // This vertex list stores the vertices in the bind pose.
        VertexList      m_Verts;
        TriangleList    m_Tris;
        WeightList      m_Weights;

        // A texture ID for the material
        //GLuint          m_TexID;
        unsigned int          m_TexID; // loaded later using SOIL

        // These buffers are used for rendering the animated mesh
        PositionBuffer  m_PositionBuffer;   // Vertex position stream
        NormalBuffer    m_NormalBuffer;     // Vertex normals stream
        Tex2DBuffer     m_Tex2DBuffer;      // Texture coordinate set
        IndexBuffer     m_IndexBuffer;      // Vertex index buffer (like myFaces)
    };
    typedef std::vector<Mesh> MeshList;

protected:
    // Prepare the mesh for rendering
    // Compute vertex positions and normals
    bool PrepareMesh( Mesh& mesh );
    bool PrepareMesh( Mesh& mesh, const MD5Animation::FrameSkeleton& skel );
    bool PrepareNormals( Mesh& mesh );

    // Render a single mesh of the model
    // ALL RENDER MOVE TO VULKANCANVAS (recordCommandBuffer(...))
    void RenderMesh( const Mesh& mesh );
    void RenderNormals( const Mesh& mesh );

    // Draw the skeleton of the mesh for debugging purposes.
    void RenderSkeleton( const JointList& joints );
    
    bool CheckAnimation( const MD5Animation& animation ) const;
private:

    int                 m_iMD5Version;
    int                 m_iNumJoints;
    int                 m_iNumMeshes;

    bool                m_bHasAnimation;

    JointList           m_Joints;
    MeshList            m_Meshes;

    MD5Animation        m_Animation;

    glm::mat4x4         m_LocalToWorldMatrix;

    
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // sally
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    virtual void combineVertexAndIndexBuffer() = 0; // new sally
    virtual void createVertexBufferAndIndexBuffer() = 0; //called in LoadModel()
    virtual void fillVertexBufferAndIndexBuffer() = 0; //called in Update()

    virtual void init3DAPIMaterials() = 0; //called in LoadModel() (& Update()?)
    //virtual void initMyMD5Models(); //???
    //void initMyObjs();

    std::string shaderPathname = "models/";
public:
    int getNumMeshes();
    MeshList getMeshList();
    int getNumJoints();
    bool getHasAnim();

    virtual void print() = 0;
};
