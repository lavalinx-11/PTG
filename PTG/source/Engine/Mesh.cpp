#include "Engine/Mesh.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include "Graphics/tiny_obj_loader.h"

Mesh::Mesh(const char* filename_)
    : dateLength{0} 
    , drawmode{0}
	, indexCount{0}
    , vao{0}
    , vbo{0}
	, ebo{0}

{
	filename = filename_;
}

Mesh::~Mesh() {}

bool Mesh::OnCreate() {
	LoadModel(filename);
    StoreMeshData(GL_TRIANGLES);
    return true;
}

void Mesh::LoadModel(const char* filename) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    vertices.clear();
    normals.clear();
	uvCoords.clear();

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename)) {
        throw std::runtime_error(warn + err);
    }
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vec3 vertex{};
            vertex.x = attrib.vertices[3 * index.vertex_index + 0];
            vertex.y = attrib.vertices[3 * index.vertex_index + 1];
            vertex.z = attrib.vertices[3 * index.vertex_index + 2];
            
            Vec3 normal{};
            normal.x = attrib.normals[3 * index.normal_index + 0];
            normal.y = attrib.normals[3 * index.normal_index + 1];
            normal.z = attrib.normals[3 * index.normal_index + 2];

            Vec2 uvCoord{};
            uvCoord.x = attrib.texcoords[2 * index.texcoord_index + 0];
            uvCoord.y = -attrib.texcoords[2 * index.texcoord_index + 1];

            vertices.push_back(vertex);
            normals.push_back(normal);
            uvCoords.push_back(uvCoord);
        }
    } 
}

void Mesh::StoreMeshData(GLenum drawmode_) {
    drawmode = drawmode_;
/// These just make the code easier for me to read
#define VERTEX_LENGTH 	(vertices.size() * (sizeof(Vec3)))
#define NORMAL_LENGTH 	(normals.size() * (sizeof(Vec3)))
#define TEXCOORD_LENGTH (uvCoords.size() * (sizeof(Vec2)))

	const int verticiesLayoutLocation = 0;
	const int normalsLayoutLocation = 1;
	const int uvCoordsLayoutLocation = 2;

	/// create and bind the VOA
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	/// Create and initialize vertex buffer object VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, VERTEX_LENGTH + NORMAL_LENGTH + TEXCOORD_LENGTH, NULL, GL_STATIC_DRAW);

	/// assigns the addr of "points" to be the beginning of the array buffer "sizeof(points)" in length
	glBufferSubData(GL_ARRAY_BUFFER, 0, VERTEX_LENGTH, &vertices[0]);
	/// assigns the addr of "normals" to be "sizeof(points)" offset from the beginning and "sizeof(normals)" in length  
	glBufferSubData(GL_ARRAY_BUFFER, VERTEX_LENGTH, NORMAL_LENGTH, &normals[0]);
	/// assigns the addr of "texCoords" to be "sizeof(points) + sizeof(normals)" offset from the beginning and "sizeof(texCoords)" in length  
	glBufferSubData(GL_ARRAY_BUFFER, VERTEX_LENGTH + NORMAL_LENGTH, TEXCOORD_LENGTH, &uvCoords[0]);

	glEnableVertexAttribArray(verticiesLayoutLocation);
	glVertexAttribPointer(verticiesLayoutLocation, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(normalsLayoutLocation);
	glVertexAttribPointer(normalsLayoutLocation, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(VERTEX_LENGTH));
	glEnableVertexAttribArray(uvCoordsLayoutLocation);
	glVertexAttribPointer(uvCoordsLayoutLocation, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(VERTEX_LENGTH + NORMAL_LENGTH));

    dateLength = vertices.size();

    /// give back the memory used in these vectors. The data is safely stored in the GPU now
    vertices.clear();
    normals.clear();
    uvCoords.clear();

    /// Don't need these defines sticking around anymore si undefine them. 
#undef VERTEX_LENGTH
#undef NORMAL_LENGTH
#undef TEXCOORD_LENGTH

}

void Mesh::Render() const {
    glBindVertexArray(vao);

    if (ebo != 0) {
        glDrawElements(drawmode, indexCount, GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(drawmode, 0, dateLength);
    }

	glBindVertexArray(0); // unbind the vao
}

void Mesh::Render(GLenum drawmode_) const {
    glBindVertexArray(vao);

    if (ebo != 0) {
        glDrawElements(drawmode_, indexCount, GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(drawmode_, 0, dateLength);
    }

	glBindVertexArray(0); //unbind the vao
}


// Terrain Mesh Data version of OnCreate
bool Mesh::OnCreate(const TerrainMeshData& data)
{
    
    indexCount = data.indices.size();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    struct Vertex {
        Vec3 position;
        Vec3 normal;
        Vec2 uv;
    };

    std::vector<Vertex> packedVertices;
    packedVertices.reserve(data.vertices.size());

    for (size_t i = 0; i < data.vertices.size(); i++) {
        packedVertices.push_back({
            data.vertices[i],
            data.normals[i],
            data.uvs[i]
            });
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        packedVertices.size() * sizeof(Vertex),
        packedVertices.data(),
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        data.indices.size() * sizeof(unsigned int),
        data.indices.data(),
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, uv));

    glBindVertexArray(0);
	packedVertices.clear();
    return true;
}

void Mesh::OnDestroy() {
    if(ebo != 0) {
        glDeleteBuffers(1, &ebo);
	}
    glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

/// Currently unused.
void Mesh::Update(const float deltaTime) {}