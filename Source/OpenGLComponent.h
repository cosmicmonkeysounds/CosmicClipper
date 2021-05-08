/*
  ==============================================================================

    OpenGLComponent.h
    Created: 5 Feb 2021 11:23:59pm
    Author:  l33t h4x0r

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


//==============================================================================
/*
*/
class OpenGLComponent  : public juce::Component,
                         public juce::OpenGLRenderer
{
public:
    OpenGLComponent();
    ~OpenGLComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

private:
    
    juce::OpenGLContext openGLContext;
    
    struct Vertex
    {
        // size 2 because X, Y
        float position[2];
        
        // alpha, red, green, blue
        float colour[4];
    };
    
    std::vector<Vertex> vertexBuffer;
    std::vector<unsigned int> indexBuffer;
    
    // just unsigned ints! no worries
    GLuint vertexBufferObject, indexBufferObject;
    
    juce::String vertexShader, fragmentShader;
    
    std::unique_ptr<juce::OpenGLShaderProgram> shaderProgram;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGLComponent)
};
