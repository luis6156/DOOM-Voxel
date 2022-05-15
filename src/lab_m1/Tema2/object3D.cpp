#include "object3D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object3D::CreateCube(
    const std::string &name,
    float length,
    glm::vec3 color) {

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(glm::vec3(0, 0, length), color),
        VertexFormat(glm::vec3(length, 0, length), color),
        VertexFormat(glm::vec3(0, length, length), color),
        VertexFormat(glm::vec3(length, length, length), color),
        VertexFormat(glm::vec3(0, 0, 0), color),
        VertexFormat(glm::vec3(length, 0, 0), color),
        VertexFormat(glm::vec3(0, length, 0), color),
        VertexFormat(glm::vec3(length, length, 0), color)
    };
    
    std::vector<unsigned int> indices = { 
        0, 1, 2,
        1, 3, 2,
        2, 3, 7,
        2, 7, 6,
        1, 7, 3,
        1, 5, 7,
        6, 7, 4,
        7, 5, 4,
        0, 4, 1,
        1, 4, 5,
        2, 6, 4,
        0, 2, 4
    };

	/*Mesh* cube = new Mesh(name);
    cube->InitFromData(vertices, indices);*/
    return CreateMesh(name, vertices, indices);
}

Mesh* object3D::CreateCubeNoFill(
    const std::string& name,
    float length,
    glm::vec3 color) {

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(glm::vec3(0, 0, length), color),
        VertexFormat(glm::vec3(length, 0, length), color),
        VertexFormat(glm::vec3(0, length, length), color),
        VertexFormat(glm::vec3(length, length, length), color),
        VertexFormat(glm::vec3(0, 0, 0), color),
        VertexFormat(glm::vec3(length, 0, 0), color),
        VertexFormat(glm::vec3(0, length, 0), color),
        VertexFormat(glm::vec3(length, length, 0), color)
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        1, 3, 2,
        2, 3, 7,
        2, 7, 6,
        1, 7, 3,
        1, 5, 7,
        6, 7, 4,
        7, 5, 4,
        0, 4, 1,
        1, 4, 5,
        2, 6, 4,
        0, 2, 4
    };

    /*Mesh* cube = new Mesh(name);
    cube->InitFromData(vertices, indices);*/
    return CreateMesh(name, vertices, indices);
}

Mesh* object3D::CreateMesh(const std::string& name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
	unsigned int VAO = 0;
	// Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Create the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// ========================================================================
	// This section demonstrates how the GPU vertex shader program
	// receives data.

	// TODO(student): If you look closely in the `Init()` and `Update()`
	// functions, you will see that we have three objects which we load
	// and use in three different ways:
	// - LoadMesh   + LabShader (this lab's shader)
	// - CreateMesh + VertexNormal (this shader is already implemented)
	// - CreateMesh + LabShader (this lab's shader)
	// To get an idea about how they're different from one another, do the
	// following experiments. What happens if you switch the color pipe and
	// normal pipe in this function (but not in the shader)? Now, what happens
	// if you do the same thing in the shader (but not in this function)?
	// Finally, what happens if you do the same thing in both places? Why?

	// Set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// Set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// Set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// Set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	// ========================================================================

	// Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	Mesh* mesh = new Mesh(name);
	mesh->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
	mesh->vertices = vertices;
	mesh->indices = indices;
	return mesh;
}
