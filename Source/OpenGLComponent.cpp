/*
  ==============================================================================

    OpenGLComponent.cpp
    Created: 5 Feb 2021 11:23:59pm
    Author:  l33t h4x0r

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OpenGLComponent.h"

//==============================================================================
OpenGLComponent::OpenGLComponent()
{
    // Indicates that no part of this Component is transparent.
    setOpaque(true);
    
   

    // Set this instance as the renderer for the context.
    openGLContext.setRenderer(this);

    // Tell the context to repaint on a loop.
    openGLContext.setContinuousRepainting(true);

    // Finally - we attach the context to this Component.
    openGLContext.attachTo(*this);
}

OpenGLComponent::~OpenGLComponent()
{
    // Tell context to stop using this object
    openGLContext.detach();
}

void OpenGLComponent::paint(juce::Graphics& g)
{
    // this shit paints over the OpenGL context. Be warned!
}

void OpenGLComponent::resized()
{

}


void OpenGLComponent::newOpenGLContextCreated()
{
    // Generate 1 buffer, use vbo member variable to store its ID
    openGLContext.extensions.glGenBuffers( 1, &vertexBufferObject );
    
    // and same thing for the ibo member variable
    openGLContext.extensions.glGenBuffers( 1, &indexBufferObject );
    
    
    // Create 4 vertices each with a different colour.
    vertexBuffer =
    {
        // Vertex 0
        {
            { -0.5f, 0.5f },        // (-0.5, 0.5)
            { 1.f, 0.f, 0.f, 1.f }  // Red
        },
        // Vertex 1
        {
            { 0.5f, 0.5f },         // (0.5, 0.5)
            { 1.f, 0.5f, 0.f, 1.f } // Orange
        },
        // Vertex 2
        {
            { 0.5f, -0.5f },        // (0.5, -0.5)
            { 1.f, 1.f, 0.f, 1.f }  // Yellow
        },
        // Vertex 3
        {
            { -0.5f, -0.5f },       // (-0.5, -0.5)
            { 1.f, 0.f, 1.f, 1.f }  // Purple
        }
    };
    
    // We need 6 indices, 1 for each corner of the two triangles.
    indexBuffer =
    {
        0, 1, 2,
        0, 2, 3
    };
    
    // Bind the VBO.
    openGLContext.extensions.glBindBuffer( GL_ARRAY_BUFFER, vertexBufferObject );
    
    // Send the vertices data.
    openGLContext.extensions.glBufferData(
        GL_ARRAY_BUFFER,                        // The type of data we're sending.
        sizeof(Vertex) * vertexBuffer.size(),   // The size (in bytes) of the data.
        vertexBuffer.data(),                    // A pointer to the actual data.
        GL_STATIC_DRAW                          // How we want the buffer to be drawn.
    );
    
    // Bind the IBO.
    openGLContext.extensions.glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBufferObject );
    
    // Send the indices data.
    openGLContext.extensions.glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * indexBuffer.size(),
        indexBuffer.data(),
        GL_STATIC_DRAW
    );
    
    
    vertexShader =
    R"(
        #version 330 core
        
        // Input attributes.
        in vec4 position;
        in vec4 sourceColour;
        
        // Output to fragment shader.
        out vec4 fragColour;
        
        void main()
        {
            // Set the position to the attribute we passed in.
            gl_Position = position;
            
            // Set the frag colour to the attribute we passed in.
            // This value will be interpolated for us before the fragment
            // shader so the colours we defined ateach vertex will create a
            // gradient across the shape.
            fragColour = sourceColour;
        }
    )";
    
    fragmentShader =
        R"(
            #version 330 core
            
            // The value that was output by the vertex shader.
            // This MUST have the exact same name that we used in the vertex shader.
            in vec4 fragColour;
            
            void main()
            {
                // Set the fragment's colour to the attribute passed in by the
                // vertex shader.
                gl_FragColor = fragColour;
            }
        )";
    
    
    // Create an instance of OpenGLShaderProgram
    shaderProgram.reset(new juce::OpenGLShaderProgram(openGLContext));
    
    // Compile and link the shader.
    // Each of these methods will return false if something goes wrong so we'll
    // wrap them in an if statement
    if( shaderProgram->addVertexShader(vertexShader)
        && shaderProgram->addFragmentShader(fragmentShader)
        && shaderProgram->link() )
    {
        // No compilation errors - set the shader program to be active
        shaderProgram->use();
    }
    else
    {
        // Oops - something went wrong with our shaders!
        // Check the output window of your IDE to see what the error might be.
        jassertfalse;
    }
    
} // end of newOpenGLContextCreated()

void OpenGLComponent::renderOpenGL()
{
    // Clear the screen by filling it with black.
    juce::OpenGLHelpers::clear(juce::Colours::black);
    
    shaderProgram->use();
    
    openGLContext.extensions.glBindBuffer( GL_ARRAY_BUFFER, vertexBufferObject );
    openGLContext.extensions.glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBufferObject );
    
    
    // Enable the position attribute.
    openGLContext.extensions.glVertexAttribPointer(
        0,              // The attribute's index (AKA location).
        2,              // How many values this attribute contains.
        GL_FLOAT,       // The attribute's type (float).
        GL_FALSE,       // Tells OpenGL NOT to normalise the values.
        sizeof(Vertex), // How many bytes to move to find the attribute with
                        // the same index in the next vertex.
        nullptr         // How many bytes to move from the start of this vertex
                        // to find this attribute (the default is 0 so we just
                        // pass nullptr here).
    );
    
    openGLContext.extensions.glEnableVertexAttribArray(0);
    
    // Enable to colour attribute.
    openGLContext.extensions.glVertexAttribPointer(
        1,                              // This attribute has an index of 1
        4,                              // This time we have four values for the
                                        // attribute (r, g, b, a)
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (GLvoid*)(sizeof(float) * 2)    // This attribute comes after the
                                        // position attribute in the Vertex
                                        // struct, so we need to skip over the
                                        // size of the position array to find
                                        // the start of this attribute.
    );
    
    openGLContext.extensions.glEnableVertexAttribArray(1);
    
    
    glDrawElements(
        GL_TRIANGLES,       // Tell OpenGL to render triangles.
        indexBuffer.size(), // How many indices we have.
        GL_UNSIGNED_INT,    // What type our indices are.
        nullptr             // We already gave OpenGL our indices so we don't
                            // need to pass that again here, so pass nullptr.
    );
    
    openGLContext.extensions.glDisableVertexAttribArray(0);
    openGLContext.extensions.glDisableVertexAttribArray(1);
}

void OpenGLComponent::openGLContextClosing()
{
    
}
